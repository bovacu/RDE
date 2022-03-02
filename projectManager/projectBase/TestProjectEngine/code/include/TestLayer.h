// Created by borja on 23/2/22.


#ifndef TESTPROJECTENGINE_TESTLAYER_H
#define TESTPROJECTENGINE_TESTLAYER_H

#include <core/render/layers/Layer.h>
#include <core/render/elements/Sprite.h>

using namespace engine;

class TestLayer : public Layer {
    private:
        Sprite sprite;

    public:
        TestLayer() : Layer("TestLayer") {  }
        void onInit() override;
        void onEvent(Event& _event) override;
        void onUpdate(Delta _dt) override;
        void onFixedUpdate(Delta _dt) override;
        void onRender(Delta _dt) override;
        void onImGuiRender(Delta _dt) override;
        void onEnd() override;
        ~TestLayer() override {  };
};

#endif //TESTPROJECTENGINE_TESTLAYER_H
