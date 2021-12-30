// Created by borja on 27/12/21.


#include "Sprite.h"

namespace engine {

    void Sprite::setPosition(const Vec2F& _position) {
        transform.setPosition(_position);
    }

    void Sprite::setScale(const Vec2F& _scale) {
        transform.setScale(_scale);
    }

    void Sprite::setRotation(float _rotation) {
        transform.setRotation(_rotation);
    }

    Vec2F Sprite::getPosition() const {
        return transform.getPosition();
    }

    Vec2F Sprite::getScale() const {
        return transform.getScale();
    }

    float Sprite::getRotation() const {
        return transform.getRotation();
    }

    Color Sprite::getColor() const {
        return renderer.color;
    }

    Texture& Sprite::getTexture() const {
        return *renderer.texture;
    }

    void Sprite::setColor(const Color& _color) {
        renderer.color = _color;
    }

    void Sprite::setTexture(Texture* _texture) {
        renderer.texture = _texture;
    }

    Color Sprite::getColor() {
        return renderer.color;
    }
}