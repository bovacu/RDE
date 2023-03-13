//
// Created by borja on 03/13/23.
//

#include "core/render/window/EmscriptenWindow.h"

#ifdef __EMSCRIPTEN__

#include "core/Engine.h"

namespace RDE {

    EmscriptenWindow::EmscriptenWindow(RDEConfig* _config) : Window(_config) {
        Util::Log::debug("Platform: Emscripten");
        Util::Log::debug("OpenGL Version: 3.2");
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        SDL_Renderer *renderer = NULL;
        SDL_CreateWindowAndRenderer((int)_config->windowData.size.x, (int)_config->windowData.size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN, &window, &renderer);

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
        Util::Log::debug("GLAD and SDL2 loaded successfully");

        SDL_SetWindowResizable(window, SDL_TRUE);

        currentDisplayIndex = SDL_GetWindowDisplayIndex(window);

        refreshDpi();
    }
}

#endif