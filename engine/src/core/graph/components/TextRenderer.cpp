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

    TextRenderer::TextRenderer(const NodeID& _nodeId, Scene* _scene, Font* _font, const std::string& _text) : IRenderizable(_scene->getMainGraph()->getComponent<Transform>(_nodeId)) {
        font = _font;
        innerText = _text;
        recalcTextDimensions(_text);
        shaderID = defaultShaders[TEXT_RENDERER_SHADER];
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
        size.y = 0;
        size.x = 0;
        float _maxHeightOfLine = -1;
        float _maxWidthOfLine = -1;

        for(auto _c : _text) {
            auto _char = font->getChars()[_c];

            if(_c == '\n') {
                size.y += _maxHeightOfLine;
                size.x = std::max(size.x, _maxWidthOfLine);
                _maxHeightOfLine = 0;
                _maxWidthOfLine = 0;
                continue;
            }

            _maxWidthOfLine += (float)_char.advance.x;
            _maxHeightOfLine = std::max(size.y, (float)_char.advance.y);
        }

        if(size.y < _maxHeightOfLine) {
            size.y = _maxHeightOfLine;
        }

        if(size.x < _maxWidthOfLine) {
            size.x = _maxWidthOfLine;
        }
    }

    Vec2F TextRenderer::getTextSize() const {
        return { size.x * transform->getModelMatrixScale().x, size.y * transform->getModelMatrixScale().y };
    }

    void TextRenderer::setFontSize(int _fontSize) {
        LOG_W("Set font size not working!!")
//        fontSize = _fontSize;
//        font = FontManager::get().getSpecificFont(font->getFontName(), _fontSize);
//        recalcTextDimensions(innerText);
    }

    int TextRenderer::getFontSize() const {
        return fontSize;
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

        float _x = 0;
        float _y = 0;
        float _biggestAdvanceY = 0;

        auto* _chars = _atlas->getChars();

        for(char _char : innerText) {
            auto _vertexCount = _vertices.size();
            _biggestAdvanceY = std::max(_biggestAdvanceY, (float)(_chars[_char].advance.y - _chars[_char].bearing.y) + newLineSize);

            if(_char == '\n') {
                _y += _biggestAdvanceY * _transform.getModelMatrixScale().x;
                _x = 0;
                continue;
            }

            float xpos = (_x - (pivot.x * size.x) + (float)_chars[_char].bearing.x + spaceBetweenChars) * _transform.getModelMatrixScale().x;
            float ypos = (_y + (pivot.y * size.y) - (float)_chars[_char].bearing.y) * _transform.getModelMatrixScale().x;

            float w = (float)_chars[_char].size.x * _transform.getModelMatrixScale().x;
            float h = (float)_chars[_char].size.y * _transform.getModelMatrixScale().x;

            auto _transformMat = _transform.modelMatrix;
            auto _screenPos = Util::worldToScreenCoords(_viewport, { _transform.modelMatrix[3][0], _transform.modelMatrix[3][1] });
            _transformMat[3][0] = _screenPos.x;
            _transformMat[3][1] = _screenPos.y;

            auto _textColor = color;
            glm::vec4 _color = {(float)_textColor.r / 255.f, (float)_textColor.g/ 255.f,(float)_textColor.b/ 255.f, (float)_textColor.a/ 255.f};

            auto _positionInScreen = Util::worldToScreenSize(_viewport, {xpos, ypos});
            auto _sizeInScreen = Util::worldToScreenSize(_viewport, {w, h});

            glm::vec4 _bottomLeftTextureCorner  = { _positionInScreen.x                  , -_positionInScreen.y                  , 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = { _positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y                  , 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner     = { _positionInScreen.x                  , -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner    = { _positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };

            Vec2F _normSize = { (float)_chars[_char].size.x / _atlasSize.x, (float)_chars[_char].size.y / _atlasSize.y };

            glm::vec2 _bottomLeftTextureCoord   = { _chars[_char].offset.x              , _chars[_char].offset.y               };
            glm::vec2 _bottomRightTextureCoord  = { _chars[_char].offset.x + _normSize.x, _chars[_char].offset.y               };
            glm::vec2 _topLeftTextureCoord      = { _chars[_char].offset.x              , _chars[_char].offset.y + _normSize.y };
            glm::vec2 _topRightTextureCoord     = { _chars[_char].offset.x + _normSize.x, _chars[_char].offset.y + _normSize.y };

            _vertices.emplace_back(OpenGLVertex   {_transformMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord   , _color });
            _vertices.emplace_back(OpenGLVertex   {_transformMat * _bottomRightTextureCorner, _bottomRightTextureCoord  , _color });
            _vertices.emplace_back(OpenGLVertex   {_transformMat * _topRightTextureCorner   , _topRightTextureCoord     , _color });
            _vertices.emplace_back(OpenGLVertex   {_transformMat * _topLeftTextureCorner    , _topLeftTextureCoord      , _color });

            _x += (float)_chars[_char].advance.x * _transform.getModelMatrixScale().x;

            _indices.emplace_back(_vertexCount + 0);
            _indices.emplace_back(_vertexCount + 1);
            _indices.emplace_back(_vertexCount + 2);

            _indices.emplace_back(_vertexCount + 2);
            _indices.emplace_back(_vertexCount + 3);
            _indices.emplace_back(_vertexCount + 0);
        }
    }

}