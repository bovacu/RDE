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

    NodeID colliderParent;

    void Editor::onInit() {
        UDelegate<void(FrameBuffer*)> _delegate;
        _delegate.bind<&Editor::redirectRendering>(this);
        // engine->setRenderingRedirectionToImGui(_delegate);
        textStressTest();
        auto _particleSystemNode = getMainGraph()->createNode("ParticleSystem");

        auto _colliderParentId = getMainGraph()->createNode("colliderParent");
        transforms.push_back(getMainGraph()->getComponent<Transform>(_colliderParentId));
        colliderParent = _colliderParentId;

        {
            auto _floorId = getMainGraph()->createNode("Floor", _colliderParentId);
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
            auto* _floorBody = getMainGraph()->addComponent<PhysicsBody>(_floorId, _transform0, _floorBodyConfig);
            engine->manager.physics.add(_floorBody);
            transforms.push_back(_transform0);
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
            auto* _leftWallBody = getMainGraph()->addComponent<PhysicsBody>(_leftWallId, _transform1, _leftWallBodyConfig);
            engine->manager.physics.add(_leftWallBody);
            transforms.push_back(_transform1);
        }

        {
            auto _rightWallId = getMainGraph()->createNode("RightWall", _colliderParentId);
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
            auto* _rightWallBody = getMainGraph()->addComponent<PhysicsBody>(_rightWallId, _transform2, _rightWallBodyConfig);
            engine->manager.physics.add(_rightWallBody);
        }

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

        if(engine->manager.inputManager.isMouseJustPressed(MouseCode::ButtonLeft)) {
            static int _counter = 0;
            Random _random;
            auto count = _random.randomi(3, 10);
            std::vector<Vec2F> vertices;
            float e = _random.randomf( 16, 32 );
            for(auto _i = 0; _i < count; _i++)
                vertices.emplace_back( _random.randomf( -e, e ), _random.randomf( -e, e ) );

            ShapeConfig _shapeConfig {
                .type = PhysicsShape::BOX,
                .size = { 32, 32 },
//                .vertices = vertices,
            };

            BodyConfig _bodyConfig {
                .shapeConfig = _shapeConfig,
                .isStatic = false
            };

            auto _polyId = getMainGraph()->createNode(APPEND_S("Poly", _counter), colliderParent);
            auto* _transform = getMainGraph()->getComponent<Transform>(_polyId);
            transforms.push_back(_transform);
            _transform->setPosition(engine->manager.inputManager.getMousePosScreenCoords().x, engine->manager.inputManager.getMousePosScreenCoords().y);
            auto* _polyBody = getMainGraph()->addComponent<PhysicsBody>(_polyId, _transform, _bodyConfig);
            _polyBody->rotate(_random.randomf(-180, 180));
            _polyBody->restitution = 0.2f;
            _polyBody->dynamicFriction = 0.2f;
            _polyBody->staticFriction = 0.4f;

            engine->manager.physics.add(_polyBody);

            _counter++;

        } else if(engine->manager.inputManager.isMouseJustPressed(MouseCode::ButtonRight)) {
            static int _counter = 0;
            Random _random;

            auto _circleId = getMainGraph()->createNode(APPEND_S("Circle", _counter));
            auto* _transform = getMainGraph()->getComponent<Transform>(_circleId);
            _transform->setPosition(engine->manager.inputManager.getMousePosScreenCoords().x, engine->manager.inputManager.getMousePosScreenCoords().y);

            ShapeConfig _shapeConfig {
                .type = PhysicsShape::CIRCLE,
                .size = { _random.randomf(5, 20), 0 },
                .vertices = {}
            };

            BodyConfig _bodyConfig {
                .shapeConfig = _shapeConfig,
                .isStatic = false
            };

            auto* _circleBody = getMainGraph()->addComponent<PhysicsBody>(_circleId, _transform, _bodyConfig);
            _circleBody->restitution = 0.2f;
            _circleBody->dynamicFriction = 0.2f;
            _circleBody->staticFriction = 0.4f;

            engine->manager.physics.add(_circleBody);
            _counter++;
        } else if(engine->manager.inputManager.isMouseJustPressed(MouseCode::ButtonMiddle)) {
            static int _counter = 0;
            auto _spriteId = getMainGraph()->createNode(APPEND_S("Sprite", _counter));
            auto* _transform = getMainGraph()->getComponent<Transform>(_spriteId);
            _transform->setPosition(engine->manager.inputManager.getMousePosScreenCoords().x, engine->manager.inputManager.getMousePosScreenCoords().y);
            getMainGraph()->addComponent<SpriteRenderer>(_spriteId, _transform, this, engine->manager.textureManager.getSubTexture("square", "whiteSquare"));
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
                getMainGraph()->addComponent<SpriteRenderer>(_text, _textTransform, this, _texture);
            }
        }

    void Editor::onDebugRender(Delta _dt) {
        Scene::onDebugRender(_dt);

        for(auto* _transform : transforms) {
            engine->manager.renderManager.drawSquare(_transform->getPositionLocal(), {8, 8}, Color::White, _transform->getRotationLocal());
        }

        engine->manager.renderManager.drawLine({64, 64}, {120, 180});
    }

}
