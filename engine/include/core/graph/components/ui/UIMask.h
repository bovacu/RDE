//
// Created by borja on 11/21/22.
//

#ifndef RDE_UI_MASK_H
#define RDE_UI_MASK_H

#include "core/Core.h"
#include "core/graph/components/Node.h"

namespace RDE {

    class Manager;
    class Graph;

    struct Node;

    class UIMask {
        private:
            Node* node;

        public:
            bool inverted = false;

        public:
			RDE_FUNC UIMask(Node* _node, Manager* _manager, Graph* _graph) : node(_node) {  }
			RDE_FUNC ~UIMask() {  }

			RDE_FUNC_ND bool isEnabled() {
                return !node->hasComponent<DisabledForRender>();
            }

			RDE_FUNC void setEnabled(bool _enabled) {
                if(_enabled && node->hasComponent<DisabledForRender>()) {
                    node->removeComponent<DisabledForRender>();
                    return;
                }

                if(!_enabled && !node->hasComponent<DisabledForRender>()) {
                    node->addComponent<DisabledForRender>();
                }
            }
    };

}

#endif //RDE_UI_MASK_H
