// Created by borja on 9/3/22.

#include "core/graph/Graph.h"
#include "core/graph/Components.h"
#include "core/render/Renderer.h"
#include "core/systems/animationSystem/AnimationSystem.h"

namespace GDE {

    static void onEventDelFoo(Event& _event) {  }
    static void onUpdateDelFoo(Delta _dt) {  }
    static void onFixedUpdateDelFoo(Delta _dt) {  }
    static void onRenderDelFoo() {  }

    Graph::Graph(const std::string& _sceneName) {
        name = _sceneName;
        sceneRoot = registry.create();

        registry.emplace<Tag>(sceneRoot, _sceneName);
        registry.emplace<Transform>(sceneRoot).parent = NODE_ID_NULL;
        registry.emplace<Active>(sceneRoot, true);

        onEventDel.bind<&onEventDelFoo>();
        onUpdateDel.bind<&onUpdateDelFoo>();
        onFixedUpdateDel.bind<&onFixedUpdateDelFoo>();
        onRenderDel.bind<&onRenderDelFoo>();
    }


    void Graph::onEvent(Event& _event) {
        onEventDel(_event);
    }

    void Graph::onUpdate(Delta _dt) {
        registry.view<Transform, Active>(entt::exclude<StaticTransform>).each([&](const auto _entity, Transform& _transform, const Active& _active) {
            _transform.update(this);
        });

        registry.group<AnimationSystem>(entt::get<SpriteRenderer, Active>).each([&](const auto _entity, AnimationSystem& _animationSystem, SpriteRenderer& _spriteRenderer, const Active& _active) {
            _animationSystem.update(_dt, _spriteRenderer);
        });

        onUpdateDel(_dt);
    }

    void Graph::onFixedUpdate(Delta _dt) {

        registry.view<Transform, Body, Active>(entt::exclude<StaticTransform>).each([&](const auto _entity, Transform& _transform, const Body& _body, const Active& _active) {
            _transform.setPosition(_body.getPosition());
            _transform.setRotation(_body.getRotation());
        });

        onFixedUpdateDel(_dt);
    }

    void Graph::onRender() {
        registry.group<Transform>(entt::get<SpriteRenderer, Active>).each([&](const auto _entity, const Transform& _transform, const SpriteRenderer& _spriteRenderer, const Active& _active) {
            Renderer::draw(_spriteRenderer, _transform);
        });

        auto _a = registry.view<int>();

        registry.view<Transform, TextRenderer>().each([&](const auto _entity, const Transform& _transform, const TextRenderer& _text) {
            Renderer::draw(_text, _transform);
        });

        registry.view<Body>().each([&](const auto _entity, const Body& _body) {
            Renderer::drawSquare(_body.getPosition(), _body.getConfig().size, {Color::Green.r, Color::Green.g, Color::Green.b, 100}, _body.getRotation());
        });

        onRenderDel();
    }

    NodeID Graph::createNode(const std::string& _tag, const NodeID& _parent) {
        auto _newNode = registry.create();

        auto _parentRef = _parent == NODE_ID_NULL ? sceneRoot : _parent;

        registry.emplace<Tag>(_newNode, _tag);
        registry.emplace<Transform>(_newNode).parent = _parentRef;
        (&registry.get<Transform>(_parentRef))->children.push_back(_newNode);

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
            registry.emplace<StaticTransform>(_node);
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
            registry.emplace<Active>(_node);
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