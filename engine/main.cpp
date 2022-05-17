#ifndef RUN_TESTS
    #include "GDE.h"
    #include "Sandbox.h"

    int main(int, char* []) {
        GDE::Engine _e;
        _e.onInit(new GDE::Sandbox(&_e));
        _e.onRun();
        _e.destroy();
        return EXIT_SUCCESS;
    }
#endif