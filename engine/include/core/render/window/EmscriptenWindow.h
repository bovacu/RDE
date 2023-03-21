//
// Created by borja on 03/13/23.
//

#ifndef RDE_EMSCRIPTEN_WINDOW_H
#define RDE_EMSCRIPTEN_WINDOW_H

#include "core/Core.h"
#ifdef __EMSCRIPTEN__

#include "Window.h"

namespace RDE {
    class EmscriptenWindow : public Window {
        public:
			explicit EmscriptenWindow(Engine* _engine, RDEConfig* _config);
    };

}

#endif

#endif //RDE_EMSCRIPTEN_WINDOW_H