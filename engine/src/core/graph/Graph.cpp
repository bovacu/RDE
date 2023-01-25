// Created by borja on 9/3/22.

#include "core/graph/Graph.h"
#include "core/graph/components/Components.h"
#include "core/graph/components/Transform.h"
#include "core/graph/components/SpriteRenderer.h"
#include "core/graph/components/TextRenderer.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/elements/Texture.h"
#include "core/systems/animationSystem/AnimationSystem.h"
#include "core/graph/Scene.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/DynamicSpriteRenderer.h"
#include "core/graph/components/ui/UI.h"
#include "core/graph/components/Node.h"

namespace RDE {
    
    /// All of these are foo functions as Delegates need to have something assigned to be called.
    static void onEventDelFoo(NodeContainer&, Event& _event) {  }
    static void onUpdateDelFoo(NodeContainer&, Delta) {  }
    static void onFixedUpdateDelFoo(NodeContainer&, Delta) {  }
    static void onRenderDelFoo(NodeContainer&) {  }

    Graph::Graph(Scene* _scene, const std::string& _sceneName, bool _isUI) {

        renderingTreeData.isUI = _isUI;

        scene = _scene;
        name = _sceneName;
        auto _sceneRootID = registry.create();

        if(!_isUI) {
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
            _interactable->interactable = false;
            _interactable->focused = false;
        }
    }

    Node* Graph::createNode(const std::string& _tag, Node* _parent) {
        auto _newNode = registry.create();

        auto _parentRef = _parent == nullptr ? sceneRoot->getID() : _parent->getID();

        Node* _node = nullptr;

        if(!renderingTreeData.isUI) {
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

        renderingTreeData.isRenderizableTreeDirty = true;

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

    Node* Graph::getNode(const NodeID& _nodeID) {
        return &registry.get<Node>(_nodeID);
    }

    void Graph::removeNode(Node* _node) {
        removeNodeInner(_node, true);
        if(onDataChanged != nullptr) onDataChanged((void*)_node);
        renderingTreeData.isRenderizableTreeDirty = true;
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

        removeNodeInner(_node, false);

        auto* _parentTransform = _parent->getTransform();

        _nodeTransform->parent = _parent->getID();
        _nodeTransform->parentTransform = _parentTransform;
        _parentTransform->children.push_back(_nodeTransform);
        _nodeTransform->setLocalMatrix(glm::inverse(_parentTransform->getLocalMatrix()) * _nodeTransform->getLocalMatrix());

        if(onDataChanged != nullptr) onDataChanged((void*)_node);
        renderingTreeData.isRenderizableTreeDirty = true;
    }

    void Graph::removeNodeInner(Node* _node, bool _delete) {
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
                removeNodeInner((*_it)->node, _delete);
            }

            if(_node->hasComponent<PhysicsBody>()) {
                scene->engine->manager.physics.remove(_node->getComponent<PhysicsBody>());
            }

            registry.destroy(_node->getID());
        }

    }

    void Graph::orphan(Node* _node) {
        removeNodeInner(_node, false);
        _node->getTransform()->parent = sceneRoot->getID();
        _node->getTransform()->parentTransform = sceneRoot->getTransform();
        sceneRoot->getTransform()->children.push_back(_node->getTransform());
        if(onDataChanged != nullptr) onDataChanged((void*)_node);
        renderingTreeData.isRenderizableTreeDirty = true;
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
            renderingTreeData.isRenderizableTreeDirty = true;
            return;
        }

        if(!_active && registry.any_of<Active>(_node->getID())) {
            registry.remove<Active>(_node->getID());
            if(onDataChanged != nullptr) onDataChanged((void*)_node);
            renderingTreeData.isRenderizableTreeDirty = true;
        }
    }

    bool Graph::isNodeActive(Node* _node) {
        return registry.any_of<Active>(_node->getID());
    }

    NodeContainer& Graph::getNodeContainer() {
        return registry;
    }
}