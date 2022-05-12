// Created by borja on 22/1/22.

#include "Sandbox.h"
#include "core/systems/physicsSystem/Physics.h"

//#if IS_ANDROID()
//#include <nativeCode/include/NativeAds.h>
//#endif

namespace GDE {

    static void test(b2Contact* _contact) {
        LOG_I("Hello world!!")
    }

    void Sandbox::onInit() {
        engine = &Engine::get();
        auto *_font = FontManager::get().loadFont("assets/fonts/arial.ttf", 54);

        TextureAtlasManager::get().addAtlas(50, 50, "assets/test/test.png");
        TextureAtlasManager::get().addAtlas(32, 32, "assets/test/square.png");
        TextureAtlasManager::get().addAtlas(120, 80, "assets/player/run.png");

        engine->setVSync(true);

        /*        auto _player = mainScene->createNode("player");
                auto _sprite = mainScene->addComponent<SpriteRenderer>(_player);
                _sprite->layer = 10;
                _sprite->texture = TextureAtlasManager::get().getTile("run",
                "run_0");
                LOG_W(mainScene->getComponent<Transform>(_player)->getScaleLocal())

                auto _animationSystem =
                mainScene->addComponent<AnimationSystem>(_player);

                _animationSystem->createAnimation("run", "run", {0, 1, 2, 3, 4, 5,
                6, 7, 8, 9}); _animationSystem->setInitialAnimation("run");
                _animationSystem->createAnimation("roll", "run", {12, 13, 14, 15,
                16, 17, 18, 19, 20, 21, 22, 23});
                _animationSystem->createTransition<&Sandbox::run_roll>("run",
                "roll", this);
                _animationSystem->createTransition<&Sandbox::roll_run>("roll",
                "run", this); _animationSystem->setInitialAnimation("run");
                _animationSystem->start();
        */

        getMainCamera()->setAdaptiveViewport(engine->getWindowSize(), engine->getWindowSize());

        mseDelegate.bind<&Sandbox::onMouseScrolled>(this);

        square = getMainGraph()->createNode("square");
        squareTransform = getMainGraph()->getComponent<Transform>(square);
        squareTransform->setPosition(-25, 0);
        auto _squareSpriteRenderer = getMainGraph()->addComponent<SpriteRenderer>(square);
        _squareSpriteRenderer->texture = TextureAtlasManager::get().getTile("square", "square_0");

        BodyConfig _bodyConf {
            1,
            {32, 32},
            0,
            0,
            1 << 1
        };
        getMainGraph()->addComponent<Body>(square, _bodyConf, squareTransform);

        auto _floor = getMainGraph()->createNode("floor");
        auto* _floorTransform = getMainGraph()->getComponent<Transform>(_floor);
        _floorTransform->setPosition(0, -128);
        auto _floorSpriteRenderer = getMainGraph()->addComponent<SpriteRenderer>(_floor);
        _floorSpriteRenderer->texture = TextureAtlasManager::get().getTile("square", "square_0");
        _floorSpriteRenderer->color = Color::Blue;

        BodyConfig _bodyConf2 {
                .mass = 1,
                .size = {32, 32},
                .friction = 0,
                .restitution = 0,
                .mask = 1 << 2,
                .bodyType = BodyType::STATIC
        };
        getMainGraph()->addComponent<Body>(_floor, _bodyConf2, _floorTransform);

        auto _text = getMainGraph()->createNode("Text");
        getMainGraph()->addComponent<TextRenderer>(_text, _font, "Hello World")->setColor(Color::Green);
        getMainGraph()->getComponent<Transform>(_text)->setPosition(0, 100);

//        Physics::get().setCallbackForCollisionBetweenMasks(1 << 2, 1 << 1).bind<&test>();

//        box2DStressTest();
//        textStressTest();
    }

    void Sandbox::onEvent(Event &_event) {
        EventDispatcher _dispatcher(_event);
        _dispatcher.dispatchEvent<MouseScrolledEvent>(mseDelegate);

        getMainGraph()->onEvent(_event);
    }

    void Sandbox::onUpdate(Delta _dt) {
        auto _t = getMainGraph()->getComponent<Transform>(square);
        if (InputManager::isKeyPressed(KeyCode::A))
            _t->translate(-_dt * 150, 0);
        else if (InputManager::isKeyPressed(KeyCode::D))
            _t->translate(_dt * 150, 0);

        if (InputManager::isKeyPressed(KeyCode::W))
            _t->translate(0, _dt * 150);
        else if (InputManager::isKeyPressed(KeyCode::S))
            _t->translate(0, -_dt * 150);

        if (InputManager::isKeyPressed(KeyCode::R))
            _t->rotate(_dt * 150);

        if (InputManager::isKeyPressed(KeyCode::Z))
            _t->scale(_dt, _dt);
        else if (InputManager::isKeyPressed(KeyCode::X))
            _t->scale(-_dt, -_dt);

        if (InputManager::isKeyPressed(KeyCode::Space))
            _t->setPosition(-150, _t->getPositionLocal().y);

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
    }

    void Sandbox::onImGuiRender(Delta _dt) { Scene::onImGuiRender(_dt); }

    void Sandbox::onEnd() { Scene::onEnd(); }

    bool Sandbox::onMouseScrolled(MouseScrolledEvent &_event) {
        Camera &_camera = *getMainCamera();
        float _zoom = _camera.getCurrentZoomLevel();
        _zoom -= _event.getScrollY() * _camera.getZoomSpeed();
        _zoom = std::max(_zoom, 0.1f);
        _camera.setCurrentZoomLevel(_zoom);
        return true;
    }

    bool Sandbox::run_roll(const TransitionParams &_params) {
        return InputManager::isKeyJustPressed(Key::Space);
    }

    bool Sandbox::roll_run(const TransitionParams &_params) {
        return InputManager::isKeyJustPressed(Key::Backspace);
    }

    void Sandbox::box2DStressTest() {
        auto _leftWall = getMainGraph()->createNode("LeftWall");
        auto* _leftWallTransform = getMainGraph()->getComponent<Transform>(_leftWall);
        _leftWallTransform->setPosition(-(float)engine->getWindowSize().x / 2.f, 0);
        BodyConfig _leftWallConfig {
            .mass = 1,
            .size = {64, (float)engine->getWindowSize().y},
            .restitution = 1.f,
            .mask = 1 << 2,
            .bodyType = BodyType::STATIC,
            .bodyShapeType = BodyShapeType::BOX,
        };
        getMainGraph()->addComponent<Body>(_leftWall, _leftWallConfig, _leftWallTransform);


        auto _rightWall = getMainGraph()->createNode("RightWall");
        auto* _rightWallTransform = getMainGraph()->getComponent<Transform>(_rightWall);
        _rightWallTransform->setPosition((float)engine->getWindowSize().x / 2.f, 0);
        BodyConfig _rightWallConfig {
                .mass = 1,
                .size = {64, (float)engine->getWindowSize().y},
                .restitution = 1.f,
                .mask = 1 << 2,
                .bodyType = BodyType::STATIC,
                .bodyShapeType = BodyShapeType::BOX,
        };
        getMainGraph()->addComponent<Body>(_rightWall, _rightWallConfig, _rightWallTransform);


        auto _bottomWall = getMainGraph()->createNode("BottomWall");
        auto* _bottomWallTransform = getMainGraph()->getComponent<Transform>(_bottomWall);
        _bottomWallTransform->setPosition(0, -(float)engine->getWindowSize().y / 2.f);
        BodyConfig _bottomWallConfig {
                .mass = 1,
                .size = {(float)engine->getWindowSize().x, 64},
                .restitution = 1.f,
                .mask = 1 << 2,
                .bodyType = BodyType::STATIC,
                .bodyShapeType = BodyShapeType::BOX,
        };
        getMainGraph()->addComponent<Body>(_bottomWall, _bottomWallConfig, _bottomWallTransform);


        auto _topWall = getMainGraph()->createNode("TopWall");
        auto* _topWallTransform = getMainGraph()->getComponent<Transform>(_topWall);
        _topWallTransform->setPosition(0, (float)engine->getWindowSize().y / 2.f);
        BodyConfig _topWallConfig {
                .mass = 1,
                .size = {(float)engine->getWindowSize().x, 64},
                .restitution = 1.f,
                .mask = 1 << 2,
                .bodyType = BodyType::STATIC,
                .bodyShapeType = BodyShapeType::BOX,
        };
        getMainGraph()->addComponent<Body>(_topWall, _topWallConfig, _topWallTransform);

        Random _r;
        // 1075 is the maximum I could get with 60fps of average performance
        for(int _i = 0; _i < 100; _i++) {
            auto _square = getMainGraph()->createNode("Square" + std::to_string(_i));
            auto* _squareTransform = getMainGraph()->getComponent<Transform>(_square);
            _squareTransform->setPosition(_r.randomf(-(float)engine->getWindowSize().x / 2.f + 64, (float)engine->getWindowSize().x / 2.f - 64),
                                          _r.randomf(-(float)engine->getWindowSize().y / 2.f + 64, (float)engine->getWindowSize().y / 2.f - 64));
            _squareTransform->setScale(0.25f, 0.25f);
            BodyConfig _squareWallConfig {
                    .mass = 1,
                    .size = {8, 8},
                    .restitution = 1.f,
                    .mask = 1 << 1,
                    .bodyType = BodyType::DYNAMIC,
                    .bodyShapeType = BodyShapeType::BOX,
            };
            getMainGraph()->addComponent<Body>(_square, _squareWallConfig, _squareTransform);
            auto _squareSpriteRenderer = getMainGraph()->addComponent<SpriteRenderer>(_square);
            _squareSpriteRenderer->texture = TextureAtlasManager::get().getTile("square", "square_0");
        }
    }

    void Sandbox::textStressTest() {
        Random _r;
        // 25000 is the maximum I could get with 60fps of average performance, with texts -> "Text[0-25000]"
        for(int _i = 0; _i < 25000; _i++) {
            auto _text = getMainGraph()->createNode("Text" + std::to_string(_i));
            auto* _textTransform = getMainGraph()->getComponent<Transform>(_text);
            _textTransform->setPosition(_r.randomf(-(float)engine->getWindowSize().x / 2.f + 64, (float)engine->getWindowSize().x / 2.f - 64),
                                        _r.randomf(-(float)engine->getWindowSize().y / 2.f + 64, (float)engine->getWindowSize().y / 2.f - 64));
            getMainGraph()->addComponent<TextRenderer>(_text, FontManager::get().getDefaultFont("arial"), "Text" + std::to_string(_i));
        }
    }
} // namespace engine
