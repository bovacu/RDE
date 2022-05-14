//
// Created by borja on 09/05/22.
//

#ifndef ENGINE_CANVAS_H
#define ENGINE_CANVAS_H

#include "core/systems/uiSystem/RmlOpenGLRenderer.h"
#include "core/systems/uiSystem/SystemInterfaceSDL2.h"
#include <RmlUi/Core.h>

namespace GDE {

    struct RmlData {
        RmlOpenGLRenderer* rmlRenderer = nullptr;
        RmlUiSDL2SystemInterface* rmlSystemInterface = nullptr;
        Rml::Context* rmlContext = nullptr;
        Rml::ElementDocument* rmlDocument = nullptr;
    };

    class Canvas {
        private:
            RmlData rmlData;
            bool enabled = true;

        private:
            static Canvas& get();

        public:
            static void init(int _width, int _height);
            static void enable(bool _enable);
            static void beginFrame(const Vec2I _viewport);
            static void update(Delta _dt);
            static void render();
            static void destroy();
            static RmlData& getData();
    };

}

#endif //ENGINE_CANVAS_H
