//
// Created by borja on 9/15/22.
//

//
// Created by borja on 8/20/22.
//

#include "core/Engine.h"
#include "RDEEntry.h"
#include "RDEDemo.h"

class RDEDemoEntry : public RDE::Engine {
    public:
        explicit RDEDemoEntry() : RDE::Engine() {
            RDE::Engine::onInit(new RDEDemo(this));
        }

        ~RDEDemoEntry() {

        }
};

RDE::Engine* RDE::createEngine(int _argc, char** _argv) {
    return new RDEDemoEntry();
}