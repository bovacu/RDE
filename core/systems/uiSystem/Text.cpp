// Created by borja on 15/1/22.

#include "Text.h"

namespace engine {

    void Text::init(Font* _font, const std::string& _text) {
        font = _font;
        innerText = _text;
        recalcTextDimensions(_text);
        shaderID = ShaderManager::get().getShader("basicText");
        debugShape.setOutlineColor(Color::Green);
        debugShape.showInnerColor(false);
    }

    void Text::setText(const std::string& _text) {
        innerText = _text;
        recalcTextDimensions(_text);
    }

    void Text::setFont(Font* _font) {
        font = _font;
    }

    Font* Text::getFont() {
        return font;
    }

    std::string& Text::getText() {
        return innerText;
    }

    void Text::setPosition(const Vec2F& _position) {
        transform.setPosition(_position);
    }

    void Text::setRotation(float _rotation) {
        transform.setRotation(_rotation);
    }

    void Text::setScale(const Vec2F& _scale) {
        transform.setScale(_scale);
    }

    Vec2F Text::getPosition() {
        return transform.getPosition();
    }

    float Text::getRotation() {
        return transform.getRotation();
    }

    Vec2F Text::getScale() {
        return transform.getScale();
    }

    void Text::recalcTextDimensions(const std::string& _text) {
        size.y = -1;
        size.x = 0;

        for(auto _c : _text) {
            auto _char = font->getChars()[_c];

            if(_c == ' ') {
                size.x += _char.advance.x / 2.f;
                continue;
            }

            if(_c == '\n') {
                continue;
            }

            size.x += _char.advance.x / 2.f;
            size.y = std::max(size.y, _char.bitmapSize.y - _char.bearing.y);
        }

        debugShape.makeSquare(getPosition(), size);
    }

    Vec2F Text::getTextSize() {
        return size;
    }

    void Text::setFontSize(int _fontSize) {
        fontSize = _fontSize;
        recalcTextDimensions(innerText);
        font = FontManager::get().getSpecificFont(font->getFontName(), _fontSize);
    }

    int Text::getFontSize() {
        return fontSize;
    }

    void Text::setSpaceWidth(float _spaceWidth) {
        spaceWidth = _spaceWidth;
        recalcTextDimensions(innerText);
    }

    float Text::getSpaceWidth() const {
        return spaceWidth;
    }

    float Text::getSpacesBetweenChars() const {
        return spaceBetweenChars;
    }

    void Text::setSpacesBetweenChars(float _spaceBetweenChars) {
        spaceBetweenChars = _spaceBetweenChars;
        recalcTextDimensions(innerText);
    }

    void Text::setTextColor(const Color& _color) {
        textColor = _color;
    }

    Color& Text::getColor() {
        return textColor;
    }

    ShaderID Text::getShaderID() {
        return shaderID;
    }

    Texture* Text::getTexture() {
        return &font->getTexture();
    }

    void Text::setShader(ShaderID _shaderID) {
        shaderID = _shaderID;
    }

    void Text::setTexture(Texture* _texture) {  }

    Shape& Text::getDebugShape() {
        return debugShape;
    }
}