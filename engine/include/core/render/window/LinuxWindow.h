//
// Created by borja on 8/20/22.
//

#ifndef RDE_LINUX_WINDOW_H
#define RDE_LINUX_WINDOW_H

#include "core/Core.h"

#if IS_LINUX() && !IS_ANDROID()

#include "core/render/window/Window.h"

namespace RDE {

    class LinuxWindow : public Window {
        public:
			explicit LinuxWindow(Engine* _engine, RDEConfig* _config);
    };

}

#endif

#endif //RDE_LINUX_WINDOW_H
