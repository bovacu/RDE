// Created by borja on 23/2/22.

#include "engine/include/core/render/elements/TextureAtlasManager.h"
#include "engine/include/core/render/Renderer.h"
#include "code/include/TestLayer.h"

void TestLayer::onInit() {
    sprite.init();
    TextureAtlasManager::get().addAtlas(256, 256, "assets/logo.png");
    sprite.setTexture(TextureAtlasManager::get().getTile("logo", "logo_0"));
    Renderer::setClearColor(Color::Red);
}

void TestLayer::onEvent(Event& _event) {

}

void TestLayer::onUpdate(Delta _dt) {

}

void TestLayer::onFixedUpdate(Delta _dt) {
    Layer::onFixedUpdate(_dt);
}

void TestLayer::onRender(Delta _dt) {
    Renderer::draw(sprite);
}

void TestLayer::onImGuiRender(Delta _dt) {
    Layer::onImGuiRender(_dt);
}

void TestLayer::onEnd() {
    Layer::onEnd();
}
