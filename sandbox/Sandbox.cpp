// Created by borja on 22/1/22.

#include "Sandbox.h"
#include "core/Engine.h"
#include "core/procedural/CellularAutomataMapGenerator.h"

//#if IS_ANDROID()
//#include <nativeCode/include/NativeAds.h>
//#endif

namespace engine {

    static void printNode(Scene* _scene, const NodeID& _nodeID) {
        auto _hierarchy = _scene->getComponent<Hierarchy>(_nodeID);
        auto _tag = _scene->getComponent<Tag>(_nodeID);
        LOG_W("Tag: ", _tag->tag, ", Value: ", (int)_nodeID, ", Parent: ", (int)_hierarchy->parent, ", Previous Brother: ", (int)_hierarchy->prevBrother, ", Next Brother: ", (int)_hierarchy->nextBrother, ", First Child: ", (int)_hierarchy->firstChild, " Number of Children: ", _hierarchy->children)
    }

    void Sandbox::onInit() {
        engine = &Engine::get();
        auto* _font = FontManager::get().loadFont("assets/fonts/arial.ttf", 54);
        
        TextureAtlasManager::get().addAtlas(50, 50, "assets/test/test.png");
        TextureAtlasManager::get().addAtlas(32, 32, "assets/test/square.png");
        TextureAtlasManager::get().addAtlas(120, 80, "assets/player/run.png");

        mainScene = new Scene("MainScene");
//        auto _player = mainScene->createNode("player");
//        auto _sprite = mainScene->addComponent<SpriteRenderer>(_player);
//        _sprite->layer = 10;
//        _sprite->texture = TextureAtlasManager::get().getTile("run", "run_0");
//        mainScene->getComponent<Transform>(_player)->setPosition({95, 0});
//
//        auto _animationSystem = mainScene->addComponent<AnimationSystem>(_player);
//
//        _animationSystem->createAnimation("run", "run", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
//        _animationSystem->setInitialAnimation("run");
//        _animationSystem->createAnimation("roll", "run", {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23});
//        _animationSystem->createTransition<&Sandbox::run_roll>("run", "roll", this);
//        _animationSystem->createTransition<&Sandbox::roll_run>("roll", "run", this);
//        _animationSystem->setInitialAnimation("run");
//        _animationSystem->start();
//
//        auto _text = mainScene->createNode("text");
//        auto* _textComp = mainScene->addComponent<Text>(_text);
//        _textComp->init(_font, "Hello World");
//        mainScene->getComponent<Transform>(_text)->setPosition({-350, 250});
//        _textComp->spriteRenderer.color = Color::Green;
//
//        int _line = 0;
//        int _row = 0;
//        for(int _i = 0; _i < 36 ; _i++) {
//            if(_i != 0 && _i % 6 == 0) {
//                _line++;
//                _row = 0;
//            }
//
//            int _index = _i > 35 ? 35 : _i;
//            auto _s = mainScene->createNode("test_" + std::to_string(_index));
//            auto _spriteRenderer = mainScene->addComponent<SpriteRenderer>(_s);
//            _spriteRenderer->texture = TextureAtlasManager::get().getTile("test", "test_" + std::to_string(_index));
//            _spriteRenderer->layer = 1;
//
//            mainScene->getComponent<Transform>(_s)->setPosition({60.f + (float)52 * _row, (float)(200 - _line * 52)});
//
//            _row++;
//        }
//
//        mseDelegate.bind<&Sandbox::onMouseScrolled>(this);
//
//        square = mainScene->createNode("square");
//        squareTransform = mainScene->getComponent<Transform>(square);
//        auto _squareSpriteRenderer = mainScene->addComponent<SpriteRenderer>(square);
//        _squareSpriteRenderer->texture = TextureAtlasManager::get().getTile("square", "square_0");
//        _squareSpriteRenderer->layer = 30;
//
//        squareChild = mainScene->createNode("squareChild", square);
//        auto _squareChildSpriteRenderer = mainScene->addComponent<SpriteRenderer>(squareChild);
//        _squareChildSpriteRenderer->texture = TextureAtlasManager::get().getTile("square", "square_0");
//        _squareChildSpriteRenderer->color = Color::Green;
//        _squareChildSpriteRenderer->layer = 30;
//        mainScene->getComponent<Transform>(squareChild)->setPosition({50, 50});

        auto _a = mainScene->createNode("A");
        auto _g = mainScene->createNode("G", _a);
        auto _j = mainScene->createNode("J", _g);
        auto _h = mainScene->createNode("H", _g);
        auto _i = mainScene->createNode("I", _g);

        printNode(mainScene, _a);
        printNode(mainScene, _g);
        printNode(mainScene, _j);
        printNode(mainScene, _h);
        printNode(mainScene, _i);
        LOG_W(mainScene->toString())

        LOG_W("----------------------------------")

        mainScene->removeNode(_h);
        printNode(mainScene, _a);
        printNode(mainScene, _g);
        printNode(mainScene, _j);
//        printNode(mainScene, _h);
        printNode(mainScene, _i);

        LOG_W(mainScene->toString())
    }

    void Sandbox::onEvent(Event& _event) {
        EventDispatcher _dispatcher(_event);
        _dispatcher.dispatchEvent<MouseScrolledEvent>(mseDelegate);

        mainScene->onEvent(_event);
    }

    void Sandbox::onUpdate(Delta _dt) {
        if(InputManager::isKeyJustPressed(KeyCode::Enter)) {
            mainScene->getComponent<SpriteRenderer>(mainScene->getNode("player"))->shaderID = ShaderManager::get().getShader("outline");
        }

        if(InputManager::isKeyJustPressed(KeyCode::Escape)) {
            mainScene->getComponent<SpriteRenderer>(mainScene->getNode("player"))->shaderID = ShaderManager::get().getShader("basic");
        }

        if(InputManager::isGamepadButtonJustPressed(GamePadButtons::ButtonA))
            InputManager::gamepadVibrate();


        if(InputManager::isKeyPressed(KeyCode::A))
            squareTransform->setPosition({squareTransform->getPosition().x - 50 * _dt, squareTransform->getPosition().y});
        else if(InputManager::isKeyPressed(KeyCode::D))
            squareTransform->setPosition({squareTransform->getPosition().x + 50 * _dt, squareTransform->getPosition().y});

        if(InputManager::isKeyPressed(KeyCode::W))
            squareTransform->setPosition({squareTransform->getPosition().x, squareTransform->getPosition().y + 50 * _dt});
        else if(InputManager::isKeyPressed(KeyCode::S))
            squareTransform->setPosition({squareTransform->getPosition().x, squareTransform->getPosition().y - 50 * _dt});

        mainScene->onUpdate(_dt);
    }

    void Sandbox::onFixedUpdate(Delta _dt) {
        Layer::onFixedUpdate(_dt);
        mainScene->onFixedUpdate(_dt);
    }

    void Sandbox::onRender(Delta _dt) {
        mainScene->onRender();
    }

    void Sandbox::onImGuiRender(Delta _dt) {
        Layer::onImGuiRender(_dt);
    }

    void Sandbox::onEnd() {
        Layer::onEnd();
        delete mainScene;
    }

    bool Sandbox::onMouseScrolled(MouseScrolledEvent& _event) {
        Camera& _camera = Engine::get().getMainCamera();
        float _zoom = _camera.getCurrentZoomLevel();
        _zoom -= _event.getScrollY() * _camera.getZoomSpeed();
        _zoom = std::max(_zoom, 0.1f);
        _camera.setCurrentZoomLevel(_zoom);
        return true;
    }

    bool Sandbox::run_roll(const TransitionParams& _params) {
        return InputManager::isKeyJustPressed(Key::Space);
    }

    bool Sandbox::roll_run(const TransitionParams& _params) {
        return InputManager::isKeyJustPressed(Key::Backspace);
    }
}