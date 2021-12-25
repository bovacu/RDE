// Created by borja on 24/12/21.

#include "GraphicsContext.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace engine {

    GraphicsContext::GraphicsContext(GLFWwindow* _windowHandle) : windowHandle(_windowHandle) {  }

    engine::GraphicsContextPtr engine::GraphicsContext::create(GLFWwindow* _window) {
        return std::make_shared<GraphicsContext>(_window);
    }

    void GraphicsContext::init() {
        ENGINE_ASSERT(this->windowHandle, "Window handle is null!")

        /// First, initialization to the Window.
        glfwMakeContextCurrent(this->windowHandle);

        /// Checking if there are errors.
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        ENGINE_ASSERT(status, "Failed to initialize Glad!")

#pragma region DebugInfoAndChecks
#ifdef ENGINE_DEBUG
        LOG_I("OpenGL Info:");
        LOG_I("  Vendor: ", glGetString(GL_VENDOR));
        LOG_I("  Renderer: ", glGetString(GL_RENDERER));
        LOG_I("  Version: ", glGetString(GL_VERSION));
#endif

#ifdef ENGINE_ENABLE_ASSERTS
        int versionMajor;
        int versionMinor;
        glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
        glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

        ENGINE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Engine requires at least OpenGL version 4.5!")
#endif
#pragma endregion
    }

    void GraphicsContext::swapBuffers() {
        /// This updates the window and allows rendering.
        glfwSwapBuffers(this->windowHandle);
    }

}
