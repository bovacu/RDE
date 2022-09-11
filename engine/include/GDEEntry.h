//
// Created by borja on 8/20/22.
//

#ifndef GDE_ENTRY_H
#define GDE_ENTRY_H

#include "core/Engine.h"

int main(int _argc, char** _argv) {
    auto* _e = GDE::createEngine(_argc, _argv);
    _e->onRun();
    _e->destroy();
    delete _e;
    return EXIT_SUCCESS;
}

#endif //GDE_ENTRY_H
