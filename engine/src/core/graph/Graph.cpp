// Created by borja on 9/3/22.

#include "core/graph/Graph.h"
#include "core/graph/components/Transform.h"
#include "core/graph/components/Body.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/TextRenderer.h"
#include "core/systems/animationSystem/AnimationSystem.h"
#include "core/graph/Scene.h"
#include "core/Engine.h"

namespace GDE {

/*    template<typename... Type>
    entt::type_list<Type...> as_type_list(const entt::type_list<Type...> &);

    template<typename Entity>
    struct PolyStorage: entt::type_list_cat_t<
            decltype(as_type_list(std::declval<entt::storage<Entity>>())),
            entt::type_list<void(entt::basic_registry<Entity> &, const Entity, const Entity)>
    > {
        template<typename Base>
        struct type: entt::storage<Entity>::template type<Base> {
            static constexpr auto base = decltype(as_type_list(std::declval<entt::storage<Entity>>()))::size;

            void copy_to(entt::basic_registry<Entity> &reg, const Entity from, const Entity to) {
                entt::poly_call<base + 0>(*this, reg, from, to);
            }
        };

        template<typename Type>
        static void copy_to(Type &pool, entt::basic_registry<Entity> &reg, const Entity from, const Entity to) {
            pool.emplace(reg, to, pool.get(from));
        }

        template<typename Type>
        using impl = entt::value_list_cat_t<
                typename entt::storage<Entity>::template impl<Type>,
                entt::value_list<&copy_to<Type>>
        >;
    };
*/

    /// All of these are foo functions as Delegates need to have something assigned to be called.
    static void onEventDelFoo(NodeContainer&, Event& _event) {  }
    static void onUpdateDelFoo(NodeContainer&, Delta) {  }
    static void onFixedUpdateDelFoo(NodeContainer&, Delta) {  }
    static void onRenderDelFoo(NodeContainer&) {  }

    Graph::Graph(Scene* _scene, const std::string& _sceneName) {
        scene = _scene;
        name = _sceneName;
        sceneRoot = registry.create();

        registry.emplace<Tag>(sceneRoot, sceneRoot, _sceneName);
        registry.emplace<Transform>(sceneRoot, sceneRoot).parent = NODE_ID_NULL;
        registry.get<Transform>(sceneRoot).parentTransform = nullptr;
        registry.emplace<Active>(sceneRoot, sceneRoot);

        onEventDel.bind<&onEventDelFoo>();
        onUpdateDel.bind<&onUpdateDelFoo>();
        onFixedUpdateDel.bind<&onFixedUpdateDelFoo>();
        onRenderDel.bind<&onRenderDelFoo>();
    }

    void Graph::onEvent(Event& _event) {
        EventDispatcher _eventDispatcher(_event);

        for(auto* _canvas : scene->canvases) {
            _canvas->onEvent(_eventDispatcher, _event);
        }

        onEventDel(registry, _event);
    }

    void Graph::onUpdate(Delta _dt) {
        registry.view<Transform, Active>(entt::exclude<StaticTransform>).each([&](const auto _entity, Transform& _transform, const Active& _) {
            _transform.update();
            if(_transform.staticTransform) {
                addComponent<StaticTransform>(_entity);
            }
        });

        registry.group<AnimationSystem>(entt::get<SpriteRenderer, Active>).each([&_dt](const auto _entity, AnimationSystem& _animationSystem, SpriteRenderer& _spriteRenderer, const Active& _) {
            _animationSystem.update(_dt, _spriteRenderer);
        });

        registry.view<Transform, Body, Active>(entt::exclude<StaticTransform>).each([](const auto _entity, Transform& _transform, Body& _body, const Active& _) {
            auto _lastStoredPosition = _body.b2dConfig.lastPosition;
            auto _diff = _transform.getPositionLocal() - _lastStoredPosition;

            if(_diff != 0) {
                auto _bodyTransform = _body.b2dConfig.body->GetTransform();
                _body.b2dConfig.body->SetTransform({_bodyTransform.p.x + _diff.x, _bodyTransform.p.y + _diff.y}, _bodyTransform.q.GetAngle());
            }

            _transform.setPositionWorld(_body.getPosition());
            _transform.setRotation(_body.getRotation());
            _body.b2dConfig.lastPosition = _transform.getPositionLocal();
        });

        registry.group<ParticleSystem>(entt::get<Active>).each([&_dt](const auto _entity, ParticleSystem& _particleSystem, const Active& _) {
            _particleSystem.update(_dt);
        });

        onUpdateDel(registry, _dt);

        for(auto* _canvas : scene->canvases) {
            _canvas->onUpdate(_dt);
        }
    }

    void Graph::onFixedUpdate(Delta _dt) {

        onFixedUpdateDel(registry, _dt);
    }

    void Graph::onRender() {
        auto _spriteRendererGroup = registry.group<const SpriteRenderer>(entt::get<Transform, Active>);
        auto _particleSystemGroup = registry.group<const ParticleSystem>(entt::get<Transform, Active>);
        auto _textRendererGroup = registry.group<TextRenderer>(entt::get<Transform, Active>);

        auto& _renderManager = scene->engine->manager.renderManager;

        for(auto* _camera : scene->cameras) {
            if(!hasComponent<Active>(_camera->ID)) continue;

            _renderManager.beginDraw(*_camera, getComponent<Transform>(_camera->ID));
            _camera->setCameraSize(_camera->getCameraSize());
            {
                _spriteRendererGroup.each([&_renderManager](const auto _entity, const SpriteRenderer& _renderizable, const Transform& _transform, const Active& _) {
                    _renderManager.draw((const IRenderizable*)&_renderizable, _transform);
                });

                _particleSystemGroup.each([&_renderManager](const auto _entity, const ParticleSystem& _renderizable, const Transform& _transform, const Active& _) {
                    _renderManager.draw((const IRenderizable*)&_renderizable, _transform);
                });

                _textRendererGroup.each([&_renderManager](const auto _entity, TextRenderer& _text, const Transform& _transform, const Active& _) {
                    _renderManager.draw((const IRenderizable*)&_text, _transform);
                });
            }

            onRenderDel(registry);
            _renderManager.endDraw();
        }

        for(auto* _canvas : scene->canvases) {
            _canvas->onRender();
        }
    }

    void Graph::onDebugRender() {
        auto _debug = registry.view<Body>();
        if(_debug.empty()) return;

        auto& _renderManager = scene->engine->manager.renderManager;

        _renderManager.beginDebugDraw(*scene->mainCamera, getComponent<Transform>(scene->mainCamera->ID));

        _debug.each([&_renderManager](const auto _entity, const Body& _body) {
            _renderManager.drawSquare(_body.getPosition(), _body.bodyConfig.size, {Color::Green.r, Color::Green.g, Color::Green.b, 100}, _body.getRotation());
        });

        _renderManager.endDebugDraw();
    }

    NodeID Graph::createNode(const std::string& _tag, const NodeID& _parent) {
        auto _newNode = registry.create();

        auto _parentRef = _parent == NODE_ID_NULL ? sceneRoot : _parent;

        registry.emplace<Tag>(_newNode, _newNode, _tag);
        registry.emplace<Transform>(_newNode, _newNode).parent = _parentRef;
        (&registry.get<Transform>(_parentRef))->children.push_back(_newNode);
        (&registry.get<Transform>(_newNode))->parentTransform = (&registry.get<Transform>(_parentRef));

        registry.emplace<Active>(_newNode, _newNode);

        return _newNode;
    }

    NodeID Graph::instantiatePrefab(const NodeID& _prefab, const Vec2F& _position, const NodeID& _parent) {
        auto _copy = registry.create();

        for(auto&& _curr: registry.storage()) {
            auto& _storage = _curr.second;
            if(_storage.contains(_prefab)) {
                _storage.emplace(_copy, _storage.get(_prefab));
            }
        }

        auto* _transform = getComponent<Transform>(_copy);
        (&registry.get<Transform>(getComponent<Transform>(_prefab)->parent))->children.push_back(_copy);
        _transform->setPosition(_position);
        _transform->update();
        if(_parent != NODE_ID_NULL) setParent(_copy, _parent);
        setNodeActive(_copy, true);

        return _copy;
    }

    NodeID Graph::getNode(const std::string& _tagName) {
        for(auto [_entity, _tag] : registry.view<Tag>().each())
            if(strcmp(_tag.tag.c_str(), _tagName.c_str()) == 0) return _entity;

        throw std::runtime_error("Tried to get node with tag '" + _tagName + "' but it was not found!");
    }

    void Graph::removeNode(const NodeID& _node) {
        remove(_node, false);
    }

    void Graph::removeNode(const std::string& _nodeTagName) {
        removeNode(getNode(_nodeTagName));
    }

    void Graph::printScene(const entt::entity& _id, std::ostream& _os, int& _indent) {
        if(_id == NODE_ID_NULL) return;

        auto _tag = registry.get<Tag>(_id);
        _os << "* " << _tag.tag;

        auto _transform = registry.get<Transform>(_id);

        for(auto _child : _transform.children) {
            _indent++;
            _os << "\n";
            for(int _in = 0; _in < _indent; _in++) _os << "\t";
            _os << "|__ ";
            printScene(_child, _os, _indent);
            _indent--;
        }
    }

    std::string Graph::toString() {
        std::stringstream _ss;
        int _indent = -1;
        printScene(sceneRoot, _ss, _indent);
        return _ss.str();
    }

    void Graph::setParent(const NodeID& _node, const NodeID& _parent) {
        auto* _nodeTransform = &registry.get<Transform>(_node);

        if(_nodeTransform->parent == _parent) {
            LOG_W("Parent of ", (int)_node, " is already ", (int)_parent, "!!")
            return;
        }

        if(std::find(_nodeTransform->children.begin(), _nodeTransform->children.end(), _parent) != _nodeTransform->children.end()) {
            LOG_E("Cannot set ", (int)_parent, " as the parent of ", (int)_node, " because ", (int)_node, " is the father of ", (int)_parent, "!!!!")
            return;
        }

        remove(_node, false);
        _nodeTransform->parent = _parent;
        auto* _parentTransform = &registry.get<Transform>(_parent);
        _parentTransform->children.push_back(_node);
    }

    void Graph::remove(const NodeID& _node, bool _delete) {
        auto* _nodeTransform = &registry.get<Transform>(_node);

        if(_nodeTransform->parent != NODE_ID_NULL) {
            auto* _parentTransform = &registry.get<Transform>(_nodeTransform->parent);
            auto _toRemove = std::remove_if(_parentTransform->children.begin(), _parentTransform->children.end(), [&](const NodeID& _nodeID) {
                return _nodeID == _node;
            });
            _parentTransform->children.erase(_toRemove);
        }

        if(_delete) {
            for(auto _child : _nodeTransform->children) {
                remove(_child, _delete);
            }

            registry.destroy(_node);
        }

    }

    void Graph::orphan(const NodeID& _node) {
        remove(_node, false);
        (&registry.get<Transform>(_node))->parent = sceneRoot;
        (&registry.get<Transform>(sceneRoot))->children.push_back(_node);
    }

    void Graph::orphan(const std::string& _nodeTagName) {
        orphan(getNode(_nodeTagName));
    }

    NodeID Graph::getID() {
        return sceneRoot;
    }

    void Graph::setNodeStatic(NodeID _node, bool _static) {
        if(_static && !registry.any_of<StaticTransform>(_node)) {
            registry.emplace<StaticTransform>(_node, _node);
            return;
        }

        if(!_static && registry.any_of<StaticTransform>(_node))
            registry.remove<StaticTransform>(_node);
    }

    bool Graph::isNodeStatic(NodeID _node) {
        return registry.any_of<StaticTransform>(_node);
    }

    void Graph::setNodeActive(NodeID _node, bool _active) {
        if(_active && !registry.any_of<Active>(_node)) {
            registry.emplace<Active>(_node, _node);
            return;
        }

        if(!_active && registry.any_of<Active>(_node))
            registry.remove<Active>(_node);
    }

    bool Graph::isNodeActive(NodeID _node) {
        return registry.any_of<Active>(_node);
    }

    NodeContainer& Graph::getNodeContainer() {
        return registry;
    }
}