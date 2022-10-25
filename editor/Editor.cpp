//
// Created by borja on 28/07/22.
//

#include "Editor.h"
#include "core/graph/Scene.h"

#if !IS_MOBILE()
#include "imgui.h"

#endif

namespace Editor {

    void Editor::redirectRendering(FrameBuffer* _frameBuffer) {
        #if !IS_MOBILE()
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
            ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            auto viewportSize = ImVec2 { viewportPanelSize.x, viewportPanelSize.y };

            uint32_t textureID = _frameBuffer->getColorAttachmentRendererID();
            ImGui::Image((void*)(intptr_t)textureID, ImVec2{ 1280, 720 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            ImGui::End();
            ImGui::PopStyleVar();
        #endif
    }

    static void test(PhysicsBody* _a, PhysicsBody* _b) {
//        LOG_I("It fucking worked first try")
    }

    static void testExit(PhysicsBody* _a, PhysicsBody* _b) {
//        LOG_I("It fucking worked first try exit")
    }

    static void testStay(PhysicsBody* _a, PhysicsBody* _b) {
//        LOG_I("It fucking worked first try stay")
    }

    void Editor::onCollisionEnter(PhysicsBody* _a, PhysicsBody* _b) {
        auto* _box = _a->transform->node->getID() == circleNode->getID() ? _b : _a;
        getMainGraph()->setParent(_box->transform->node, circleNode);
        collisionHappened = true;
    }

    void Editor::onInit() {
        localizationTest();
        textStressTest(10);

        {
            auto _floorNode = getMainGraph()->createNode("Floor");
            _floorNode->getTransform()->setPosition(0, -300);
            ShapeConfig _floorShapeConfig{
                    .type = PhysicsShapeType::SEGMENT,
                    .size = {1280, 0},
                    .vertices = {},
            };

            _floorShapeConfig.shapeMaskingConfig.mask = 2;
            _floorShapeConfig.shapeMaskingConfig.toCollideWith = 1 | 5;
            BodyConfig _floorBodyConfig{
                    .shapeConfig = _floorShapeConfig
            };
            _floorBodyConfig.physicsBodyType = GDE::STATIC;
            _floorNode->addComponent<PhysicsBody>(_floorBodyConfig);
        }

        {
            circleNode = getMainGraph()->createNode("Circle");
            circleNode->getTransform()->setPosition(0, 300);
            ShapeConfig _floorShapeConfig{
                    .type = PhysicsShapeType::CIRCLE,
                    .size = {64, 64},
                    .vertices = {},
            };
            _floorShapeConfig.restitution = 0;

            _floorShapeConfig.shapeMaskingConfig.mask = 1;
            _floorShapeConfig.shapeMaskingConfig.toCollideWith = 1;
            BodyConfig _floorBodyConfig{
                    .shapeConfig = _floorShapeConfig
            };
            _floorBodyConfig.physicsBodyType = GDE::STATIC;
            circleNode->addComponent<PhysicsBody>(_floorBodyConfig);

//            getMainGraph()->addComponent<SpriteRenderer>(circleNodeID, this, engine->manager.textureManager.getSubTexture("assets", "buttonDark"));
        }

        PhysicsCollisionCallbacks _callbacks;
        _callbacks.onCollisionEnter.bind<&Editor::onCollisionEnter>(this);
        _callbacks.onCollisionExit.bind<testExit>();
        _callbacks.onCollisionStay.bind<testStay>();
        engine->manager.physics.addCollisionCallbacks(1, 1, _callbacks);

//        particleSystemTest();

//        UDelegate<void(FrameBuffer*)> _delegate;
//        _delegate.bind<&Editor::redirectRendering>(this);
//
//        physicsTest();
//
//        auto _uiTest = getCanvases()[0]->getGraph()->createNode("TestUINode");
//        auto* _transform = getCanvases()[0]->getGraph()->getComponent<Transform>(_uiTest);
//        _transform->setPosition(-250, 0);
//        auto* ui = getCanvases()[0]->getGraph()->addComponent<NineSliceSprite>(_uiTest, this, getCanvases()[0],
//                                                                                       engine->manager.textureManager.getSubTexture(
//                                                                                               "ui", "block1"));
//        ui->nineSliceSize = {400, 225};
//        ui->interaction->onClick.bind<&Editor::onMouseClick>(this);
//        ui->interaction->onMouseEntered.bind<&Editor::onMouseEntered>(this);
//        ui->interaction->onMouseExited.bind<&Editor::onMouseExited>(this);
//
//

        auto _panelNode = getCanvases()[0]->getGraph()->createNode("Panel");
        auto* _panel = _panelNode->addComponent<UIPanel>(UIPanelConfig {  });

        auto _uiButtonNode = getCanvases()[0]->getGraph()->createNode("UIButton");
        auto* _uiButton = _uiButtonNode->addComponent<UIButton>(UIButtonConfig { .text = "Button" });
        _uiButtonNode->getTransform()->setPosition(-140, 0);
//        _uiButton->setInteractable(true);
//        getCanvases()[0]->getGraph()->setNodeActive(_uiButtonId, false);

        auto _checkboxNode = getCanvases()[0]->getGraph()->createNode("Checkbox");
        auto* _checkbox = _checkboxNode->addComponent<UICheckbox>(UICheckboxConfig {  });
        _checkboxNode->getTransform()->setPosition(0, 64);
//        _checkbox->setInteractable(false);

        auto _uiButtonNode2 = getCanvases()[0]->getGraph()->createNode("UIButton2");
        auto* _uiButton2 = _uiButtonNode2->addComponent<UIButton>(UIButtonConfig { .text = "Button2" });
        _uiButtonNode2->getTransform()->setPosition(140, 0);
//        _uiButton2->setInteractable(false);


        auto _inputTextNode = getCanvases()[0]->getGraph()->createNode("InputText");
        auto* _input = _inputTextNode->addComponent<UIInput>(UIInputConfig {  });
        _inputTextNode->getTransform()->setPosition(0, 128);

        auto _sliderNode = getCanvases()[0]->getGraph()->createNode("Slider");
        slider = _sliderNode->addComponent<UISlider>(UISliderConfig {  });
        _sliderNode->getTransform()->setPosition(0, 230);
    }

    void Editor::onUpdate(Delta _dt) {
        Scene::onUpdate(_dt);

        if(engine->manager.inputManager.isKeyPressed(KeyCode::Left)) {
            slider->fillBarSprite->setSize({slider->fillBarSprite->getSize().x - 1, slider->fillBarSprite->getSize().y});
        }

        if(engine->manager.inputManager.isKeyPressed(KeyCode::Right)) {
            slider->fillBarSprite->setSize({slider->fillBarSprite->getSize().x + 1, slider->fillBarSprite->getSize().y});
        }

        LOG_I(slider->fillBarSprite->getSize())

        static float _timer = 0.f;
        static int _fpsCounter = 0;
        static int _frameCounter = 0;
        if (_timer >= 1.f) {
            _fpsCounter = _frameCounter;
            engine->getWindow().setTitle("Engine: " + std::to_string(_fpsCounter));
            _frameCounter = 0;
            _timer = 0;
        }
        ++_frameCounter;
        _timer += _dt;
//
        circleNode->getTransform()->rotate(250.f * _dt);
//
//        if(engine->manager.inputManager.isKeyJustPressed(KeyCode::K)) {
//            getMainGraph()->setParent(getMainGraph()->getNode("RightWall"), getMainGraph()->getNode("Text"));
//        }

        if(engine->manager.inputManager.isKeyJustPressed(KeyCode::K)) {
            static int _knifeCounter = 0;
            auto _knifeNode = getMainGraph()->createNode(APPEND_S("Knife", _knifeCounter++));
            _knifeNode->getTransform()->setPosition(0, -200);
            ShapeConfig _floorShapeConfig{
                    .type = PhysicsShapeType::BOX,
                    .size = {16, 64},
                    .vertices = {},
            };
            _floorShapeConfig.restitution = 0;

            _floorShapeConfig.shapeMaskingConfig.mask = 1;
            _floorShapeConfig.shapeMaskingConfig.toCollideWith = 1 | 2 | 5;
            BodyConfig _floorBodyConfig{
                    .shapeConfig = _floorShapeConfig
            };
            _floorBodyConfig.physicsBodyType = GDE::DYNAMIC;
            body = _knifeNode->addComponent<PhysicsBody>(_floorBodyConfig);

            body->applyImpulseLocal({0, 2500}, {0, 0});
        }
    }

    void Editor::onLateUpdate(Delta _dt) {
        Scene::onLateUpdate(_dt);

        if(collisionHappened) {
//            body->setBodyType(PhysicsBodyType::STATIC);
            body->setMask(0, 5);
            collisionHappened = false;
        }
    }

    void Editor::textStressTest(int _amount) {
            GDE::Random _r;
            auto _texture = engine->manager.textureManager.getSubTexture("square", "whiteSquare");
            auto _parentNode = getMainGraph()->createNode("parent");

            for(int _i = 0; _i < _amount; _i++) {
                auto _textNode = getMainGraph()->createNode("Block" + std::to_string(_i));
                nodes.push_back(_textNode->getID());
                _textNode->getTransform()->setPosition(_r.randomf(-engine->getWindow().getWidth() / 2.f, engine->getWindow().getWidth() / 2.f), _r.randomf(-engine->getWindow().getHeight() / 2.f, engine->getWindow().getHeight() / 2.f));
                _textNode->getTransform()->setRotation(45);
                auto* _sprite = _textNode->addComponent<SpriteRenderer>(_texture);
                getMainGraph()->setParent(_textNode, _parentNode);
            }
        }

    void Editor::onDebugRender(Delta _dt) {
        Scene::onDebugRender(_dt);
//        auto* _transformRightWall = getMainGraph()->getComponent<Transform>(getMainGraph()->getNode("RightWall"));
//        engine->manager.renderManager.drawSquare(_transformRightWall->getModelMatrixPosition(), {8, 8});
    }

    void Editor::physicsTest() {
//        NodeID _colliderParentId;
//
//        {
//            auto _floorId = getMainGraph()->createNode("Floor");
//            _colliderParentId = _floorId;
//            auto* _transform0 = getMainGraph()->getComponent<Transform>(_floorId);
//            _transform0->setPosition(0, -100);
//            ShapeConfig _floorShapeConfig{
//                    .type = PhysicsShape::BOX,
//                    .size = {500, 5},
//                    .vertices = {},
//            };
//            BodyConfig _floorBodyConfig{
//                    .shapeConfig = _floorShapeConfig,
//                    .isStatic = true
//            };
//            auto* _floorBody = getMainGraph()->addComponent<PhysicsBody>(_floorId, this, _floorBodyConfig);
//        }
//
//        {
//            auto _leftWallId = getMainGraph()->createNode("LeftWall", _colliderParentId);
//            auto* _transform1 = getMainGraph()->getComponent<Transform>(_leftWallId);
//            _transform1->setPosition(-300, 0);
//            ShapeConfig _leftWallShapeConfig {
//                    .type = PhysicsShape::BOX,
//                    .size = { 5, 500 },
//                    .vertices = {},
//            };
//            BodyConfig _leftWallBodyConfig {
//                    .shapeConfig = _leftWallShapeConfig,
//                    .isStatic = true
//            };
//            auto* _leftWallBody = getMainGraph()->addComponent<PhysicsBody>(_leftWallId, this, _leftWallBodyConfig);
//        }
//
//        {
//            auto _rightWallId = getMainGraph()->createNode("RightWall");
//            auto* _transform2 = getMainGraph()->getComponent<Transform>(_rightWallId);
//            _transform2->setScale(0.25f, 0.25f);
//            _transform2->setPosition(300, 0);
//            ShapeConfig _rightWallShapeConfig {
//                    .type = PhysicsShape::BOX,
//                    .size = { 5, 500 },
//                    .vertices = {},
//            };
//            BodyConfig _rightWallBodyConfig {
//                    .shapeConfig = _rightWallShapeConfig,
//                    .isStatic = true
//            };
//            auto* _rightWallBody = getMainGraph()->addComponent<PhysicsBody>(_rightWallId, this, _rightWallBodyConfig);
//            auto _texture = engine->manager.textureManager.getSubTexture("square", "whiteSquare");
//            getMainGraph()->addComponent<SpriteRenderer>(_rightWallId, this, _texture);
//        }

//        {
//            static int _counter = 0;
//            Random _random;
//
//            auto _circleId = getMainGraph()->createNode(APPEND_S("Circle", _counter));
//            auto* _transform = getMainGraph()->getComponent<Transform>(_circleId);
//            _transform->setPosition(engine->manager.inputManager.getMousePosScreenCoords().x, engine->manager.inputManager.getMousePosScreenCoords().y);
//
//            ShapeConfig _shapeConfig {
//                    .type = PhysicsShape::CIRCLE,
//                    .size = { _random.randomf(5, 20), 0 },
//                    .vertices = {}
//            };
//
//            BodyConfig _bodyConfig {
//                    .shapeConfig = _shapeConfig,
//                    .isStatic = false
//            };
//
//            auto* _circleBody = getMainGraph()->addComponent<PhysicsBody>(_circleId, this, _bodyConfig);
//            _circleBody->restitution = 0.2f;
//            _circleBody->dynamicFriction = 0.2f;
//            _circleBody->staticFriction = 0.4f;
//            _counter++;
//        }
//
//        {
//            static int _counter = 0;
//            Random _random;
//            auto count = _random.randomi(3, 10);
//            std::vector<Vec2F> vertices;
//            float e = _random.randomf( 16, 32 );
//            for(auto _i = 0; _i < count; _i++)
//                vertices.emplace_back( _random.randomf( -e, e ), _random.randomf( -e, e ) );
//
//            ShapeConfig _shapeConfig {
//                    .type = PhysicsShape::POLYGON,
//                    .size = {  },
//                    .vertices = vertices,
//            };
//
//            BodyConfig _bodyConfig {
//                    .shapeConfig = _shapeConfig,
//                    .isStatic = false
//            };
//
//            auto _polyId = getMainGraph()->createNode(APPEND_S("Poly", _counter));
//            auto* _transform = getMainGraph()->getComponent<Transform>(_polyId);
//            _transform->setPosition(engine->manager.inputManager.getMousePosScreenCoords().x, engine->manager.inputManager.getMousePosScreenCoords().y);
//            auto* _polyBody = getMainGraph()->addComponent<PhysicsBody>(_polyId, this, _bodyConfig);
//            _polyBody->rotate(_random.randomf(-180, 180));
//            _polyBody->restitution = 0.2f;
//            _polyBody->dynamicFriction = 0.2f;
//            _polyBody->staticFriction = 0.4f;
//
//            _counter++;
//        }

//        engine->manager.physics.addOrGetCollisionToTable(1, 2)->onCollisionEnter.bind<&Editor::collisionA>(this);
//        engine->manager.physics.addOrGetCollisionToTable(2, 2)->onCollisionEnter.bind<&Editor::collisionB>(this);

    }

    void Editor::particleSystemTest() {
        auto _particleSystemNode = getMainGraph()->createNode("ParticleSystem");

        ParticleSystemConfig _particleSystemConfig {
                ParticleSystemColorGradientConfig {
                        Color {17, 79, 15, 255},
                        Color {223, 216, 88, 50}
                },
                ParticleSystemCallbacksConfig {

                },
                ParticleSystemDataConfig {

                }
        };

        _particleSystemConfig.dataConfig.numberOfParticles = 500;
        _particleSystemConfig.dataConfig.lifeTime = 3.f;
        _particleSystemConfig.dataConfig.timeToCreateNewParticleMs = 0.3f;
        _particleSystemNode->getTransform()->setPosition(100, 0);
        auto _particleSystem = _particleSystemNode->addComponent<ParticleSystem>(_particleSystemConfig);
        _particleSystem->setLayer(5);
        _particleSystem->play();
    }

    void Editor::onMouseClick(MouseCode _mouseCode) {
        LOG_I("I'm inside the UI sprite!!")
    }

    void Editor::onMouseEntered() {
        LOG_I("Entered!")
    }

    void Editor::onMouseExited() {
        LOG_I("Exited!")
    }

    void Editor::collisionB(PhysicsBody* _a, PhysicsBody* _b) {

    }

    void Editor::localizationTest() {
        engine->manager.localizationManager.loadLanguage(LocalizationInfo::PT_BR);
        engine->manager.localizationManager.localizationInfo.language = LocalizationInfo::PT_BR;
        auto _a = 1;
        auto _b = "MyString";
        auto _c = 3.14f;
        auto _d = 1L;
        LOG_W(engine->manager.localizationManager.localize("scoreKey",_a, _b, _c, _d))
    }


}
