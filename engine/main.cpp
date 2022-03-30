#ifndef RUN_TESTS
    #include "GDE.h"
    #include "Sandbox.h"

    int main(int, char* []) {
        engine::Engine _e;
        _e.onInit();
        auto* _projectManager = new engine::Sandbox;
        _e.setScene(_projectManager);
        _e.onRun();
        _e.destroy();

        return EXIT_SUCCESS;
    }
#endif