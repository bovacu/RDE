//
// Created by borja on 8/20/22.
//

#ifndef RDE_ANDROID_WINDOW_H
#define RDE_ANDROID_WINDOW_H

#include "core/Core.h"

#if IS_ANDROID()

#include "Window.h"

namespace RDE {

    class AndroidWindow : public Window {
    public:
        explicit AndroidWindow(RDEConfig* _config);
    };
}

#endif

#endif //RDE_ANDROID_WINDOW_H
