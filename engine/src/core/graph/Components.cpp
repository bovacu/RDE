// Created by borja on 11/3/22.

#include "core/graph/Components.h"
#include "core/graph/Graph.h"
#include "core/graph/Node.h"
#include "core/util/Functions.h"

namespace engine {

    glm::mat4 Transform::getLocalModelMatrix() {
        const glm::mat4 _rot = glm::rotate(glm::mat4(1.0f), glm::radians(localRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        localModelMatrix = glm::translate(glm::mat4(1.0f), localPosition) * _rot * glm::scale(glm::mat4(1.0f), localScale);
        return localModelMatrix;
    }

    void Transform::update(Graph* _graph) {
        if (parent != NODE_ID_NULL && !constant) {
            modelMatrix = _graph->getComponent<Transform>(parent)->modelMatrix * localModelMatrix;
        } else
            modelMatrix = getLocalModelMatrix();
    }

    void Transform::setPosition(const Vec2F& _position) {
        setPosition(_position.x, _position.y);
    }

    void Transform::setPosition(float _x, float _y) {
        Util::worldToScreenSize(_x, _y);
        localPosition = glm::vec3 {_x, _y, 0.0f};
        getLocalModelMatrix();
    }

    Vec2F Transform::getPositionLocal() const {
        float _x = localPosition.x, _y = localPosition.y;
        Util::screenToWorldCoords(_x, _y);
        return {_x, _y};
    }

    void Transform::setRotation(float _rotation) {
        localRotation = _rotation;
        getLocalModelMatrix();
    }

    float Transform::getRotationLocal() const {
        return localRotation;
    }

    void Transform::setScale(const Vec2F& _scale) {
        setScale(_scale.x, _scale.y);
    }

    void Transform::setScale(float _x, float _y) {
        localScale = {_x, _y, 1.0f};
        getLocalModelMatrix();
    }

    Vec2F Transform::getScaleLocal() const {
        return {localScale.x, localScale.y};
    }

    void Transform::translate(const Vec2F& _direction) {
        translate(_direction.x, _direction.y);
    }

    void Transform::translate(float _x, float _y) {
        Util::worldToScreenSize(_x, _y);
        localPosition.x += _x;
        localPosition.y += _y;
        getLocalModelMatrix();
    }

    void Transform::rotate(float _amount) {
        localRotation += _amount;
        getLocalModelMatrix();
    }

    void Transform::scale(const Vec2F& _scale) {
        scale(_scale.x, _scale.y);
    }

    void Transform::scale(float _x, float _y) {
        localScale.x += _x;
        localScale.y += _y;
        getLocalModelMatrix();
    }

    Vec2F Transform::getPositionWorld() const {
        float _x = modelMatrix[3][0], _y =  modelMatrix[3][1];
        Util::screenToWorldCoords(_x, _y);
        return {_x, _y};
    }

    float Transform::getRotationWorld() const {
        return -1;
    }

    Vec2F Transform::getScaleLWorld() const {
        return { modelMatrix[0][0], modelMatrix[1][1] };
    }

    void Transform::setConstant(bool _constant) {
        constant = _constant;
    }

    bool Transform::isConstant() {
        return constant;
    }


    /// -------------------------------------------

    TextRenderer::TextRenderer(Font* _font, const std::string& _text) {
        font = _font;
        innerText = _text;
        recalcTextDimensions(_text);
        spriteRenderer.shaderID = ShaderManager::get().getShader("basicText");
        spriteRenderer.texture = &font->getTexture();
    }

    TextRenderer::TextRenderer(Font* _font) {
        font = _font;
        spriteRenderer.shaderID = ShaderManager::get().getShader("basicText");
        spriteRenderer.texture = &font->getTexture();
    }

    void TextRenderer::setText(const std::string& _text) {
        innerText = _text;
        recalcTextDimensions(_text);
    }

    void TextRenderer::setFont(Font* _font) {
        font = _font;
    }

    Font* TextRenderer::getFont() const {
        return font;
    }

    const std::string& TextRenderer::getText() const {
        return innerText;
    }

    void TextRenderer::recalcTextDimensions(const std::string& _text) {
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

//        debugShape.makeSquare(getPositionLocal(), size);
    }

    Vec2F TextRenderer::getTextSize() const {
        return size;
    }

    void TextRenderer::setFontSize(int _fontSize) {
        fontSize = _fontSize;
        recalcTextDimensions(innerText);
        font = FontManager::get().getSpecificFont(font->getFontName(), _fontSize);
    }

    int TextRenderer::getFontSize() {
        return fontSize;
    }

    void TextRenderer::setSpaceWidth(float _spaceWidth) {
        spaceWidth = _spaceWidth;
        recalcTextDimensions(innerText);
    }

    float TextRenderer::getSpaceWidth() const {
        return spaceWidth;
    }

    float TextRenderer::getSpacesBetweenChars() const {
        return spaceBetweenChars;
    }

    void TextRenderer::setSpacesBetweenChars(float _spaceBetweenChars) {
        spaceBetweenChars = _spaceBetweenChars;
        recalcTextDimensions(innerText);
    }

//    Shape& TextRenderer::getDebugShape() {
//        return debugShape;
//    }

}
