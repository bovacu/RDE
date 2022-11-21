//
// Created by borja on 8/20/22.
//

#ifndef RDE_IOS_WINDOW_H
#define RDE_IOS_WINDOW_H

#include "core/Core.h"

#if IS_IOS()

#include "Window.h"

namespace RDE {

    class IOSWindow : public Window {
        public:
            explicit IOSWindow(RDEConfig* _config);
    };
}

#endif

#endif //RDE_IOS_WINDOW_H