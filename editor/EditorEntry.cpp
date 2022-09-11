//
// Created by borja on 8/20/22.
//

#include "core/Engine.h"
#include "GDEEntry.h"
#include "Editor.h"

namespace Editor {

    class EditorEntry : public GDE::Engine {
        public:
        explicit EditorEntry() : GDE::Engine() {
            GDE::Engine::onInit(new Editor(this));
        }

        ~EditorEntry() {

        }
    };
}

GDE::Engine* GDE::createEngine(int _argc, char** _argv) {
    return new Editor::EditorEntry();
}