//
// Created by borja on 9/5/22.
//

#include "core/graph/components/TextRenderer.h"
#include "core/util/Functions.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/Vertex.h"
#include "core/render/elements/Batch.h"
#include "core/systems/uiSystem/FontManager.h"

namespace GDE {

    TextRenderer::TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font, const std::string& _text) {
        font = _font;
        innerText = _text;
        recalcTextDimensions(_text);
        shaderID = _scene->engine->manager.shaderManager.getShader("basicText")->getShaderID();
        texture = &font->getTexture();
        IRenderizable::batchPriority = BatchPriority::TextPriority;
    }

    TextRenderer::TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font) {
        font = _font;
        shaderID = _scene->engine->manager.shaderManager.getShader("basicText")->getShaderID();
        texture = &font->getTexture();
        IRenderizable::batchPriority = BatchPriority::TextPriority;
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
        LOG_W("Set font size not working!!")
//        fontSize = _fontSize;
//        recalcTextDimensions(innerText);
//        font = FontManager::get().getSpecificFont(font->getFontName(), _fontSize);
    }

    int TextRenderer::getFontSize() const {
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

    void TextRenderer::draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const {
        auto* _atlas = font;
        auto _atlasSize = _atlas->getSize();

        float _x = _transform.getPositionLocal().x - size.x / 2.f;
        float _y = _transform.getPositionLocal().y - size.y / 4.f;

        auto* _chars = _atlas->getChars();

        for(char _char : innerText) {
            auto _vertexCount = _vertices.size();
            float x2 = _x + _chars[_char].bitmapPos.x * 1;
            float y2 = -_y - _chars[_char].bitmapPos.y * 1;
            float w = _chars[_char].bitmapSize.x * 1;
            float h = _chars[_char].bitmapSize.y * 1;

            if(_char == '\n') {
                _y -= _chars[innerText[0]].bitmapSize.y * 0.8f;
                _x = _transform.getPositionLocal().x - size.x / 2.f;
                continue;
            }

            if (w == 0 || h == 0) {
                _x += _chars[_char].advance.x / 2.f;
                continue;
            }

            auto _transformMat = _transform.modelMatrix;
            auto _screenPos = Util::worldToScreenCoords(_viewport, {_transform.modelMatrix[3][0], _transform.modelMatrix[3][1]});
            _transformMat[3][0] = _screenPos.x;
            _transformMat[3][1] = _screenPos.y;

            auto _textColor = color;
            glm::vec4 _color = {(float)_textColor.r / 255.f, (float)_textColor.g/ 255.f,(float)_textColor.b/ 255.f, (float)_textColor.a/ 255.f};

            auto _positionInScreen = Util::worldToScreenSize(_viewport, {x2 + _x, y2 + _y});
            auto _sizeInScreen = Util::worldToScreenSize(_viewport, {w, h});

            glm::vec4 _bottomLeftTextureCorner = { _positionInScreen.x, -_positionInScreen.y, 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = {_positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y, 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner = {_positionInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner = {_positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };

            glm::vec2 _bottomLeftTextureCoord = {_chars[_char].offset.x, _chars[_char].offset.y};
            glm::vec2 _bottomRightTextureCoord = {_chars[_char].offset.x + _chars[_char].bitmapSize.x / _atlasSize.x, _chars[_char].offset.y};
            glm::vec2 _topLeftTextureCoord = {_chars[_char].offset.x, _chars[_char].offset.y + _chars[_char].bitmapSize.y / _atlasSize.y};
            glm::vec2 _topRightTextureCoord = {_chars[_char].offset.x + _chars[_char].bitmapSize.x / _atlasSize.x,_chars[_char].offset.y + _chars[_char].bitmapSize.y / _atlasSize.y};

            _vertices.emplace_back(OpenGLVertex {_transformMat * _bottomLeftTextureCorner, _bottomLeftTextureCoord, _color });
            _vertices.emplace_back(OpenGLVertex {_transformMat * _bottomRightTextureCorner, _bottomRightTextureCoord, _color });
            _vertices.emplace_back(OpenGLVertex {_transformMat * _topRightTextureCorner, _topRightTextureCoord, _color });
            _vertices.emplace_back(OpenGLVertex {_transformMat * _topLeftTextureCorner, _topLeftTextureCoord, _color });

            _x += _chars[_char].advance.x / 2.f;
            _y += _chars[_char].advance.y;

            _indices.emplace_back(_vertexCount + 0);
            _indices.emplace_back(_vertexCount + 1);
            _indices.emplace_back(_vertexCount + 2);

            _indices.emplace_back(_vertexCount + 2);
            _indices.emplace_back(_vertexCount + 3);
            _indices.emplace_back(_vertexCount + 0);
        }
    }

}