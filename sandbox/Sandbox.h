// Created by borja on 22/1/22.


#ifndef ENGINE2_0_SANDBOX_H
#define ENGINE2_0_SANDBOX_H


#include "GDE.h"

namespace GDE {

    class Sandbox : public Scene {
        private:
            UniqueDelegate<bool(MouseScrolledEvent&)> mseDelegate;
            NinePatchSprite* ui;

        public:
            Sandbox(Engine* _engine, const std::string& _debugName = "Editor") : Scene(_engine, _debugName) {  }
            void onInit() override;
            void onEvent(Event& _event) override;
            void onUpdate(Delta _dt) override;
            void onFixedUpdate(Delta _dt) override;
            void onImGuiRender(Delta _dt) override;
            void onDebugRender(Delta _dt) override;
            void onEnd() override;
            ~Sandbox() override {  };

        private:
            bool onMouseScrolled(MouseScrolledEvent& _event);
            bool run_roll(const TransitionParams& _params);
            bool roll_run(const TransitionParams& _params);

            bool uiButtonTrigger(NodeID _nodeId, Canvas* _canvas);
            void onMouseClick(MouseCode _mouseCode);
            void onMouseEntered();
            void onMouseExited();

            void box2DStressTest();
            void textStressTest();

            void redirectRendering(FrameBuffer* _frameBuffer);
    };

}


#endif //ENGINE2_0_SANDBOX_H
