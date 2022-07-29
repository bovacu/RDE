#ifndef RUN_TESTS
    #include "GDE.h"
    #include "Editor.h"

int main(int, char* []) {
        GDE::Engine _e;
        _e.onInit(new Editor::Editor(&_e));
        _e.onRun();
        _e.destroy();
        return EXIT_SUCCESS;
    }
#endif