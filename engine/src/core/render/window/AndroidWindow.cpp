//
// Created by borja on 8/20/22.
//

#include "core/render/window/AndroidWindow.h"

#if IS_ANDROID()

#include "core/Engine.h"

namespace RDE {

	AndroidWindow::AndroidWindow(Engine* _engine, RDEConfig* _config) : Window(_engine, _config) {
            Util::Log::debug("Platform: Android");
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
            SDL_DisplayMode mode;
            SDL_GetDisplayMode(0,0,&mode);
            int width = mode.w;
            int height = mode.h;
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
            window = SDL_CreateWindow(nullptr,0,0,width,height,SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
            SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait");

            int drawableSizeX = 0, drawableSizeY = 0;
            SDL_GL_GetDrawableSize(window, &drawableSizeX, &drawableSizeY);
            Util::Log::debug("Screen Size: (", width, ", ", height, ")", "Drawable Size: (", drawableSizeX, ", ", drawableSizeY, ")");

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

            SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE, "1");
            SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE_PAUSEAUDIO, "1");
            SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1");

            SDL_GL_SetSwapInterval(1);

            refreshDpi();
    }
}

#endif
