//
// Created by borja on 8/20/22.
//

#include "core/Engine.h"
#include "RDEEntry.h"
#include "Editor.h"

namespace Editor {

    class EditorEntry : public RDE::Engine {
        public:
        explicit EditorEntry() : RDE::Engine() {
            RDE::Engine::onInit(new Editor(this));
        }

        ~EditorEntry() {

        }
    };
}

RDE::Engine* RDE::createEngine(int _argc, char** _argv) {
    return new Editor::EditorEntry();
}