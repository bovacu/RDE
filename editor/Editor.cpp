//
// Created by borja on 28/07/22.
//

#include "Editor.h"
#include "core/graph/Scene.h"

#if !IS_MOBILE()
#include "imgui.h"
#include "core/graph/components/UIButton.h"

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

    void Editor::onInit() {
        UDelegate<void(FrameBuffer*)> _delegate;
        _delegate.bind<&Editor::redirectRendering>(this);

        physicsTest();

        auto _uiTest = getCanvases()[0]->getGraph()->createNode("TestUINode");
        auto* _transform = getCanvases()[0]->getGraph()->getComponent<Transform>(_uiTest);
        _transform->setPosition(-250, 0);
        auto* ui = getCanvases()[0]->getGraph()->addComponent<NineSliceSprite>(_uiTest, this, getCanvases()[0],
                                                                                       engine->manager.textureManager.getSubTexture(
                                                                                               "ui", "block1"));
        ui->nineSliceSize = {400, 225};
        ui->interaction->interactionTrigger.bind<&Editor::uiButtonTrigger>(this);
        ui->interaction->onClick.bind<&Editor::onMouseClick>(this);
        ui->interaction->onMouseEntered.bind<&Editor::onMouseEntered>(this);
        ui->interaction->onMouseExited.bind<&Editor::onMouseExited>(this);


        auto _uiButtonId = getCanvases()[0]->getGraph()->createNode("UIButton");
        auto* _uiButton = getCanvases()[0]->getGraph()->addComponent<UIButton>(_uiButtonId, this, getCanvases()[0], engine->manager.textureManager.getSubTexture(
                "ui", "button"), engine->manager.fontManager.getDefaultFont("arial"), "Button");
        _uiButton->textRenderer->color = Color::Black;
    }

    void Editor::onUpdate(Delta _dt) {
        Scene::onUpdate(_dt);

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

        auto* _buttonTransform = getMainGraph()->getComponent<Transform>(getMainGraph()->getNode("button"));
//        _buttonTransform->rotate(50 * _dt);

        if(engine->manager.inputManager.isKeyJustPressed(KeyCode::K)) {
            getMainGraph()->setParent(getMainGraph()->getNode("RightWall"), getMainGraph()->getNode("Text"));
        }
    }

    void Editor::textStressTest() {
            GDE::Random _r;
            auto _texture = engine->manager.textureManager.getSubTexture("square", "whiteSquare");
            // 40000 is the maximum I could get with 55fps of average performance, with texts -> "Text[0-40000]" ~350000 images
            auto _parent = getMainGraph()->createNode("parent");

            for(int _i = 0; _i < 10; _i++) {
                auto _text = getMainGraph()->createNode("Block" + std::to_string(_i), _parent);
                nodes.push_back(_text);
                auto* _textTransform = getMainGraph()->getComponent<Transform>(_text);
                _textTransform->staticTransform = true;
                _textTransform->setPosition(_r.randomf(-(float)engine->getWindow().getWindowSize().x / 2.f, (float)engine->getWindow().getWindowSize().x / 2.f),
                                            _r.randomf(-(float)engine->getWindow().getWindowSize().y / 2.f, (float)engine->getWindow().getWindowSize().y / 2.f));
                getMainGraph()->addComponent<SpriteRenderer>(_text, this, _texture);
            }
        }

    void Editor::onDebugRender(Delta _dt) {
        Scene::onDebugRender(_dt);
        auto* _transformRightWall = getMainGraph()->getComponent<Transform>(getMainGraph()->getNode("RightWall"));
        engine->manager.renderManager.drawSquare(_transformRightWall->getModelMatrixPosition(), {8, 8});
    }

    void Editor::physicsTest() {
        NodeID _colliderParentId;

        {
            auto _floorId = getMainGraph()->createNode("Floor");
            _colliderParentId = _floorId;
            auto* _transform0 = getMainGraph()->getComponent<Transform>(_floorId);
            _transform0->setPosition(0, -100);
            ShapeConfig _floorShapeConfig{
                    .type = PhysicsShape::BOX,
                    .size = {500, 5},
                    .vertices = {},
            };
            BodyConfig _floorBodyConfig{
                    .shapeConfig = _floorShapeConfig,
                    .isStatic = true
            };
            auto* _floorBody = getMainGraph()->addComponent<PhysicsBody>(_floorId, this, _floorBodyConfig);
        }

        {
            auto _leftWallId = getMainGraph()->createNode("LeftWall", _colliderParentId);
            auto* _transform1 = getMainGraph()->getComponent<Transform>(_leftWallId);
            _transform1->setPosition(-300, 0);
            ShapeConfig _leftWallShapeConfig {
                    .type = PhysicsShape::BOX,
                    .size = { 5, 500 },
                    .vertices = {},
            };
            BodyConfig _leftWallBodyConfig {
                    .shapeConfig = _leftWallShapeConfig,
                    .isStatic = true
            };
            auto* _leftWallBody = getMainGraph()->addComponent<PhysicsBody>(_leftWallId, this, _leftWallBodyConfig);
        }

        {
            auto _rightWallId = getMainGraph()->createNode("RightWall");
            auto* _transform2 = getMainGraph()->getComponent<Transform>(_rightWallId);
            _transform2->setPosition(300, 0);
            ShapeConfig _rightWallShapeConfig {
                    .type = PhysicsShape::BOX,
                    .size = { 5, 500 },
                    .vertices = {},
            };
            BodyConfig _rightWallBodyConfig {
                    .shapeConfig = _rightWallShapeConfig,
                    .isStatic = true
            };
            auto* _rightWallBody = getMainGraph()->addComponent<PhysicsBody>(_rightWallId, this, _rightWallBodyConfig);
            auto _texture = engine->manager.textureManager.getSubTexture("square", "whiteSquare");
            getMainGraph()->addComponent<SpriteRenderer>(_rightWallId, this, _texture);
        }

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

        engine->manager.physics.addOrGetCollisionToTable(1, 2)->onCollisionEnter.bind<&Editor::collisionA>(this);
        engine->manager.physics.addOrGetCollisionToTable(2, 2)->onCollisionEnter.bind<&Editor::collisionB>(this);

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
        auto _particleSystemTransform = getMainGraph()->getComponent<Transform>(_particleSystemNode);
        _particleSystemTransform->setPosition(100, 0);
        auto _particleSystem = getMainGraph()->addComponent<ParticleSystem>(_particleSystemNode, _particleSystemTransform, this, _particleSystemConfig);
        _particleSystem->play();
    }

    bool Editor::uiButtonTrigger(NodeID _nodeId, Canvas* _canvas) {
        auto* _ninePatch = _canvas->getGraph()->getComponent<NineSliceSprite>(_nodeId);
        auto* _transform = _canvas->getGraph()->getComponent<Transform>(_nodeId);
        auto _mousePos = engine->manager.inputManager.getMousePosWorldPos();

        return _mousePos.isInside(_transform->getPosition(), Vec2F {(float)_ninePatch->nineSliceSize.x, (float)_ninePatch->nineSliceSize.y});
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

    void Editor::collisionA(PhysicsBody* _a, PhysicsBody* _b) {

    }

    void Editor::collisionB(PhysicsBody* _a, PhysicsBody* _b) {

    }

}
