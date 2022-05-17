// Created by borja on 22/1/22.


#ifndef ENGINE2_0_SANDBOX_H
#define ENGINE2_0_SANDBOX_H


#include "GDE.h"

namespace GDE {

    class Sandbox : public Scene {
        private:
            AnimationSystem animationSystem;
            TransitionParams params;
            Engine* engine;
            UDelegate<bool(MouseScrolledEvent&)> mseDelegate;

            NodeID square, squareChild, secondaryCamera;
            Transform* squareTransform;
            Transform* childTransform;

        public:
            Sandbox(Engine* _engine, const std::string& _debugName = "Sandbox") : Scene(_engine, _debugName) {  }
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

            void box2DStressTest();
            void textStressTest();
    };

}


#endif //ENGINE2_0_SANDBOX_H
