//
// Created by borja on 8/20/22.
//

#ifndef RDE_MAC_WINDOW_H
#define RDE_MAC_WINDOW_H

#include "core/Core.h"
#if IS_MAC()

#include "core/Engine.h"

namespace RDE {
    class MacWindow : public Window {
        public:
            explicit MacWindow(RDEConfig* _config);
    };

}

#endif

#endif //RDE_MAC_WINDOW_H
