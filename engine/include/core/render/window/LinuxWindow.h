//
// Created by borja on 8/20/22.
//

#ifndef GDE_LINUX_WINDOW_H
#define GDE_LINUX_WINDOW_H

#include "core/Core.h"

#if IS_LINUX() && !IS_ANDROID()

#include "core/render/window/Window.h"

namespace GDE {

    class LinuxWindow : public Window {
        public:
            explicit LinuxWindow(GDEConfig* _config);
    };

}

#endif

#endif //GDE_LINUX_WINDOW_H
