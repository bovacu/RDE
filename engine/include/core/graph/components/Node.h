//
// Created by borja on 10/25/22.
//

#ifndef RDE_NODE_H
#define RDE_NODE_H

#include "core/graph/Graph.h"
#include "core/graph/components/Components.h"

class Manager;

namespace RDE {

    struct Node {
        friend class ImGuiScene; 
        friend class UISlider;

        private:
            Graph* graph = nullptr;
            Manager* manager = nullptr;
            NodeID ID = NODE_ID_NULL;
            Transform* transform = nullptr;

        public:
			RDE_FUNC Node(const NodeID& _nodeID, Graph* _graph, Manager* _manager, Transform* _transform) : ID(_nodeID), graph(_graph), transform(_transform), manager(_manager) {  }

            template<typename Component, typename... Args>
			RDE_FUNC Component* addComponent(Args... _args) {
                ENGINE_ASSERT(!hasComponent<Component>(), "A Node cannot have 2 components of the same type!!")
                
                #if IS_WINDOWS()
                auto* _component = &graph->registry.emplace<Component>(ID, this, manager, graph, _args...);
                #else
                auto* _component = &graph->registry.template emplace<Component>(ID, this, manager, graph, _args...);
                #endif
                if(graph->onDataChanged != nullptr) graph->onDataChanged((void*)_component);
                graph->renderingTreeData.isRenderizableTreeDirty |= std::is_same<Component, DisabledForRender>::value;
                return _component;
            }

            template<typename Component>
			RDE_FUNC_ND Component* getComponent() {
                #if IS_WINDOWS()
                return &graph->registry.get<Component>(ID);
                #else
                return &graph->registry.template get<Component>(ID);
                #endif
            }

            template<typename Component>
			RDE_FUNC void removeComponent() {
                #if IS_WINDOWS()
                auto _removed = graph->registry.remove<Component>(ID);
                #else
                auto _removed = graph->registry.template remove<Component>(ID);
                #endif
                if(graph->onDataChanged != nullptr) graph->onDataChanged((void*)_removed);
                graph->renderingTreeData.isRenderizableTreeDirty |= std::is_same<Component, DisabledForRender>::value;
            }

            template<typename Component>
			RDE_FUNC_ND bool hasComponent() const {
                #if IS_WINDOWS()
                return graph->registry.any_of<Component>(ID);
                #else
                return graph->registry.template any_of<Component>(ID);
                #endif
            }

			RDE_FUNC_ND Transform* getTransform() const {
                return transform;
            }

			RDE_FUNC_ND NodeID getID() {
                return ID;
            }

			RDE_FUNC void setParent(Node* _parent) {
                graph->setParent(this, _parent);
            }

			RDE_FUNC_ND bool isActive() {
                return graph->isNodeActive(this);
            }

			RDE_FUNC void setActive(bool _active) {
                return graph->setNodeActive(this, _active);
            }
    };

}

#endif //RDE_NODE_H
