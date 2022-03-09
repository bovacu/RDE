// Created by borja on 22/1/22.

#include "Sandbox.h"
#include "core/Engine.h"
#include "core/procedural/CellularAutomataMapGenerator.h"

//#if IS_ANDROID()
//#include <nativeCode/include/NativeAds.h>
//#endif

namespace engine {


    void Sandbox::onInit() {
        engine = &Engine::get();
        auto* _font = FontManager::get().loadFont("assets/fonts/arial.ttf", 54);
        
        TextureAtlasManager::get().addAtlas(50, 50, "assets/test/test.png");
        TextureAtlasManager::get().addAtlas(120, 80, "assets/player/run.png");

        mainScene = new Scene("MainScene");
        auto _player = mainScene->createNode("player");
        auto _sprite = mainScene->addComponent<SpriteRenderer>(_player->id);
        _sprite->layer = 10;
        _sprite->texture = TextureAtlasManager::get().getTile("run", "run_0");
        mainScene->getComponent<Transform>(_player->id)->setPosition({95, 0});

        auto _animationSystem = mainScene->addComponent<AnimationSystem>(_player->id);

        _animationSystem->createAnimation("run", "run", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
        _animationSystem->setInitialAnimation("run");
        _animationSystem->createAnimation("roll", "run", {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23});
        _animationSystem->createTransition<&Sandbox::run_roll>("run", "roll", this);
        _animationSystem->createTransition<&Sandbox::roll_run>("roll", "run", this);
        _animationSystem->setInitialAnimation("run");
        _animationSystem->start();

        auto _text = mainScene->createNode("text");
        auto* _textComp = mainScene->addComponent<Text>(_text->id);
        _textComp->init(_font, "Hello World");
        mainScene->getComponent<Transform>(_text->id)->setPosition({-350, 250});
        _textComp->getRenderer().color = Color::Green;

        int _line = 0;
        int _row = 0;
        for(int _i = 0; _i < 36 ; _i++) {
            if(_i != 0 && _i % 6 == 0) {
                _line++;
                _row = 0;
            }

            int _index = _i > 35 ? 35 : _i;
            auto _s = mainScene->createNode("test_" + std::to_string(_index));
            auto _spriteRenderer = mainScene->addComponent<SpriteRenderer>(_s->id);
            _spriteRenderer->texture = TextureAtlasManager::get().getTile("test", "test_" + std::to_string(_index));
            _spriteRenderer->layer = 1;

            mainScene->getComponent<Transform>(_s->id)->setPosition({60.f + (float)52 * _row, (float)(200 - _line * 52)});

            _row++;
        }

        mseDelegate.bind<&Sandbox::onMouseScrolled>(this);
    }

    void Sandbox::onEvent(Event& _event) {
        EventDispatcher _dispatcher(_event);
        _dispatcher.dispatchEvent<MouseScrolledEvent>(mseDelegate);
    }

    void Sandbox::onUpdate(Delta _dt) {
        if(InputManager::isKeyJustPressed(KeyCode::Enter)) {
            mainScene->getComponent<SpriteRenderer>(mainScene->getNode("player")->id)->shaderID = ShaderManager::get().getShader("outline");
        }

        if(InputManager::isKeyJustPressed(KeyCode::Escape)) {
            mainScene->getComponent<SpriteRenderer>(mainScene->getNode("player")->id)->shaderID = ShaderManager::get().getShader("basic");
        }

        if(InputManager::isGamepadButtonJustPressed(GamePadButtons::ButtonA))
            InputManager::gamepadVibrate();

        if(InputManager::isMobileScreenJustPressed(0)) {
//            #if IS_ANDROID()
//                NativeAds::get().showAd(AdType::BANNER);
//            #endif
        }

        mainScene->onUpdate(_dt);
    }

    void Sandbox::onFixedUpdate(Delta _dt) {
        Layer::onFixedUpdate(_dt);
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