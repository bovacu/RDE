// Created by borja on 22/1/22.

#include "Sandbox.h"
#include "core/Engine.h"

namespace engine {


    void Sandbox::onInit() {
        engine = &Engine::get();

        auto* _font = FontManager::get().loadFont("assets/fonts/arial.ttf", 54);

        TextureAtlasManager::get().addAtlas(50, 50, "assets/test.png");
        TextureAtlasManager::get().addAtlas(120, 80, "assets/player/run.png");

        animationSystem.createAnimation("run", "run", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
        animationSystem.setInitialAnimation("run");

        // 12-23
        animationSystem.createAnimation("roll", "run", {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23});

        animationSystem.setInitialAnimation("run");

        player.init();
        player.setPosition({95, 0});
        player.setLayer(10);

        animationSystem.start();

        text.init(_font, "Hello World");
        text.setPosition({0, 0});
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
    }

    void Sandbox::onEvent(Event& _event) {

    }

    void Sandbox::onUpdate(Delta _dt) {
        if(InputManager::isKeyJustPressed(KeyCode::Enter))
            player.setShader(ShaderManager::get().getShader("outline"));

        animationSystem.update(_dt, player);
    }

    void Sandbox::onFixedUpdate(Delta _dt) {
        Layer::onFixedUpdate(_dt);
    }

    void Sandbox::onRender(Delta _dt) {
        Renderer::draw(player);
        for(auto& _sprite : sprites)
            Renderer::draw(_sprite);
        Renderer::draw(text);
    }

    void Sandbox::onImGuiRender(Delta _dt) {
        Layer::onImGuiRender(_dt);
    }

    void Sandbox::onEnd() {
        Layer::onEnd();
    }
}