// Created by borja on 22/1/22.

#include "Sandbox.h"
#include "core/Engine.h"
#include "core/systems/soundSystem/SoundManager.h"
#include "core/procedural/CellularAutomataMapGenerator.h"
#include "core/systems/fileSystem/FilesSystem.h"

//#if IS_ANDROID()
//#include <nativeCode/include/NativeAds.h>
//#endif

namespace engine {


    void Sandbox::onInit() {
        engine = &Engine::get();
        auto* _font = FontManager::get().loadFont("assets/fonts/arial.ttf", 54);
        
        TextureAtlasManager::get().addAtlas(50, 50, "assets/test/test.png");
        TextureAtlasManager::get().addAtlas(120, 80, "assets/player/run.png");

        animationSystem.createAnimation("run", "run", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
        animationSystem.setInitialAnimation("run");

        animationSystem.createAnimation("roll", "run", {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23});
        animationSystem.createTransition<&Sandbox::run_roll>("run", "roll", this);
        animationSystem.createTransition<&Sandbox::roll_run>("roll", "run", this);

        animationSystem.setInitialAnimation("run");

        player.init();
        player.setPosition({95, 0});
        player.setLayer(10);

        animationSystem.start();

        text.init(_font, "Hello World");
        text.setPosition({-350, 250});
        text.setTextColor(Color::Green);

        int _line = 0;
        int _row = 0;
        for(int _i = 0; _i < 36 ; _i++) {
            if(_i != 0 && _i % 6 == 0) {
                _line++;
                _row = 0;
            }

            Sprite _s;
            _s.init();
            int _index = _i > 35 ? 35 : _i;
            _s.setTexture(TextureAtlasManager::get().getTile("test", "test_" + std::to_string(_index)));
            _s.setPosition({60.f + (float)52 * _row, (float)(200 - _line * 52)});
            _s.setLayer(1);
            sprites.push_back(_s);

            _row++;
        }
//        auto& _music = SoundManager::get().loadMusic("assets/sounds/getout.ogg");
//        SoundManager::get().playMusic(_music.name);
//        #if IS_ANDROID()
//            NativeAds::get().loadAd(AdType::BANNER);
//        #endif

        mseDelegate.bind<&Sandbox::onMouseScrolled>(this);

    }

    void Sandbox::onEvent(Event& _event) {
        EventDispatcher _dispatcher(_event);
        _dispatcher.dispatchEvent<MouseScrolledEvent>(mseDelegate);
    }

    void Sandbox::onUpdate(Delta _dt) {
        if(InputManager::isKeyJustPressed(KeyCode::Enter)) {
            player.setShader(ShaderManager::get().getShader("outline"));
        }

        if(InputManager::isKeyJustPressed(KeyCode::Escape)) {
            player.setShader(ShaderManager::get().getShader("basic"));
        }

        animationSystem.update(_dt, player);

        if(InputManager::isGamepadButtonJustPressed(GamePadButtons::ButtonA))
            InputManager::gamepadVibrate();

        if(InputManager::isMobileScreenJustPressed(0)) {
//            #if IS_ANDROID()
//                NativeAds::get().showAd(AdType::BANNER);
//            #endif
        }
    }

    void Sandbox::onFixedUpdate(Delta _dt) {
        Layer::onFixedUpdate(_dt);
    }

    void Sandbox::onRender(Delta _dt) {
        Renderer::draw(player);
        for(auto& _sprite : sprites)
            Renderer::draw(_sprite);
        Renderer::draw(text);

        Renderer::drawSquare(player.getPosition(), {64, 96}, {255, 255, 0, 125});
    }

    void Sandbox::onImGuiRender(Delta _dt) {
        Layer::onImGuiRender(_dt);
    }

    void Sandbox::onEnd() {
        Layer::onEnd();
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