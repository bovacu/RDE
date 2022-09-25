//
// Created by borja on 8/20/22.
//

#include "core/render/window/IOSWindow.h"

#if IS_IOS()

#include "core/Engine.h"

namespace GDE {

    IOSWindow::IOSWindow(GDEConfig* _config) : Window(_config) {
        LOG_DEBUG("Platform: IOS")
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        SDL_DisplayMode displayMode;
        SDL_GetDesktopDisplayMode(0, &displayMode);
        window = SDL_CreateWindow(nullptr, 0, 0, displayMode.w, displayMode.h,  SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

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

        SDL_GL_SetSwapInterval(1);
    }
}

#endif