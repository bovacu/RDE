//
// Created by borja on 28/07/22.
//

#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "RDE.h"
#include "core/graph/Scene.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/util/Delta.h"
#include <vector>

using namespace RDE;

namespace Editor {

    class Editor : public Scene {

        PhysicsBody* body;
        Node* circleNode;
        bool collisionHappened = false;
        UISlider* slider;
        UniqueDelegate<void(FrameBuffer*)> redirectRenderingDel;

        public:
            explicit Editor(Engine* _engine, const std::string& _debugName = "Editor") : Scene(_engine, _debugName) {  }
            void onInit() override;
            void onUpdate(Delta _dt) override;
            void onLateUpdate(Delta _dt) override;
            void onDebugRender(Delta _dt) override;

        private:
            void redirectRendering(FrameBuffer* _frameBuffer);
            void textStressTest(int _amount);
            void physicsTest();
            void particleSystemTest();
            void localizationTest();
            std::vector<NodeID> nodes;


			void onMouseClick(RDE_MOUSE_BUTTON_ _mouseCode);
            void onMouseEntered();
            void onMouseExited();

            void onCollisionEnter(PhysicsBody* _a, PhysicsBody* _b);
            void collisionB(PhysicsBody* _a, PhysicsBody* _b);
    };

}


#endif //ENGINE_EDITOR_H
