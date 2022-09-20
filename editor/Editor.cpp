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

    void Editor::onInit() {
        UDelegate<void(FrameBuffer*)> _delegate;
        _delegate.bind<&Editor::redirectRendering>(this);
        // engine->setRenderingRedirectionToImGui(_delegate);
        textStressTest();
        auto _particleSystemNode = getMainGraph()->createNode("ParticleSystem");

        auto* _ground = new PhysicsShape;
        _ground->makeRectangle({500.0f, 5.0f});
        auto* _transform0 = getMainGraph()->getComponent<Transform>(getMainGraph()->createNode("Floor"));
        _ground->transform = _transform0;
        auto* _groundBody = engine->manager.physics.add( _ground, {0, -100} );
        _groundBody->setStatic();
        _groundBody->rotate(0);

        auto* _leftWall = new PhysicsShape;
        _leftWall->makeRectangle({5.0f, 250.0f});
        auto* _transform1 = getMainGraph()->getComponent<Transform>(getMainGraph()->createNode("LeftWall"));
        _leftWall->transform = _transform1;
        auto* _leftWallBody = engine->manager.physics.add( _leftWall, {-300, 0} );
        _leftWallBody->setStatic();
        _leftWallBody->rotate(0);

        auto* _rightWall = new PhysicsShape;
        _rightWall->makeRectangle({5.0f, 250.0f});
        auto* _transform2 = getMainGraph()->getComponent<Transform>(getMainGraph()->createNode("RightWall"));
        _rightWall->transform = _transform2;
        auto* _rightWallBody = engine->manager.physics.add( _rightWall, {300, 0} );
        _rightWallBody->setStatic();
        _rightWallBody->rotate(0);

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
        _particleSystemTransform->setPositionWorld(100, 0);
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
            auto* poly = new PhysicsShape;
            Random _random;
            auto count = _random.randomi( 3, 10 );
            std::vector<Vec2F> vertices;
            float e = _random.randomf( 16, 32 );
            for(auto _i = 0; _i < count; _i++)
                vertices.emplace_back( _random.randomf( -e, e ), _random.randomf( -e, e ) );
            poly->makePolygon( vertices );

            auto* _transform = getMainGraph()->getComponent<Transform>(getMainGraph()->createNode(APPEND_S("Poly", _counter)));
            poly->transform = _transform;
            _counter++;

            auto* b = engine->manager.physics.add(poly, {engine->manager.inputManager.getMousePosScreenCoords().x, engine->manager.inputManager.getMousePosScreenCoords().y} );
            b->rotate(_random.randomf(-180, 180));
            b->restitution = 0.2f;
            b->dynamicFriction = 0.2f;
            b->staticFriction = 0.4f;
        } else if(engine->manager.inputManager.isMouseJustPressed(MouseCode::ButtonRight)) {
            static int _counter = 0;
            Random _random;
            auto* _circle = new PhysicsShape;
            _circle->makeCircle(_random.randomf(5, 20));

            auto* _transform = getMainGraph()->getComponent<Transform>(getMainGraph()->createNode(APPEND_S("Circle", _counter)));
            _circle->transform = _transform;
            _counter++;

            auto* _circleBody = engine->manager.physics.add(_circle, {engine->manager.inputManager.getMousePosScreenCoords().x, engine->manager.inputManager.getMousePosScreenCoords().y} );
            _circleBody->restitution = 0.2f;
            _circleBody->dynamicFriction = 0.2f;
            _circleBody->staticFriction = 0.4f;
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
    }

}
