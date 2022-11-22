//
// Created by borja on 11/21/22.
//

#ifndef RDE_UI_MASK_H
#define RDE_UI_MASK_H

#include "core/Core.h"

namespace RDE {

    FORWARD_DECLARE_CLASS(Node, Manager, Graph)

    class UIMask {
        public:
            bool inverted = false;

        public:
            UIMask(Node* _node, Manager* _manager, Graph* _graph) {  }
            ~UIMask() {  }
    };

}

#endif //RDE_UI_MASK_H
