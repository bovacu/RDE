//
// Created by borja on 8/20/22.
//

#ifndef RDE_ENTRY_H
#define RDE_ENTRY_H

/**
 * @brief This is an automatic 'main' so the final user doesn't need to create one and can just run the game or apps.
 */

#include "core/Engine.h"

int main(int _argc, char** _argv) {
    auto* _e = RDE::createEngine(_argc, _argv);
    _e->onRun();
    _e->destroy();
    delete _e;
    return EXIT_SUCCESS;
}

#endif //RDE_ENTRY_H
