// Created by borja on 22/1/22.

#include "Sandbox.h"
#include "core/Engine.h"
#include "core/procedural/CellularAutomataMapGenerator.h"
#include <glm/gtx/string_cast.hpp>

//#if IS_ANDROID()
//#include <nativeCode/include/NativeAds.h>
//#endif

namespace engine {

    void Sandbox::onInit() {
        engine = &Engine::get();
        auto* _font = FontManager::get().loadFont("assets/fonts/arial.ttf", 54);
        
        TextureAtlasManager::get().addAtlas(50, 50, "assets/test/test.png");
        TextureAtlasManager::get().addAtlas(32, 32, "assets/test/square.png");
        TextureAtlasManager::get().addAtlas(120, 80, "assets/player/run.png");

        engine->setVSync(true);

        mainScene = new Graph("MainScene");
//        auto _player = mainScene->createNode("player");
//        auto _sprite = mainScene->addComponent<SpriteRenderer>(_player);
//        _sprite->layer = 10;
//        _sprite->texture = TextureAtlasManager::get().getTile("run", "run_0");
//        LOG_W(mainScene->getComponent<Transform>(_player)->getScaleLocal())
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

//        auto _text = mainScene->createNode("text");
//        auto* _textComp = mainScene->addComponent<Text>(_text);
//        _textComp->init(_font, "Hello World");
//        mainScene->getComponent<Transform>(_text)->setPosition({-350, 250});
//        _textComp->spriteRenderer.color = Color::Green;





        mseDelegate.bind<&Sandbox::onMouseScrolled>(this);

        square = mainScene->createNode("square");
        squareTransform = mainScene->getComponent<Transform>(square);
        auto _squareSpriteRenderer = mainScene->addComponent<SpriteRenderer>(square);
        _squareSpriteRenderer->texture = TextureAtlasManager::get().getTile("square", "square_0");
        _squareSpriteRenderer->layer = 30;

        squareChild = mainScene->createNode("squareChild", square);
        childTransform = mainScene->getComponent<Transform>(squareChild);
        auto _squareChildSpriteRenderer = mainScene->addComponent<SpriteRenderer>(squareChild);
        _squareChildSpriteRenderer->texture = TextureAtlasManager::get().getTile("square", "square_0");
        _squareChildSpriteRenderer->layer = 30;
        _squareChildSpriteRenderer->color = Color::Yellow;
        childTransform->setPosition({0, 128});
//
//        auto _childChild = mainScene->createNode("childChild", squareChild);
//        mainScene->addComponent<SpriteRenderer>(_childChild, TextureAtlasManager::get().getTile("square", "square_0"))->color = Color::Yellow;
//        mainScene->getComponent<Transform>(_childChild)->setPosition({-100, 0});
//
//        auto _childChildChild = mainScene->createNode("childChildChild", _childChild);
//        mainScene->addComponent<SpriteRenderer>(_childChildChild, TextureAtlasManager::get().getTile("square", "square_0"))->color = Color::Orange;
//        mainScene->getComponent<Transform>(_childChildChild)->setPosition({100, 50});

//        printNode(mainScene, square);
//        printNode(mainScene, squareChild);
//        printNode(mainScene, _childChild);


//        Random _r;
//        for(int _i = 0; _i < 1000; _i++) {
//            auto _node = mainScene->createNode(std::to_string(_i), square);
//            mainScene->addComponent<SpriteRenderer>(_node, TextureAtlasManager::get().getTile("square", "square_0"));
//            mainScene->getComponent<Transform>(_node)->setPosition({_r.randomf(-1.7, 1.7), _r.randomf(-1, 1)});
//        }

        LOG_W(mainScene->toString())
    }

    void Sandbox::onEvent(Event& _event) {
        EventDispatcher _dispatcher(_event);
        _dispatcher.dispatchEvent<MouseScrolledEvent>(mseDelegate);

        mainScene->onEvent(_event);
    }

    void Sandbox::onUpdate(Delta _dt) {


        auto _t = mainScene->getComponent<Transform>(square);
        if(InputManager::isKeyPressed(KeyCode::A))
            _t->translate(-_dt * 150, 0);
        else if(InputManager::isKeyPressed(KeyCode::D))
            _t->translate(_dt * 150, 0);

        if(InputManager::isKeyPressed(KeyCode::W))
            _t->translate(0, _dt * 150);
        else if(InputManager::isKeyPressed(KeyCode::S))
            _t->translate(0, -_dt * 150);

        if(InputManager::isKeyPressed(KeyCode::R))
            _t->rotate(_dt * 150);

        if(InputManager::isKeyPressed(KeyCode::Z))
            _t->scale(_dt, _dt);
        else if (InputManager::isKeyPressed(KeyCode::X))
            _t->scale(-_dt, -_dt);



        auto _t2 = mainScene->getComponent<Transform>(squareChild);
        if(InputManager::isKeyPressed(KeyCode::Left))
            _t2->translate(-_dt * 150, 0);
        else if(InputManager::isKeyPressed(KeyCode::Right))
            _t2->translate(_dt * 150, 0);

        if(InputManager::isKeyPressed(KeyCode::Up))
            _t2->translate(0, _dt * 150);
        else if(InputManager::isKeyPressed(KeyCode::Down))
            _t2->translate(0, -_dt * 150);

        if(InputManager::isKeyPressed(KeyCode::E))
            _t2->rotate(_dt * 150);

        if(InputManager::isKeyPressed(KeyCode::N))
            _t2->scale(_dt, _dt);
        else if (InputManager::isKeyPressed(KeyCode::M))
            _t2->scale(-_dt, -_dt);


        if(InputManager::isKeyJustPressed(KeyCode::Enter)) {
            mainScene->orphan(squareChild);
            LOG_W(mainScene->toString())
        }

        if(InputManager::isKeyJustPressed(KeyCode::Backspace)) {
            mainScene->setParent(squareChild, square);
            LOG_W(mainScene->toString())
        }

        mainScene->onUpdate(_dt);
    }

    void Sandbox::onFixedUpdate(Delta _dt) {
        Layer::onFixedUpdate(_dt);
        mainScene->onFixedUpdate(_dt);
    }

    void Sandbox::onRender(Delta _dt) {
//        Renderer::drawGrid();
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