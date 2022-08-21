//
// Created by borja on 28/07/22.
//

#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "GDE.h"
#include "core/graph/Scene.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/util/Delta.h"
#include <vector>

using namespace GDE;

namespace Editor {

    class Editor : public Scene {
        public:
            Editor(Engine* _engine, const std::string& _debugName = "Editor") : Scene(_engine, _debugName) {  }
            void onInit() override;
            void onUpdate(Delta _dt) override;

        private:
            void redirectRendering(FrameBuffer* _frameBuffer);
            void textStressTest();
            std::vector<NodeID> nodes;
    };

}


#endif //ENGINE_EDITOR_H
