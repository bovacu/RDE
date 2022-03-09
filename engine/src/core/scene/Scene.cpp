// Created by borja on 9/3/22.

#include "core/scene/Scene.h"
#include "engine/include/core/scene/Components.h"
#include "core/render/Renderer.h"
#include "core/systems/animationSystem/AnimationSystem.h"

namespace engine {

    Scene::Scene(const std::string& _sceneName) {
        name = _sceneName;
        sceneRoot = registry.create();

        registry.emplace<Tag>(sceneRoot, _sceneName);
        registry.emplace<Transform>(sceneRoot);
        registry.emplace<Active>(sceneRoot, true);
        registry.emplace<Node>(sceneRoot, this, nullptr, sceneRoot);
    }


    void Scene::onEvent(Event& _event) {

    }

    void Scene::onUpdate(Delta _dt) {
        registry.view<SpriteRenderer, AnimationSystem>().each([&](const auto _entity, SpriteRenderer& _spriteRenderer, AnimationSystem& _animationSystem) {
            _animationSystem.update(_dt, _spriteRenderer);
        });
    }

    void Scene::onFixedUpdate(Delta _dt) {

    }

    void Scene::onRender() {
        registry.view<Transform, SpriteRenderer>().each([&](const auto _entity, Transform& _transform, SpriteRenderer& _spriteRenderer) {
             Renderer::draw(_spriteRenderer, _transform);
        });

        registry.view<Transform, Text>().each([&](const auto _entity, Transform& _transform, Text& _text) {
            Renderer::draw(_text, _transform);
        });
    }


    Node* Scene::createNode(const std::string& _tag, Node* _parent) {
        auto _n = registry.create();

        registry.emplace<Tag>(_n, _tag);
        registry.emplace<Transform>(_n);
        registry.emplace<Active>(_n, true);

        return &registry.emplace<Node>(_n, this, _parent == nullptr ? &registry.get<Node>(sceneRoot) : _parent, _n);
    }

    Node* Scene::createSpriteNode(const std::string& _tag, Node* _parent) {
        auto* _node = createNode(_tag, _parent);
        addComponent<SpriteRenderer>(_node->id);
        return _node;
    }

    Node* Scene::getNode(const std::string& _tagName) {
        for(auto [_entity, _tag, _node] : registry.view<Tag, Node>().each()) {
            if(strcmp(_tag.tag.c_str(), _tagName.c_str()) == 0) return &_node;
        }

        return nullptr;
    }

    Node* Scene::getNode(const NodeID& _nodeID) {
        return &registry.get<Node>(_nodeID);
    }

    void Scene::removeNode(const Node& _node) {
//        registry.view<Parent>().each([&](Parent& _parent) {
//            if(_parent.node->id == _node.id) {
//                removeNode(_node);
//                delete *nodeTracker.erase(_node.iter).base();
//            }
//        });
    }

    void Scene::removeNode(const std::string& _nodeTagName) {
//        auto _nodeToRemove = *std::find(nodeTracker.begin(), nodeTracker.end(),
//           [&](Node* _n) {
//               return std::equal(registry.get<Tag>(_n->id).tag.begin(), registry.get<Tag>(_n->id).tag.end(), _nodeTagName.c_str());
//            }).base();
//
//        removeNode(*_nodeToRemove);
    }

    void Scene::printScene(const entt::entity& _id, std::ostream& _os, int& _indent) {
        auto _tag = registry.get<Tag>(_id);
        _os << "* " << _tag.tag;
        auto _view = registry.view<Node>();

        for(auto _i = _view.rbegin(); _i != _view.rend(); _i++) {
            auto _node = _view.get<Node>((*_i));
            if(_node.parent != nullptr && _node.id == _id) {
                _os << "\n";
                for(int _in = 0; _in < _indent; _in++) _os << "\t";
                _os << "|__ ";
                _indent++;
                printScene((*_i), _os, _indent);
                _indent--;
            }
        }
    }

    std::string Scene::toString() {
        std::stringstream _ss;
        int _indent = 0;
        printScene(sceneRoot, _ss, _indent);
        return _ss.str();
    }

    void Scene::setParent(const Node& _node, Node& _parent) {
        registry.get<Node>(_node.id).parent = &_parent;
    }
}