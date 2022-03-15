// Created by borja on 9/3/22.

#include "core/scene/Scene.h"
#include "engine/include/core/scene/Components.h"
#include "core/render/Renderer.h"
#include "core/systems/animationSystem/AnimationSystem.h"
#include "core/Engine.h"

namespace engine {

    Scene::Scene(const std::string& _sceneName) {
        name = _sceneName;
        sceneRoot = registry.create();

        registry.emplace<Tag>(sceneRoot, _sceneName);
        registry.emplace<Transform>(sceneRoot, this, sceneRoot);
        registry.emplace<Active>(sceneRoot, true);
        registry.emplace<Hierarchy>(sceneRoot);
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


    struct dirty {};

    NodeID Scene::createNode(const std::string& _tag, const NodeID& _parent) {
        auto _newNode = registry.create();

        auto _parentRef = _parent == NODE_ID_NULL ? sceneRoot : _parent;
        registry.emplace<Hierarchy>(_newNode);
        insert(_newNode, _parentRef);
//
        registry.emplace<Tag>(_newNode, _tag);
        registry.emplace<Transform>(_newNode, this, _newNode).parent = &registry.get<Transform>(_parentRef);
        registry.emplace<Active>(_newNode, true);

        return _newNode;
    }

    NodeID Scene::createSpriteNode(Texture* _texture, const std::string& _tag, const NodeID& _parent) {
        auto _node = createNode(_tag, _parent);
        registry.emplace<SpriteRenderer>(_node, _texture);
        return _node;
    }

    NodeID Scene::getNode(const std::string& _tagName) {
        for(auto [_entity, _tag] : registry.view<Tag>().each())
            if(strcmp(_tag.tag.c_str(), _tagName.c_str()) == 0) return _entity;

        throw std::runtime_error("Tried to get node with tag '" + _tagName + "' but it was not found!");
    }

    void Scene::removeNode(const NodeID& _node) {
        remove(_node, true);
    }

    void Scene::removeNode(const std::string& _nodeTagName) {
        removeNode(getNode(_nodeTagName));
    }

    void Scene::printScene(const entt::entity& _id, std::ostream& _os, int& _indent) {
        if(_id == NODE_ID_NULL) return;

        auto _tag = registry.get<Tag>(_id);
        _os << "* " << _tag.tag;

        auto _hierarchy = registry.get<Hierarchy>(_id);

        if(_hierarchy.firstChild != NODE_ID_NULL) {
            _indent++;
            _os << "\n";
            for(int _in = 0; _in < _indent; _in++) _os << "\t";
            _os << "|__ ";
            printScene(_hierarchy.firstChild, _os, _indent);
            _indent--;
        }

        if(_hierarchy.nextBrother != NODE_ID_NULL) {
            _os << "\n";
            for(int _in = 0; _in < _indent; _in++) _os << "\t";
            _os << "|__ ";

            printScene(_hierarchy.nextBrother, _os, _indent);
        }
    }

    std::string Scene::toString() {
        std::stringstream _ss;
        int _indent = -1;
        printScene(sceneRoot, _ss, _indent);
        return _ss.str();
    }

    void Scene::setParent(const NodeID& _node, const NodeID& _parent) {
        remove(_node, false);
        insert(_node, _parent);
    }

    bool Scene::nodeIsLeaf(const NodeID& _nodeID) {
        auto _nodeHierarchy = registry.get<Hierarchy>(_nodeID);
        return _nodeHierarchy.nextBrother == NODE_ID_NULL && _nodeHierarchy.firstChild == NODE_ID_NULL;
    }

    void Scene::insert(const NodeID& _node, const NodeID& _parent) {
        auto* _newNodeHierarchy = &registry.get<Hierarchy>(_node);
        _newNodeHierarchy->parent = _parent;

        auto* _parentHierarchy = getComponent<Hierarchy>(_parent);
        auto _firstChild = _parentHierarchy->firstChild;
        _parentHierarchy->children++;

        if(_firstChild == NODE_ID_NULL) {
            _parentHierarchy->firstChild = _node;
        } else {
            auto* _lastNodeHierarchy = &registry.get<Hierarchy>(_parentHierarchy->lastChild);
            _newNodeHierarchy->prevBrother = _parentHierarchy->lastChild;
            _lastNodeHierarchy->nextBrother = _node;
        }

        _parentHierarchy->lastChild = _node;
    }

    void Scene::remove(const NodeID& _node, bool _delete) {
        if(_node == NODE_ID_NULL) return;
        auto* _nodeHierarchy = &registry.get<Hierarchy>(_node);

        size_t _childrenCount = _nodeHierarchy->children;
        for(size_t _child = 0; _child < _childrenCount; _child++)
            removeNode(_nodeHierarchy->firstChild);

        auto* _parentHierarchy = &registry.get<Hierarchy>(_nodeHierarchy->parent);
        auto _nextBrother = _nodeHierarchy->nextBrother;
        auto _previousBrother = _nodeHierarchy->prevBrother;

        if(_delete) registry.destroy(_node);
        if(_previousBrother != NODE_ID_NULL)        (&registry.get<Hierarchy>(_previousBrother))->nextBrother = _nextBrother;
        if(_nextBrother != NODE_ID_NULL)            (&registry.get<Hierarchy>(_nextBrother))->prevBrother = _previousBrother;
        if(_parentHierarchy->firstChild == _node)   _parentHierarchy->firstChild = _nextBrother;
        if(_parentHierarchy->lastChild == _node)    _parentHierarchy->lastChild = _previousBrother;


        _parentHierarchy->children--;
    }

    void Scene::sortHierarchyInMemory() {
        registry.sort<Hierarchy>([&](const entt::entity lhs, const entt::entity rhs) {
            const auto &clhs = registry.get<Hierarchy>(lhs);
            const auto &crhs = registry.get<Hierarchy>(rhs);
            return crhs.parent == lhs || clhs.nextBrother == rhs || (!(clhs.parent == rhs || crhs.nextBrother == lhs) && (clhs.parent < crhs.parent || (clhs.parent == crhs.parent && &clhs < &crhs)));
        });
    }
}