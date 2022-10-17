// Created by borja on 9/3/22.

#include "core/graph/Graph.h"
#include "core/graph/components/Transform.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/TextRenderer.h"
#include "core/systems/animationSystem/AnimationSystem.h"
#include "core/graph/Scene.h"
#include "core/Engine.h"
#include "core/util/GLUtil.h"

namespace GDE {
    
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
            _canvas->onEvent(scene->engine, _eventDispatcher, _event);
        }

        onEventDel(registry, _event);
    }

    void Graph::onUpdate(Delta _dt) {
        registry.group<AnimationSystem>(entt::get<SpriteRenderer, Active>).each([&_dt](const auto _entity, AnimationSystem& _animationSystem, SpriteRenderer& _spriteRenderer, const Active& _) {
            _animationSystem.update(_dt, _spriteRenderer);
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

        for(auto* _body : scene->engine->manager.physics.bodies) {
            _body->update();
        }
    }

    void Graph::onRender() {
        auto _spriteRendererGroup = registry.view<const SpriteRenderer,Transform, Active>(entt::exclude<StaticTransform>);
        auto _particleSystemGroup = registry.group<const ParticleSystem>(entt::get<Transform, Active>);
        auto _textRendererGroup = registry.group<TextRenderer>(entt::get<Transform, Active>);

        auto& _renderManager = scene->engine->manager.renderManager;

        for(auto* _camera : scene->cameras) {
            if(!hasComponent<Active>(_camera->ID)) continue;

            _renderManager.beginDraw(*_camera, getComponent<Transform>(_camera->ID));
            _camera->setCameraSize(_camera->getCameraSize());
            {
                _spriteRendererGroup.each([&_renderManager](const auto _entity, const SpriteRenderer& _spriteRenderer, Transform& _transform, const Active& _) {
                    _renderManager.draw((IRenderizable*) &_spriteRenderer, _transform);
                });

                _particleSystemGroup.each([&_renderManager](const auto _entity, const ParticleSystem& _particleSystem, Transform& _transform, const Active& _) {
                    _renderManager.draw((IRenderizable*) &_particleSystem, _transform);
                });

                _textRendererGroup.each([&_renderManager](const auto _entity, TextRenderer& _text, Transform& _transform, const Active& _) {
                    _renderManager.draw((IRenderizable*) &_text, _transform);
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
//        auto _debug = registry.view<Body>();
//        if(_debug.empty() || !scene->engine->manager.physics.drawDebugInfo) return;
//
        auto& _renderManager = scene->engine->manager.renderManager;
//
        _renderManager.beginDebugDraw(*scene->mainCamera, getComponent<Transform>(scene->mainCamera->ID));
        scene->engine->manager.physics.debugRender(&_renderManager);
//
//        _debug.each([&_renderManager](const auto _entity, const Body& _body) {
//            if(_body.bodyConfig.bodyShapeType == BodyShapeType::BOX) {
//                _renderManager.drawSquare(_body.getPosition(), _body.bodyConfig.size, {Color::Green.r, Color::Green.g, Color::Green.b, 100}, _body.getRotation());
//            } else if (_body.bodyConfig.bodyShapeType == BodyShapeType::CIRCLE) {
////                Shape _circle;
////                _circle.makeCircle(_body.getPosition(), _body.bodyConfig.size.x / 2.f * 0.4f, 400);
////                _circle.showInnerColor(false);
////                _renderManager.drawShape(_circle);
//            }
//        });
//
        _renderManager.endDebugDraw();
    }

    NodeID Graph::createNode(const std::string& _tag, const NodeID& _parent) {
        auto _newNode = registry.create();

        auto _parentRef = _parent == NODE_ID_NULL ? sceneRoot : _parent;

        registry.emplace<Tag>(_newNode, _newNode, _tag);
        registry.emplace<Transform>(_newNode, _newNode).parent = _parentRef;
        (&registry.get<Transform>(_parentRef))->children.push_back(&registry.get<Transform>(_newNode));
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
        (&registry.get<Transform>(getComponent<Transform>(_prefab)->parent))->children.push_back(_transform);
        _transform->setPosition(_position);
        _transform->parentTransform = (&registry.get<Transform>(getComponent<Transform>(_prefab)->parent));
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
        remove(_node, true);
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
            printScene(_child->ID, _os, _indent);
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

        if(std::find(_nodeTransform->begin(), _nodeTransform->end(), getComponent<Transform>(_parent)) != _nodeTransform->end()) {
            LOG_E("Cannot set ", (int)_parent, " as the parent of ", (int)_node, " because ", (int)_node, " is the father of ", (int)_parent, "!!!!")
            return;
        }

        remove(_node, false);

        auto* _parentTransform = &registry.get<Transform>(_parent);

        _nodeTransform->parent = _parent;
        _nodeTransform->parentTransform = _parentTransform;
        _parentTransform->children.push_back(_nodeTransform);
        _nodeTransform->setLocalMatrix(glm::inverse(_parentTransform->getLocalMatrix()) * _nodeTransform->getLocalMatrix());
    }

    void Graph::remove(const NodeID& _node, bool _delete) {
        auto* _nodeTransform = &registry.get<Transform>(_node);

        if(_nodeTransform->parent != NODE_ID_NULL) {
            auto* _parentTransform = &registry.get<Transform>(_nodeTransform->parent);
            auto _toRemove = std::remove_if(_parentTransform->children.begin(), _parentTransform->children.end(), [&](Transform* _transform) {
                return _transform->ID == _node;
            });
            _parentTransform->children.erase(_toRemove);
        }

        if(_delete) {
            for(auto _it = _nodeTransform->children.rbegin(); _it < _nodeTransform->children.rend(); _it++) {
                remove((*_it)->ID, _delete);
            }

            if(hasComponent<PhysicsBody>(_node)) {
                scene->engine->manager.physics.remove(getComponent<PhysicsBody>(_node));
            }

            registry.destroy(_node);
        }

    }

    void Graph::orphan(const NodeID& _node) {
        remove(_node, false);
        (&registry.get<Transform>(_node))->parent = sceneRoot;
        (&registry.get<Transform>(_node))->parentTransform = getComponent<Transform>(sceneRoot);
        (&registry.get<Transform>(sceneRoot))->children.push_back(&registry.get<Transform>(_node));
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