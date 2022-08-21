//
// Created by borja on 8/20/22.
//

#ifndef GDE_MAC_WINDOW_H
#define GDE_MAC_WINDOW_H

#include "core/Core.h"
#if IS_MAC()

#include "core/Engine.h"

namespace GDE {
    class MacWindow : public Window {
        public:
            explicit MacWindow(GDEConfig* _config);
    };

}

#endif

#endif //GDE_MAC_WINDOW_H
