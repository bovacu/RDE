#ifndef RUN_TESTS
    #include "core/Engine.h"
    #include "Sandbox.h"

    int main(int, char* []) {
        engine::Engine _e;
        auto* _projectManager = new engine::Sandbox;
        _e.pushLayer(_projectManager);
        _e.onRun();
        _e.destroy();

        return EXIT_SUCCESS;
    }
#endif