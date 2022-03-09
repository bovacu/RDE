// Created by borja on 15/1/22.

#include "core/systems/uiSystem/Text.h"

namespace engine {

    void Text::init(Font* _font, const std::string& _text) {
        font = _font;
        innerText = _text;
        recalcTextDimensions(_text);
        spriteRenderer.shaderID = ShaderManager::get().getShader("basicText");
        debugShape.setOutlineColor(Color::Green);
        debugShape.showInnerColor(false);
        spriteRenderer.texture = &font->getTexture();
    }

    void Text::setText(const std::string& _text) {
        innerText = _text;
        recalcTextDimensions(_text);
    }

    void Text::setFont(Font* _font) {
        font = _font;
    }

    Font* Text::getFont() const {
        return font;
    }

    const std::string& Text::getText() const {
        return innerText;
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

//        debugShape.makeSquare(getPosition(), size);
    }

    Vec2F Text::getTextSize() const {
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

    Shape& Text::getDebugShape() {
        return debugShape;
    }
}