//
// Created by borja on 8/20/22.
//

#include "core/render/window/LinuxWindow.h"

#if IS_LINUX() && !IS_ANDROID()
#include "core/Engine.h"

namespace GDE {

    LinuxWindow::LinuxWindow(GDEConfig* _config) : Window(_config) {

        LOG_DEBUG("Platform: Linux")
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        window = SDL_CreateWindow(_config->windowData.title.c_str(), 0, 0,
                                  (int)_config->windowData.size.x, (int)_config->windowData.size.y,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);

        if(window == nullptr) {
            LOG_E("SDL window creation failed: ", SDL_GetError());
            return;
        }
        context = SDL_GL_CreateContext(window);

        if(context == nullptr) {
            LOG_E("OpenGL context couldn't initialize -> ", SDL_GetError())
            return;
        }

        SDL_GL_MakeCurrent(window, context);

        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            LOG_E("Failed to initialize GLAD")
            return;
        }
        LOG_DEBUG("GLAD and SDL2 loaded successfully");

        SDL_GL_SetSwapInterval(1);

        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_SetWindowResizable(window, SDL_TRUE);

        if(!properties->projectData.iconPath.empty()) setIcon(properties->projectData.iconPath);
    }
}

#endif