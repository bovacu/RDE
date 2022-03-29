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

//        engine->setVSync(true);

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

        Engine::get().getMainCamera().setAdaptiveViewport(Engine::get().getWindowSize());

        mseDelegate.bind<&Sandbox::onMouseScrolled>(this);

        square = getMainGraph()->createNode("square");
        squareTransform = getMainGraph()->getComponent<Transform>(square);
        auto _squareSpriteRenderer = getMainGraph()->addComponent<SpriteRenderer>(square);
        _squareSpriteRenderer->texture = TextureAtlasManager::get().getTile("square", "square_0");
        _squareSpriteRenderer->layer = 30;

        auto _text = getMainGraph()->createNode("Text", square);
        getMainGraph()->addComponent<TextRenderer>(_text, _font, "Hello World");
        getMainGraph()->getComponent<Transform>(_text)->setPosition(0, 100);

//        Random _r;
//        for(int _i = 0; _i < 10; _i++) {
//            auto _node = getMainGraph()->createNode(std::to_string(_i), square);
//            getMainGraph()->addComponent<SpriteRenderer>(_node, TextureAtlasManager::get().getTile("square", "square_0"));
//            getMainGraph()->getComponent<Transform>(_node)->setPosition({_r.randomf(-1280, 1280), _r.randomf(-720, 720)});
//        }
    }

    void Sandbox::onEvent(Event& _event) {
        EventDispatcher _dispatcher(_event);
        _dispatcher.dispatchEvent<MouseScrolledEvent>(mseDelegate);

        getMainGraph()->onEvent(_event);
    }

    void Sandbox::onUpdate(Delta _dt) {

        auto _t = getMainGraph()->getComponent<Transform>(square);
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

        getMainGraph()->onUpdate(_dt);
    }

    void Sandbox::onFixedUpdate(Delta _dt) {
        Scene::onFixedUpdate(_dt);
        getMainGraph()->onFixedUpdate(_dt);
    }

    void Sandbox::onRender(Delta _dt) {
//        Renderer::drawGrid();
        getMainGraph()->onRender();
    }

    void Sandbox::onDebugRender(Delta _dt) {
        Scene::onDebugRender(_dt);
//        Renderer::drawSquare({100, 100}, {100, 100}, Color::Green);
    }

    void Sandbox::onImGuiRender(Delta _dt) {
        Scene::onImGuiRender(_dt);
    }

    void Sandbox::onEnd() {
        Scene::onEnd();
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