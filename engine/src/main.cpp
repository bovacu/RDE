#include "core/Engine.h"
#include "Sandbox.h"

int main(int, char* []) {
    engine::Engine _e;
    auto* _sandbox = new engine::Sandbox;
    _e.pushLayer(_sandbox);
    _e.onRun();

    return EXIT_SUCCESS;
}