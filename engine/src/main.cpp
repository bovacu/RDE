#include "core/Engine.h"
#include "Sandbox.h"

int main(int, char* []) {
    engine::Engine _e;
    engine::Sandbox _sandbox;
    _e.pushLayer(&_sandbox);
    _e.onRun();

    return EXIT_SUCCESS;
}