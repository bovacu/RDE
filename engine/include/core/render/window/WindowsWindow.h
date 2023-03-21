//
// Created by borja on 8/20/22.
//

#ifndef RDE_WINDOWS_WINDOW_H
#define RDE_WINDOWS_WINDOW_H

#include "core/Core.h"

#if IS_WINDOWS() && !IS_ANDROID()
#include "core/render/window/Window.h"

namespace RDE {

    class WindowsWindow : public Window {
        public:
			RDE_FUNC_EXPLICIT WindowsWindow(Engine* _engine, RDEConfig* _config);
    };

}
#endif

#endif //RDE_WINDOWS_WINDOW_H