//
// Created by borja on 8/20/22.
//

#ifndef GDE_GDE_ENTRY_H
#define GDE_GDE_ENTRY_H

#include "core/Engine.h"

int main(int argc, char** argv) {
    auto* _e = GDE::createEngine();
    _e->onRun();
    _e->destroy();
    delete _e;
    return EXIT_SUCCESS;
}

#endif //GDE_GDE_ENTRY_H
