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

        auto* poly = new PhysicsShape;
        Random _random;
        auto count = _random.randomi( 3, MAX_VERTICES_PER_POLYGON );
        std::vector<Vec2F> vertices;
        real e = _random.randomf( 5, 10 );
        for(uint32 i = 0; i < count; ++i)
            vertices.emplace_back(Vec2F { _random.randomf( -e, e ), _random.randomf( -e, e ) });
        poly->makePolygon(vertices);
        PhysicsBody *b = engine->manager.physics.add( poly, { 0, 0 });
        b->setOrient( random( -PI, PI ) );
        b->restitution = 0.2f;
        b->dynamicFriction = 0.2f;
        b->staticFriction = 0.4f;

        auto* _ground = new PhysicsShape;
        _ground->makeRectangle({100.0f, 5.0f});
        auto* _groundBody = engine->manager.physics.add( _ground, {0, -50} );
        _groundBody->setStatic();
        _groundBody->setOrient(0);

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

//        _particleSystemConfig.dataConfig.texture = engine->manager.textureManager.getSubTexture("square", "whiteSquare");
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

        }
    }

    void Editor::textStressTest() {
            Random _r;
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
