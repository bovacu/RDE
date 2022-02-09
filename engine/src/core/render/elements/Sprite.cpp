// Created by borja on 27/12/21.


#include "core/render/elements/Sprite.h"

namespace engine {

    void Sprite::init() {
        renderer.shaderID = ShaderManager::get().getShader("basic");
    }

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

    void Sprite::setColor(const Color& _color) {
        renderer.color = _color;
    }

    void Sprite::setTexture(Texture* _texture) {
        renderer.texture = _texture;
    }

    Color Sprite::getColor() {
        return renderer.color;
    }

    void Sprite::flipX(bool _flip) {
        setScale({getScale().x * (_flip ? -1.f : 1.f), getScale().y});
    }

    void Sprite::flipY(bool _flip) {
        setScale({getScale().x, getScale().y * (_flip ? -1.f : 1.f)});
    }

    bool Sprite::isXFlipped() const {
        return getScale().x < 0;
    }

    bool Sprite::isYFlipped() const {
        return getScale().y < 0;
    }

    void Sprite::setLayer(int _layer) {
        renderer.layer = _layer;
    }

    int Sprite::getLayer() const {
        return renderer.layer;
    }

    ShaderID Sprite::getShaderID() {
        return renderer.shaderID;
    }

    Texture* Sprite::getTexture() {
        return renderer.texture;
    }

    void Sprite::setShader(ShaderID _shaderID) {
        renderer.shaderID = _shaderID;
    }
}