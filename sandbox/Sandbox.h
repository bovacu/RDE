// Created by borja on 22/1/22.


#ifndef ENGINE2_0_SANDBOX_H
#define ENGINE2_0_SANDBOX_H


#include "core/util/Util.h"
#include "engine/include/core/graph/Scene.h"
#include "core/render/Camera.h"
#include "core/systems/animationSystem/AnimationSystem.h"
#include "core/render/window/event/JoystickEvent.h"
#include "core/Engine.h"
#include "core/render/window/event/MobileEvent.h"
#include "core/graph/Graph.h"

namespace engine {

    class Sandbox : public Scene {
        private:
            AnimationSystem animationSystem;
            TransitionParams params;
            Engine* engine;
            UDelegate<bool(MouseScrolledEvent&)> mseDelegate;

            NodeID square, squareChild;
            Transform* squareTransform;
            Transform* childTransform;

        public:
            Sandbox() : Scene("Sandbox") {  }
            void onInit() override;
            void onEvent(Event& _event) override;
            void onUpdate(Delta _dt) override;
            void onFixedUpdate(Delta _dt) override;
            void onRender(Delta _dt) override;
            void onImGuiRender(Delta _dt) override;
            void onDebugRender(Delta _dt) override;
            void onEnd() override;
            ~Sandbox() override {  };

        private:
            bool onMouseScrolled(MouseScrolledEvent& _event);
            bool run_roll(const TransitionParams& _params);
            bool roll_run(const TransitionParams& _params);
    };

}


#endif //ENGINE2_0_SANDBOX_H
