//
// Created by borja on 9/15/22.
//

#ifndef GDEDEMO_GDEDEMO_H
#define GDEDEMO_GDEDEMO_H


#include "GDE.h"
#include "core/graph/Scene.h"
#include "core/render/elements/FrameBuffer.h"
#include "core/util/Delta.h"
#include <vector>

using namespace GDE;

class GDEDemo : public Scene {
    private:
        Transform* logTransform = nullptr;
        float logRotationSpeed = 100.f;
        int score = 0;

        TextRenderer* scoreText = nullptr;
        PhysicsBody* knifeBody = nullptr;

    public:
        explicit GDEDemo(Engine* _engine, const std::string& _debugName = "GDEDemo") : Scene(_engine, _debugName) {  }
        void onInit() override;
        void onUpdate(Delta _dt) override;
        void onDebugRender(Delta _dt) override;

        void onCollisionLog_Knife(PhysicsBody* _a, PhysicsBody* _b);
        void onCollisionKnife_Knife(PhysicsBody* _a, PhysicsBody* _b);
};


#endif //GDEDEMO_GDEDEMO_H
