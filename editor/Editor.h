//
// Created by borja on 28/07/22.
//

#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "GDE.h"
#include "core/graph/Scene.h"
#include "core/render/elements/FrameBuffer.h"

using namespace GDE;

namespace Editor {

    class Editor : public Scene {
        public:
            Editor(Engine* _engine, const std::string& _debugName = "Editor") : Scene(_engine, _debugName) {  }
            void onInit() override;

        private:
            void redirectRendering(FrameBuffer* _frameBuffer);
    };

}


#endif //ENGINE_EDITOR_H
