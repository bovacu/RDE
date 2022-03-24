// Created by borja on 9/3/22.

#include "core/graph/Graph.h"
#include "engine/include/core/graph/Components.h"
#include "core/render/Renderer.h"
#include "core/systems/animationSystem/AnimationSystem.h"

namespace engine {

    Graph::Graph(const std::string& _sceneName) {
        name = _sceneName;
        sceneRoot = registry.create();

        registry.emplace<Tag>(sceneRoot, _sceneName);
        registry.emplace<Transform>(sceneRoot).parent = NODE_ID_NULL;
        registry.emplace<Active>(sceneRoot, true);
        registry.emplace<Hierarchy>(sceneRoot);
    }


    void Graph::onEvent(Event& _event) {

    }

    void Graph::onUpdate(Delta _dt) {
        registry.group<Transform>(entt::get<Active>).each([&](const auto _entity, Transform& _transform, const Active& _active) {
            LOG_I("Dirty ", _transform.isDirty(), " for ", (int)_entity)
            if(!_transform.isDirty() || !_active.active) return;
            _transform.update(this);
        });

        registry.group<SpriteRenderer>(entt::get<AnimationSystem, Active>).each([&](const auto _entity, SpriteRenderer& _spriteRenderer, AnimationSystem& _animationSystem, const Active& _active) {
            if(!_active.active) return;
            _animationSystem.update(_dt, _spriteRenderer);
        });
    }

    void Graph::onFixedUpdate(Delta _dt) {

    }

    void Graph::onRender() {
        registry.group<Transform>(entt::get<SpriteRenderer, Active>).each([&](const auto _entity, Transform& _transform, SpriteRenderer& _spriteRenderer, const Active& _active) {
            if(!_active.active) return;
            Renderer::draw(_spriteRenderer, _transform);
        });

        registry.view<Transform, Text>().each([&](const auto _entity, Transform& _transform, Text& _text) {
            Renderer::draw(_text, _transform);
        });
    }

    NodeID Graph::createNode(const std::string& _tag, const NodeID& _parent) {
        auto _newNode = registry.create();

        auto _parentRef = _parent == NODE_ID_NULL ? sceneRoot : _parent;
        registry.emplace<Hierarchy>(_newNode);
        insert(_newNode, _parentRef);

        registry.emplace<Tag>(_newNode, _tag);
        registry.emplace<Transform>(_newNode).parent = _parentRef;
        registry.get<Transform>(_parentRef).children.push_back(_newNode);

        registry.emplace<Active>(_newNode, true);

        return _newNode;
    }

    NodeID Graph::createSpriteNode(Texture* _texture, const std::string& _tag, const NodeID& _parent) {
        auto _node = createNode(_tag, _parent);
        registry.emplace<SpriteRenderer>(_node, _texture);
        return _node;
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

    std::string Graph::toString() {
        std::stringstream _ss;
        int _indent = -1;
        printScene(sceneRoot, _ss, _indent);
        return _ss.str();
    }

    void Graph::setParent(const NodeID& _node, const NodeID& _parent) {
        remove(_node, false);
        insert(_node, _parent);
    }

    bool Graph::nodeIsLeaf(const NodeID& _nodeID) {
        auto _nodeHierarchy = registry.get<Hierarchy>(_nodeID);
        return _nodeHierarchy.nextBrother == NODE_ID_NULL && _nodeHierarchy.firstChild == NODE_ID_NULL;
    }

    void Graph::insert(const NodeID& _node, const NodeID& _parent) {
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

    void Graph::remove(const NodeID& _node, bool _delete) {
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

    void Graph::sortHierarchyInMemory() {
        registry.sort<Hierarchy>([&](const entt::entity lhs, const entt::entity rhs) {
            const auto &clhs = registry.get<Hierarchy>(lhs);
            const auto &crhs = registry.get<Hierarchy>(rhs);
            return crhs.parent == lhs || clhs.nextBrother == rhs || (!(clhs.parent == rhs || crhs.nextBrother == lhs) && (clhs.parent < crhs.parent || (clhs.parent == crhs.parent && &clhs < &crhs)));
        });
    }
}