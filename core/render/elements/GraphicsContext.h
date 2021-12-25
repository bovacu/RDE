// Created by borja on 24/12/21.


#ifndef ENGINE2_0_GRAPHICS_CONTEXT_H
#define ENGINE2_0_GRAPHICS_CONTEXT_H


#include "core/util/Util.h"

struct GLFWwindow;

namespace engine {

    class GraphicsContext;
    typedef std::shared_ptr<GraphicsContext> GraphicsContextPtr;

    class GraphicsContext {
        private:
            GLFWwindow* windowHandle = nullptr;

        public:
            explicit GraphicsContext(GLFWwindow* _window);
            void init();
            void swapBuffers();

            static GraphicsContextPtr create(GLFWwindow* _window);
    };

}


#endif //ENGINE2_0_GRAPHICS_CONTEXT_H
