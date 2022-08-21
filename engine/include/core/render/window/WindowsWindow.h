//
// Created by borja on 8/20/22.
//

#ifndef GDE_WINDOWS_WINDOW_H
#define GDE_WINDOWS_WINDOW_H

#include "core/Core.h"

#if IS_WINDOWS() && !IS_ANDROID()
#include "core/render/window/Window.h"

namespace GDE {

    class WindowsWindow : public Window {
        public:
            explicit WindowsWindow(GDEConfig* _config);
    };

}
#endif

#endif //GDE_WINDOWS_WINDOW_H