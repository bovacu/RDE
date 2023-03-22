//
// Created by borja on 8/20/22.
//

#ifndef RDE_ENTRY_H
#define RDE_ENTRY_H

/**
 * @brief This is an automatic 'main' so the final user doesn't need to create one and can just run the game or apps.
 */

#include "core/Engine.h"
#include "core/util/Functions.h"
#if IS_WINDOWS()
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

#ifdef __EMSCRIPTEN__
#include "emscripten.h"

RDE::Engine* _e = nullptr;

void mainLoop() {
    _e->onRun();
}

int main(int _argc, char** _argv) {
    // This is needed because on windows SDL2 disables the console and logs if it is not in Debug.
    #if IS_WINDOWS()
        #ifdef ENGINE_DEBUG
            if(AllocConsole())
            {
                freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
                freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
                std::ios::sync_with_stdio(1);
            }
        #endif
    #endif

    _e = RDE::createEngine(_argc, _argv);
    emscripten_set_main_loop(mainLoop, -1, 1);
    _e->destroy();

    RDE::Util::Log::info("Program finished!");

    delete _e;

    return EXIT_SUCCESS;
}

#else

int main(int _argc, char** _argv) {
    // This is needed because on windows SDL2 disables the console and logs if it is not in Debug.
    #if IS_WINDOWS()
        #ifdef ENGINE_DEBUG
            if(AllocConsole())
            {
                freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
                freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
                std::ios::sync_with_stdio(1);
            }
        #endif
    #endif

    auto* _e = RDE::createEngine(_argc, _argv);
    _e->onRun();
    _e->destroy();

    return EXIT_SUCCESS;
}

#endif

#endif //RDE_ENTRY_H
