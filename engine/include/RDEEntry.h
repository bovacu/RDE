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

struct Mystruct {
    int a;
    int b;
};

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
    delete _e;

    return EXIT_SUCCESS;
}

#endif //RDE_ENTRY_H
