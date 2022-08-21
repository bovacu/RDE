//
// Created by borja on 8/20/22.
//

#ifndef GDE_IOS_WINDOW_H
#define GDE_IOS_WINDOW_H

#include "core/Core.h"

#if IS_IOS()

#include "Window.h"

namespace GDE {

    class IOSWindow : public Window {
        public:
            explicit IOSWindow(GDEConfig* _config);
    };
}

#endif

#endif //GDE_IOS_WINDOW_H