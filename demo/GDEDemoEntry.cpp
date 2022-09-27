//
// Created by borja on 9/15/22.
//

//
// Created by borja on 8/20/22.
//

#include "core/Engine.h"
#include "GDEEntry.h"
#include "GDEDemo.h"

class GDEDemoEntry : public GDE::Engine {
    public:
        explicit GDEDemoEntry() : GDE::Engine() {
            GDE::Engine::onInit(new GDEDemo(this));
        }

        ~GDEDemoEntry() {

        }
};

GDE::Engine* GDE::createEngine(int _argc, char** _argv) {
    return new GDEDemoEntry();
}