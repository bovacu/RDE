#include "core/Engine.h"
#include "Sandbox.h"
#include "code/include/ProjectManagerLayer.h"

int main(int, char* []) {
    engine::Engine _e;
    auto* _projectManager = new engine::ProjectManagerLayer;
    _e.pushLayer(_projectManager);
    _e.onRun();
    _e.destroy();

    return EXIT_SUCCESS;
}