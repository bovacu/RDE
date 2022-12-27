// Created by borja on 9/3/22.

#include "core/graph/Graph.h"
#include "core/graph/components/Transform.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/TextRenderer.h"
#include "core/systems/animationSystem/AnimationSystem.h"
#include "core/graph/Scene.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/DynamicSpriteRenderer.h"
#include "core/graph/components/ui/UI.h"

namespace RDE {
    
    /// All of these are foo functions as Delegates need to have something assigned to be called.
    static void onEventDelFoo(NodeContainer&, Event& _event) {  }
    static void onUpdateDelFoo(NodeContainer&, Delta) {  }
    static void onFixedUpdateDelFoo(NodeContainer&, Delta) {  }
    static void onRenderDelFoo(NodeContainer&) {  }

    Graph::Graph(Scene* _scene, const std::string& _sceneName, bool _isIUI) : isUI(_isIUI) {
        scene = _scene;
        name = _sceneName;
        auto _sceneRootID = registry.create();

        if(!_isIUI) {
            registry.emplace<Tag>(_sceneRootID, _sceneRootID, _sceneName);
            registry.emplace<Transform>(_sceneRootID, this).parent = NODE_ID_NULL;
            sceneRoot = &registry.emplace<Node>(_sceneRootID, _sceneRootID, this, &_scene->engine->manager, &registry.get<Transform>(_sceneRootID));
            registry.get<Transform>(_sceneRootID).parentTransform = nullptr;
            registry.get<Transform>(_sceneRootID).node = sceneRoot;
            registry.emplace<Active>(_sceneRootID, sceneRoot, &_scene->engine->manager, this);
        } else {
            registry.emplace<Tag>(_sceneRootID, _sceneRootID, _sceneName);
            registry.emplace<UITransform>(_sceneRootID, this).parent = NODE_ID_NULL;
            sceneRoot = &registry.emplace<Node>(_sceneRootID, _sceneRootID, this, &_scene->engine->manager, (Transform*)&registry.get<UITransform>(_sceneRootID));
            registry.get<UITransform>(_sceneRootID).parentTransform = nullptr;
            registry.get<UITransform>(_sceneRootID).node = sceneRoot;
            registry.emplace<Active>(_sceneRootID, sceneRoot, &_scene->engine->manager, this);

            auto* _interactable = &registry.emplace<UIInteractable>(_sceneRootID, sceneRoot, &scene->engine->manager, this);
            registry.get<UITransform>(_sceneRootID).setSize({ (float)scene->mainCamera->getViewport()->getVirtualResolution().x, (float)scene->mainCamera->getViewport()->getVirtualResolution().y });
            _interactable->interactable = false;
            _interactable->focused = false;
        }

        onEventDel.bind<&onEventDelFoo>();
        onUpdateDel.bind<&onUpdateDelFoo>();
        onFixedUpdateDel.bind<&onFixedUpdateDelFoo>();
        onRenderDel.bind<&onRenderDelFoo>();
    }

    void Graph::onEvent(Event& _event) {
        onEventDel(registry, _event);
    }

    void Graph::onUpdate(Delta _dt) {
        // Is DisabledForRender because that is the flag used by renderizable elements to be enabled/disabled.
        auto _animations = registry.view<AnimationSystem, SpriteRenderer, Active>(entt::exclude<DisabledForRender>);
        auto _particleSystems = registry.view<ParticleSystem, Active>(entt::exclude<DisabledForRender>);

        _animations.each([&_dt](const auto _entity, AnimationSystem& _animationSystem, SpriteRenderer& _spriteRenderer, const Active& _) {
            _animationSystem.update(_dt, &_spriteRenderer);
        });

        _particleSystems.each([&_dt](const auto _entity, ParticleSystem& _particleSystem, const Active& _) {
            _particleSystem.update(_dt);
        });

        onUpdateDel(registry, _dt);
    }

    void Graph::onFixedUpdate(Delta _dt) {
        onFixedUpdateDel(registry, _dt);

        for(auto* _body : scene->engine->manager.physics.bodies) {
            if(!_body->isEnabled()) continue;
            _body->update();
        }
    }

    void Graph::onRender() {
        auto& _renderManager = scene->engine->manager.renderManager;

        for(auto* _camera : scene->cameras) {
            if(!hasComponent<Active>(_camera->node->getID())) continue;

            _renderManager.beginDraw(_camera, getComponent<Transform>(_camera->node->getID()));
            _camera->update();
            {
                for(auto* _renderizable : renderizableTree) {
                    _renderManager.draw(_renderizable, *_renderizable->node->getTransform());
                }
            }

            onRenderDel(registry);
            _renderManager.endDraw();
        }

        postRenderSync();
    }

    void Graph::onLateUpdate(Delta _dt) {
        if(isRenderizableTreeDirty) {

            renderizableTree.clear();

            // [0] -> SpriteRenderer
            // [1] -> ParticleSystem
            // [2] -> TextRenderer
            const int RENDERIZABLES_COUNT = 3;
            std::vector<IRenderizable*> _renderizables[RENDERIZABLES_COUNT];
            recalculateRenderizableTree(sceneRoot, _renderizables);

            int _totalElementsToReserve = 0;
            for(auto _i = 0; _i < RENDERIZABLES_COUNT; _i++) {
                _totalElementsToReserve += (int)_renderizables[_i].size();
            }

            renderizableTree.reserve(_totalElementsToReserve);

            for(auto _i = 0; _i < RENDERIZABLES_COUNT; _i++) {
                renderizableTree.insert(renderizableTree.end(), _renderizables[_i].begin(), _renderizables[_i].end());
            }

            isRenderizableTreeDirty = false;
        }

        registry.view<DynamicSpriteRenderer, Active>().each([](const NodeID _nodeID, DynamicSpriteRenderer& _dynamicSpriteRenderer, Active& _) {
            if(_dynamicSpriteRenderer.isEnabled()) {
                _dynamicSpriteRenderer.texture->uploadToGPU();
            }
        });
    }

    void Graph::onDebugRender() {
        auto& _renderManager = scene->engine->manager.renderManager;
        _renderManager.beginDebugDraw(scene->mainCamera, getComponent<Transform>(scene->mainCamera->node->getID()));
        scene->engine->manager.physics.debugRender(&_renderManager);
        _renderManager.endDebugDraw();
    }

    Node* Graph::createNode(const std::string& _tag, Node* _parent) {
        auto _newNode = registry.create();

        auto _parentRef = _parent == nullptr ? sceneRoot->getID() : _parent->getID();

        Node* _node = nullptr;

        if(!isUI) {
            registry.emplace<Tag>(_newNode, _newNode, _tag);
            registry.emplace<Transform>(_newNode, this).parent = _parentRef;
            (&registry.get<Transform>(_parentRef))->children.push_back(&registry.get<Transform>(_newNode));
            (&registry.get<Transform>(_newNode))->parentTransform = (&registry.get<Transform>(_parentRef));

            _node = &registry.emplace<Node>(_newNode, _newNode, this, &scene->engine->manager, &registry.get<Transform>(_newNode));
            registry.get<Transform>(_newNode).node = _node;
            registry.emplace<Active>(_newNode, _node, &scene->engine->manager, this);
        } else {
            registry.emplace<Tag>(_newNode, _newNode, _tag);
            registry.emplace<UITransform>(_newNode, this).parent = _parentRef;
            (&registry.get<UITransform>(_parentRef))->children.push_back(&registry.get<UITransform>(_newNode));
            (&registry.get<UITransform>(_newNode))->parentTransform = (&registry.get<UITransform>(_parentRef));

            _node = &registry.emplace<Node>(_newNode, _newNode, this, &scene->engine->manager, (Transform*)&registry.get<UITransform>(_newNode));
            registry.get<UITransform>(_newNode).node = _node;
            registry.emplace<Active>(_newNode, _node, &scene->engine->manager, this);
        }

        if(onDataChanged != nullptr) onDataChanged((void*)_newNode);

        isRenderizableTreeDirty |= true;

        return _node;
    }

    Node* Graph::instantiatePrefab(Node* _prefab, const Vec2F& _position, Node* _parent) {
        auto _copy = registry.create();
        Util::Log::warn("InstantiatePrefab Needs implementation!!!");
//        for(auto&& _curr: registry.storage()) {
//            auto& _storage = _curr.second;
//            if(_storage.contains(_prefab)) {
//                _storage.emplace(_copy, _storage.get(_prefab));
//            }
//        }
//
//        auto* _transform = getComponent<Transform>(_copy);
//        (&registry.get<Transform>(getComponent<Transform>(_prefab)->parent))->children.push_back(_transform);
//        _transform->setPosition(_position);
//        _transform->parentTransform = (&registry.get<Transform>(getComponent<Transform>(_prefab)->parent));
//        if(_parent != NODE_ID_NULL) setParent(_copy, _parent);
//        setNodeActive(_copy, true);
//
//        if(onDataChanged != nullptr) onDataChanged((void*)_copy);

        return nullptr;
    }

    Node* Graph::getNode(const std::string& _tagName) {
        for(auto [_entity, _tag] : registry.view<Tag>().each())
            if(strcmp(_tag.tag.c_str(), _tagName.c_str()) == 0) return &registry.get<Node>(_entity);

        throw std::runtime_error("Tried to get node with tag '" + _tagName + "' but it was not found!");
    }

    void Graph::removeNode(Node* _node) {
        remove(_node, true);
        if(onDataChanged != nullptr) onDataChanged((void*)_node);
        isRenderizableTreeDirty |= true;
    }

    void Graph::removeNode(const std::string& _nodeTagName) {
        removeNode(getNode(_nodeTagName));
    }

    void Graph::printScene(Node* _node, std::ostream& _os, int& _indent) {
        if(_node->getID() == NODE_ID_NULL) return;

        auto _tag = registry.get<Tag>(_node->getID());
        _os << "* " << _tag.tag;

        auto _transform = _node->getTransform();

        for(auto _child : _transform->children) {
            _indent++;
            _os << "\n";
            for(int _in = 0; _in < _indent; _in++) _os << "\t";
            _os << "|__ ";
            printScene(_child->node, _os, _indent);
            _indent--;
        }
    }

    std::string Graph::toString() {
        std::stringstream _ss;
        int _indent = -1;
        printScene(sceneRoot, _ss, _indent);
        return _ss.str();
    }

    void Graph::setParent(Node* _node, Node* _parent) {
        auto* _nodeTransform = _node->getTransform();

        if(_nodeTransform->parent == _parent->getID()) {
            Util::Log::warn("Parent of ", (int)_node->getID(), " is already ", (int)_parent->getID(), "!!");
            return;
        }

        if(std::find(_nodeTransform->begin(), _nodeTransform->end(), _parent->getTransform()) != _nodeTransform->end()) {
            Util::Log::error("Cannot set ", (int)_parent->getID(), " as the parent of ", (int)_node->getID(), " because ", (int)_node->getID(), " is the father of ", (int)_parent->getID(), "!!!!");
            return;
        }

        remove(_node, false);

        auto* _parentTransform = _parent->getTransform();

        _nodeTransform->parent = _parent->getID();
        _nodeTransform->parentTransform = _parentTransform;
        _parentTransform->children.push_back(_nodeTransform);
        _nodeTransform->setLocalMatrix(glm::inverse(_parentTransform->getLocalMatrix()) * _nodeTransform->getLocalMatrix());

        if(onDataChanged != nullptr) onDataChanged((void*)_node);
        isRenderizableTreeDirty |= true;
    }

    void Graph::remove(Node* _node, bool _delete) {
        auto* _nodeTransform = _node->getTransform();

        if(_nodeTransform->parent != NODE_ID_NULL) {
            auto* _parentTransform = &registry.get<Transform>(_nodeTransform->parent);
            auto _toRemove = std::remove_if(_parentTransform->children.begin(), _parentTransform->children.end(), [&](Transform* _transform) {
                return _transform->node->getID() == _node->getID();
            });
            _parentTransform->children.erase(_toRemove);
        }

        if(_delete) {
            for(auto _it = _nodeTransform->children.rbegin(); _it < _nodeTransform->children.rend(); _it++) {
                remove((*_it)->node, _delete);
            }

            if(hasComponent<PhysicsBody>(_node->getID())) {
                scene->engine->manager.physics.remove(getComponent<PhysicsBody>(_node->getID()));
            }

            registry.destroy(_node->getID());
        }

    }

    void Graph::orphan(Node* _node) {
        remove(_node, false);
        _node->getTransform()->parent = sceneRoot->getID();
        _node->getTransform()->parentTransform = sceneRoot->getTransform();
        sceneRoot->getTransform()->children.push_back(_node->getTransform());
        if(onDataChanged != nullptr) onDataChanged((void*)_node);
        isRenderizableTreeDirty |= true;
    }

    void Graph::orphan(const std::string& _nodeTagName) {
        orphan(getNode(_nodeTagName));
    }

    Node* Graph::getRoot() {
        return sceneRoot;
    }

    void Graph::setNodeActive(Node* _node, bool _active) {
        if(_active && !registry.any_of<Active>(_node->getID())) {
            registry.emplace<Active>(_node->getID(), _node, &scene->engine->manager, this);
            if(onDataChanged != nullptr) onDataChanged((void*)_node);
            isRenderizableTreeDirty |= true;
            return;
        }

        if(!_active && registry.any_of<Active>(_node->getID())) {
            registry.remove<Active>(_node->getID());
            if(onDataChanged != nullptr) onDataChanged((void*)_node);
            isRenderizableTreeDirty |= true;
        }
    }

    bool Graph::isNodeActive(Node* _node) {
        return registry.any_of<Active>(_node->getID());
    }

    NodeContainer& Graph::getNodeContainer() {
        return registry;
    }

    void Graph::recalculateRenderizableTree(Node* _node, std::vector<IRenderizable*>* _renderizables) {
        auto _id = _node->getID();

        if(!hasComponent<DisabledForRender>(_id)) {

            if(hasComponent<SpriteRenderer>(_id)) {
                _renderizables[0].push_back(getComponent<SpriteRenderer>(_id));
            }

            if(hasComponent<DynamicSpriteRenderer>(_id)) {
                _renderizables[0].push_back(getComponent<DynamicSpriteRenderer>(_id));
            }

            if(hasComponent<ParticleSystem>(_id)) {
                _renderizables[1].push_back(getComponent<ParticleSystem>(_id));
            }

            if(hasComponent<TextRenderer>(_id)) {
                _renderizables[2].push_back(getComponent<TextRenderer>(_id));
            }
        }

        for(auto* _child : _node->getTransform()->children) {
            recalculateRenderizableTree(_child->node, _renderizables);
        }
    }

    void Graph::postRenderSync() {
        for(auto* _dirtyTransform : dirtyTransforms) {
            _dirtyTransform->clearDirty();
        }
        dirtyTransforms.clear();
    }
}