//
// Created by borja on 8/20/22.
//

#include "core/render/window/IOSWindow.h"

#if IS_IOS()

#include "core/Engine.h"

namespace RDE {

    IOSWindow::IOSWindow(RDEConfig* _config) : Window(_config) {
        Util::Log::debug("Platform: IOS");
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        SDL_DisplayMode displayMode;
        SDL_GetDesktopDisplayMode(0, &displayMode);
        window = SDL_CreateWindow(nullptr, 0, 0, displayMode.w, displayMode.h,  SDL_WINDOW_OPENGL);

        if(window == nullptr) {
            Util::Log::error("SDL window creation failed: ", SDL_GetError());
            return;
        }
        context = SDL_GL_CreateContext(window);

        if(context == nullptr) {
            Util::Log::error("OpenGL context couldn't initialize -> ", SDL_GetError());
            return;
        }

        SDL_GL_MakeCurrent(window, context);

        SDL_GL_SetSwapInterval(1);

        refreshDpi();
    }
}

#endif