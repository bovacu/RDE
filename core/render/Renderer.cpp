// Created by borja on 26/12/21.


#include "Renderer.h"

namespace engine {

    SpriteBatch Renderer::batch;

    void Renderer::init(const Vec2I& _screenSize) {
        batch.init();
        batch.setScreenSize({_screenSize.x, _screenSize.y});
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable( GL_BLEND );
    }

    void Renderer::beginDraw(const Color& _color) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor((float)_color.r / 255.f, (float)_color.g / 255.f, (float)_color.b / 255.f, (float)_color.a / 255.f);
    }

    void Renderer::draw(Texture* _texture, const Vec2I& _position, const Vec2I& _size, const Color& _tintColor) {
        batch.draw(
                glm::vec4(_position.x, _position.y, _texture->getSize().x, _texture->getSize().y),
                glm::vec4(0, 0, _size.x, _size.y),
                glm::vec4((float)_tintColor.r / 255.f, (float)_tintColor.g / 255.f, (float)_tintColor.b / 255.f, (float)_tintColor.a / 255.f),
                _texture
                );
    }

    void Renderer::endDraw() {
        batch.flush();
    }

}