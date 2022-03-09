// Created by borja on 9/3/22.

#include "core/scene/Scene.h"
#include "engine/include/core/scene/Components.h"
#include "core/scene/Nodes.h"

namespace engine {

    Scene::Scene(const std::string& _sceneName) {
        name = _sceneName;

        sceneRoot = new Node;
        sceneRoot->id = registry.create();

        registry.emplace<Tag>(sceneRoot->id, _sceneName);
        registry.emplace<Transform>(sceneRoot->id);
        registry.emplace<Active>(sceneRoot->id, true);
        registry.emplace<Parent>(sceneRoot->id, nullptr);

        sceneRoot->scene = this;
    }

    Node& Scene::createNode(const std::string& _tag, Node* _parent) {
        Node* _n = new Node;
        _n->id = registry.create();
        _n->scene = this;

        registry.emplace<Tag>(_n->id, _tag);
        registry.emplace<Transform>(_n->id);
        registry.emplace<Active>(_n->id, true);
        registry.emplace<Parent>(_n->id, _parent == nullptr ? sceneRoot : _parent);

        nodeTracker.push_back(_n);
        _n->iter = nodeTracker.end();

        return *nodeTracker.back();
    }

    Node& Scene::createSpriteNode(const std::string& _tag, Node* _parent) {
        auto& _node = createNode(_tag, _parent);
        _node.addComponent<SpriteRenderer>();
        return _node;
    }

    void Scene::removeNode(const Node& _node) {
        registry.view<Parent>().each([&](Parent& _parent) {
            if(_parent.node->id == _node.id) {
                removeNode(_node);
                delete *nodeTracker.erase(_node.iter).base();
            }
        });
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
        auto _view = registry.view<Parent>();

        for(auto _i = _view.rbegin(); _i != _view.rend(); _i++) {
            auto _parent = _view.get<Parent>((*_i));
            if(_parent.node != nullptr && _parent.node->id == _id) {
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
        printScene(sceneRoot->id, _ss, _indent);
        return _ss.str();
    }

    void Scene::setParent(const Node& _node, Node& _parent) {
        registry.get<Parent>(_node.id).node = &_parent;
    }

    Scene::~Scene() {
        for(auto* _node : nodeTracker) delete _node;
        delete sceneRoot;
    }
}