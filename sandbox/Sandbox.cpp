// Created by borja on 22/1/22.

#include "Sandbox.h"
#include "core/systems/physicsSystem/PhysicsManager.h"
#include "core/systems/uiSystem/Canvas.h"

//#if IS_ANDROID()
//#include <nativeCode/include/NativeAds.h>
//#endif

namespace GDE {

    static void test(b2Contact* _contact) {
        LOG_I("Hello world!!")
    }

    void Sandbox::redirectRendering(FrameBuffer* _frameBuffer) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        auto viewportSize = ImVec2 { viewportPanelSize.x, viewportPanelSize.y };

        uint32_t textureID = _frameBuffer->getColorAttachmentRendererID();
        ImGui::Image((void*)(intptr_t)textureID, ImVec2{ 1280, 720 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void Sandbox::onInit() {
        engine->getWindow()->setVSync(true);

        /*        auto _player = mainScene->createNode("player");
                auto _sprite = mainScene->addComponent<SpriteRenderer>(_player);
                _sprite->layer = 10;
                _sprite->texture = TextureAtlasManager::get().getSubTexture("run",
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

        mseDelegate.bind<&Sandbox::onMouseScrolled>(this);
//        mainCamera->setAdaptiveViewport(engine->getWindowSize(), engine->getWindowSize());

//        BodyConfig _bodyConf {
//            1,
//            {32, 32},
//            0,
//            0,
//            1 << 1
//        };
//        auto _playerID = getMainGraph()->getNode("Sprite0");
//        getMainGraph()->addComponent<Body>(_playerID, _bodyConf, getMainGraph()->getComponent<Transform>(_playerID));
//
//        BodyConfig _bodyConf2 {
//                .mass = 1,
//                .size = {32, 32},
//                .friction = 0,
//                .restitution = 0,
//                .mask = 1 << 2,
//                .bodyType = BodyType::STATIC
//        };
//        auto _floorID = getMainGraph()->getNode("Sprite1");
//        getMainGraph()->addComponent<Body>(_floorID, _bodyConf2,  getMainGraph()->getComponent<Transform>(_floorID));

//        Physics::get().setCallbackForCollisionBetweenMasks(1 << 2, 1 << 1).bind<&test>();

//        box2DStressTest();
//        textStressTest();


//        auto _uiTest = getCanvases()[0]->graph->createNode("TestUINode");
//        auto* _transform = getCanvases()[0]->graph->getComponent<Transform>(_uiTest);
//        _transform->setPosition(-250, 0);
//        ui = getCanvases()[0]->graph->addComponent<NinePatchSprite>(_uiTest, this, getCanvases()[0],
//                                                                                       engine->manager.textureManager.getSubTexture(
//                                                                                               "ui", "panel0"));
//        ui->ninePatchSize = {200, 128};
//        ui->interaction->interactionTrigger.bind<&Sandbox::uiButtonTrigger>(this);
//        ui->interaction->onClick.bind<&Sandbox::onMouseClick>(this);
//        ui->interaction->onMouseEntered.bind<&Sandbox::onMouseEntered>(this);
//        ui->interaction->onMouseExited.bind<&Sandbox::onMouseExited>(this);
//
//        UniqueDelegate<void(FrameBuffer*)> _redirectFunc;
//        _redirectFunc.bind<&Sandbox::redirectRendering>(this);
//        engine->setRenderingRedirectionToImGui(_redirectFunc);
//
//        mainCamera->setFreeViewport(engine->getWindow()->getWindowSize());
//        getCanvases()[0]->matchMainCameraViewPort();
    }

    void Sandbox::onEvent(Event &_event) {
        Scene::onEvent(_event);

        EventDispatcher _dispatcher(_event);
        _dispatcher.dispatchEvent<MouseScrolledEvent>(mseDelegate);
    }

    void Sandbox::onUpdate(Delta _dt) {
        Scene::onUpdate(_dt);
    }

    void Sandbox::onFixedUpdate(Delta _dt) {
        Scene::onFixedUpdate(_dt);

    }

    void Sandbox::onDebugRender(Delta _dt) {
        Scene::onDebugRender(_dt);
//        Renderer::drawSquare({-250, 0}, {437, 128});
    }

    void Sandbox::onImGuiRender(Delta _dt) { Scene::onImGuiRender(_dt); }

    void Sandbox::onEnd() { Scene::onEnd(); }

    bool Sandbox::onMouseScrolled(MouseScrolledEvent &_event) {
        Camera &_camera = *mainCamera;
        float _zoom = _camera.getCurrentZoomLevel();
        _zoom -= _event.getScrollY() * _camera.getZoomSpeed();
        _zoom = std::max(_zoom, 0.1f);
        _camera.setCurrentZoomLevel(_zoom);
        return true;
    }

    bool Sandbox::run_roll(const TransitionParams &_params) {
        return engine->manager.inputManager.isKeyJustPressed(Key::Space);
    }

    bool Sandbox::roll_run(const TransitionParams &_params) {
        return engine->manager.inputManager.isKeyJustPressed(Key::Backspace);
    }

    void Sandbox::box2DStressTest() {
        auto _leftWall = getMainGraph()->createNode("LeftWall");
        auto* _leftWallTransform = getMainGraph()->getComponent<Transform>(_leftWall);
        _leftWallTransform->setPosition(-(float)engine->getWindow()->getWindowSize().x / 2.f, 0);
        BodyConfig _leftWallConfig {
            .mass = 1,
            .size = {64, (float)engine->getWindow()->getWindowSize().y},
            .restitution = 1.f,
            .mask = 1 << 2,
            .bodyType = BodyType::STATIC,
            .bodyShapeType = BodyShapeType::BOX,
        };
        getMainGraph()->addComponent<Body>(_leftWall, this, _leftWallConfig, _leftWallTransform);


        auto _rightWall = getMainGraph()->createNode("RightWall");
        auto* _rightWallTransform = getMainGraph()->getComponent<Transform>(_rightWall);
        _rightWallTransform->setPosition((float)engine->getWindow()->getWindowSize().x / 2.f, 0);
        BodyConfig _rightWallConfig {
                .mass = 1,
                .size = {64, (float)engine->getWindow()->getWindowSize().y},
                .restitution = 1.f,
                .mask = 1 << 2,
                .bodyType = BodyType::STATIC,
                .bodyShapeType = BodyShapeType::BOX,
        };
        getMainGraph()->addComponent<Body>(_rightWall, this, _rightWallConfig, _rightWallTransform);


        auto _bottomWall = getMainGraph()->createNode("BottomWall");
        auto* _bottomWallTransform = getMainGraph()->getComponent<Transform>(_bottomWall);
        _bottomWallTransform->setPosition(0, -(float)engine->getWindow()->getWindowSize().y / 2.f);
        BodyConfig _bottomWallConfig {
                .mass = 1,
                .size = {(float)engine->getWindow()->getWindowSize().x, 64},
                .restitution = 1.f,
                .mask = 1 << 2,
                .bodyType = BodyType::STATIC,
                .bodyShapeType = BodyShapeType::BOX,
        };
        getMainGraph()->addComponent<Body>(_bottomWall, this, _bottomWallConfig, _bottomWallTransform);


        auto _topWall = getMainGraph()->createNode("TopWall");
        auto* _topWallTransform = getMainGraph()->getComponent<Transform>(_topWall);
        _topWallTransform->setPosition(0, (float)engine->getWindow()->getWindowSize().y / 2.f);
        BodyConfig _topWallConfig {
                .mass = 1,
                .size = {(float)engine->getWindow()->getWindowSize().x, 64},
                .restitution = 1.f,
                .mask = 1 << 2,
                .bodyType = BodyType::STATIC,
                .bodyShapeType = BodyShapeType::BOX,
        };
        getMainGraph()->addComponent<Body>(_topWall, this, _topWallConfig, _topWallTransform);

        Random _r;
        // 1075 is the maximum I could get with 60fps of average performance
        for(int _i = 0; _i < 100; _i++) {
            auto _square = getMainGraph()->createNode("Square" + std::to_string(_i));
            auto* _squareTransform = getMainGraph()->getComponent<Transform>(_square);
            _squareTransform->setPosition(_r.randomf(-(float)engine->getWindow()->getWindowSize().x / 2.f + 64, (float)engine->getWindow()->getWindowSize().x / 2.f - 64),
                                          _r.randomf(-(float)engine->getWindow()->getWindowSize().y / 2.f + 64, (float)engine->getWindow()->getWindowSize().y / 2.f - 64));
            _squareTransform->setScale(0.25f, 0.25f);
            BodyConfig _squareWallConfig {
                    .mass = 1,
                    .size = {8, 8},
                    .restitution = 1.f,
                    .mask = 1 << 1,
                    .bodyType = BodyType::DYNAMIC,
                    .bodyShapeType = BodyShapeType::BOX,
            };
            getMainGraph()->addComponent<Body>(_square, this, _squareWallConfig, _squareTransform);
            auto _squareSpriteRenderer = getMainGraph()->addComponent<SpriteRenderer>(_square, this);
            _squareSpriteRenderer->setTexture(engine->manager.textureManager.getSubTexture("square", "square_0"));
        }
    }

    void Sandbox::textStressTest() {
        Random _r;
        // 40000 is the maximum I could get with 55fps of average performance, with texts -> "Text[0-40000]" ~350000 images
        auto _parent = getMainGraph()->createNode("TextParent");
        for(int _i = 0; _i < 1000; _i++) {
            auto _text = getMainGraph()->createNode("Text" + std::to_string(_i), _parent);
            auto* _textTransform = getMainGraph()->getComponent<Transform>(_text);
            _textTransform->setPosition(_r.randomf(-(float)engine->getWindow()->getWindowSize().x / 2.f + 64, (float)engine->getWindow()->getWindowSize().x / 2.f - 64),
                                        _r.randomf(-(float)engine->getWindow()->getWindowSize().y / 2.f + 64, (float)engine->getWindow()->getWindowSize().y / 2.f - 64));
            getMainGraph()->addComponent<TextRenderer>(_text, this, engine->manager.fontManager.getDefaultFont("arial"), "Text" + std::to_string(_i));
        }
    }

    bool Sandbox::uiButtonTrigger(NodeID _nodeId, Canvas* _canvas) {
        auto* _ninePatch = _canvas->graph->getComponent<NinePatchSprite>(_nodeId);
        auto* _transform = _canvas->graph->getComponent<Transform>(_nodeId);
        auto _mousePos = engine->manager.inputManager.getMousePosWorldPos();

        return _mousePos.isInside(_transform->getPositionWorld(), Vec2F {(float)_ninePatch->ninePatchSize.x, (float)_ninePatch->ninePatchSize.y});
    }

    void Sandbox::onMouseClick(MouseCode _mouseCode) {
        LOG_I("I'm inside the UI sprite!!")
    }

    void Sandbox::onMouseEntered() {
        LOG_I("Entered!")
    }

    void Sandbox::onMouseExited() {
        LOG_I("Exited!")
    }
} // namespace engine
