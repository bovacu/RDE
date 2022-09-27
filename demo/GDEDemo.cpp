//
// Created by borja on 9/15/22.
//

#include "GDEDemo.h"

#define LOG_MASK 1
#define KNIFE_MASK 2
#define KNIFE_STUBBED_MASK 3

void GDEDemo::onInit() {
    Scene::onInit();

    auto _windowSize = engine->getWindow().getWindowSize();

    {
        auto _logNode = getMainGraph()->createNode("Log");

        logTransform = getMainGraph()->getComponent<Transform>(_logNode);
        logTransform->setScale(0.25f, 0.25f);
        logTransform->setPosition(0, _windowSize.y * 0.35);

        auto* _sprite = getMainGraph()->addComponent<SpriteRenderer>(_logNode, this,engine->manager.textureManager.getSubTexture("assets", "log"));

        BodyConfig _bodyConfig;
        _bodyConfig.shapeConfig.type = PhysicsShape::CIRCLE;
        _bodyConfig.shapeConfig.size.x = _sprite->getSize().x * 0.5f;
        _bodyConfig.isStatic = true;
        _bodyConfig.collisionMask = LOG_MASK;
        getMainGraph()->addComponent<PhysicsBody>(_logNode, this, _bodyConfig);
    }

    {
        auto _scoreNode = getMainGraph()->createNode("ScoreText");
        auto* _scoreTransform = getMainGraph()->getComponent<Transform>(_scoreNode);

        scoreText = getMainGraph()->addComponent<TextRenderer>(_scoreNode, this, engine->manager.fontManager.getDefaultFont("Yulong-Regular"), "Score: ");
        scoreText->pivot.x = 0.0f;
        auto _halfWidth = (float)_windowSize.x / 2.f;
        auto _halfHeight = (float)_windowSize.y / 2.f;
        _scoreTransform->setPosition(-_halfWidth + (float)_windowSize.x * 0.05f, _halfHeight - scoreText->getSize().y / 2.f);
    }

    auto* _collisionLog_Knife = engine->manager.physics.addOrGetCollisionToTable(LOG_MASK, KNIFE_MASK);
    auto* _collisionKnife_Knife = engine->manager.physics.addOrGetCollisionToTable(KNIFE_MASK, KNIFE_MASK);

    _collisionLog_Knife->onCollisionEnter.bind<&GDEDemo::onCollisionLog_Knife>(this);
//    _collisionKnife_Knife->onCollisionEnter.bind<&GDEDemo::onCollisionKnife_Knife>(this);
}

void GDEDemo::onUpdate(Delta _dt) {
    Scene::onUpdate(_dt);

    logTransform->rotate(50.f * _dt);

    if(engine->manager.inputManager.isMouseJustPressed(MouseCode::ButtonLeft)) {
        auto _knifeNode = getMainGraph()->createNode(APPEND_S("Knife", score));
        BodyConfig _bodyConfig;
        _bodyConfig.shapeConfig.type = PhysicsShape::BOX;
        _bodyConfig.shapeConfig.size = { 32, 32 };
        _bodyConfig.collisionMask = KNIFE_MASK;
        knifeBody = getMainGraph()->addComponent<PhysicsBody>(_knifeNode, this, _bodyConfig);
    }

    if(knifeBody != nullptr) {
        knifeBody->transform->translate({2, 500 * _dt});
    }
}

void GDEDemo::onDebugRender(Delta _dt) {
    Scene::onDebugRender(_dt);
}

void GDEDemo::onCollisionLog_Knife(PhysicsBody* _a, PhysicsBody* _b) {
    auto _logNode = std::equal(getMainGraph()->getComponent<Tag>(_a->transform->ID)->tag.begin(),
                           getMainGraph()->getComponent<Tag>(_a->transform->ID)->tag.end(), "Log") ? _a->transform->ID : _b->transform->ID;
    auto _knifeNode = _a->transform->ID == _logNode ? _b->transform->ID : _a->transform->ID;
    getMainGraph()->setParent(_knifeNode, _logNode);
    knifeBody->ignorePhysics = true;
    knifeBody->collisionMask = KNIFE_STUBBED_MASK;
    knifeBody = nullptr;
    score++;
}

void GDEDemo::onCollisionKnife_Knife(PhysicsBody* _a, PhysicsBody* _b) {

}
