//
// Created by borja on 9/5/22.
//

#include "core/graph/components/TextRenderer.h"
#include "core/util/Functions.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/Vertex.h"
#include "core/render/elements/Batch.h"
#include "core/systems/uiSystem/FontManager.h"
#include "core/graph/Scene.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/Engine.h"

namespace GDE {

    TextRenderer::TextRenderer(Node* _node, Scene* _scene, const std::string& _text, Font* _font) :
    TextRenderer(_node, &_scene->engine->manager, _scene->getMainGraph(), _text, _font) {  }

    TextRenderer::TextRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const std::string& _text, Font* _font) :
    TextRenderer(_node, &_scene->engine->manager, _canvas->getGraph(), _text, _font) {  }

    TextRenderer::TextRenderer(Node* _node, Manager* _manager, Graph* _graph, const std::string& _text, Font* _font) : IRenderizable(_node) {
        font = _font;

        if(_font == nullptr) {
            font = _manager->fontManager.getDefaultFont("MontserratRegular");
        } else {
            font = _font;
        }

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
        auto [_, _totalWidth, _totalHeight] = calculateLinesInfo(font->getChars());
        size.set(_totalWidth, _totalHeight);
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

    void TextRenderer::drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const ViewPort& _viewport) {
        if(!enabled) return;

        auto* _atlas = font;
        auto _atlasSize = _atlas->getSize();

        float _x = 0;
        float _y = 0;

        auto* _chars = _atlas->getChars();

        auto [_linesInfo, _, _totalHeight] = calculateLinesInfo(_chars);
        const auto _numberOfLines = _linesInfo.size();
        const auto _percentage = 1.f / (float)_numberOfLines;

        const float _nonUIPivotY = 0.4f;
        const float _nonUIPivotX = 0.5f;

        auto _index = 0;
        for(auto& _lineInfo : _linesInfo) {

            // This is to adjust X, so it goes back to the right, and to set the Y to its proper height when in multiline.
            if(_linesInfo.size() > 1) {
                auto _amountToAdd = (float)_numberOfLines / 2.f - (float)_index;
                _y = -(_totalHeight * newLineSize) * _percentage * _amountToAdd - (_totalHeight / 2.f) + _lineInfo.biggestCharHeight;
                _index++;
                _x = 0;
            }

            for(char _char : _lineInfo.line) {
                auto _vertexCount = _vertices.size();

                float _xPos = (_x - (size.x * _nonUIPivotX) + (float)_chars[_char].bearing.x + spaceBetweenChars) * _transform.getModelMatrixScale().x;
                float _yPos = (_y + (size.y * _nonUIPivotY) - (float)_chars[_char].bearing.y) * _transform.getModelMatrixScale().x;

                float _w = (float)_chars[_char].size.x * _transform.getModelMatrixScale().x;
                float _h = (float)_chars[_char].size.y * _transform.getModelMatrixScale().x;

                auto [_transformMat, _dirty] = _transform.localToWorld();
                if(_dirty || dirty) {
                    _transform.clearDirty();
                    dirty = false;
                }
                auto _screenPos = Util::worldToScreenCoords(_viewport, { _transformMat[3][0], _transformMat[3][1] });
                _transformMat[3][0] = _screenPos.x;
                _transformMat[3][1] = _screenPos.y;

                auto _textColor = color;
                glm::vec4 _color = {(float)_textColor.r / 255.f, (float)_textColor.g/ 255.f,(float)_textColor.b/ 255.f, (float)_textColor.a/ 255.f};

                auto _positionInScreen = Util::worldToScreenSize(_viewport, { _xPos, _yPos });
                auto _sizeInScreen = Util::worldToScreenSize(_viewport, { _w, _h });

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

                _x += (float)_chars[_char].advance.x;

                _indices.emplace_back(_vertexCount + 0);
                _indices.emplace_back(_vertexCount + 1);
                _indices.emplace_back(_vertexCount + 2);

                _indices.emplace_back(_vertexCount + 2);
                _indices.emplace_back(_vertexCount + 3);
                _indices.emplace_back(_vertexCount + 0);
            }
        }
    }

    std::tuple<std::vector<TextRenderer::LineInfo>, float, float> TextRenderer::calculateLinesInfo(CharInfo* _chars) const {
        auto _linesInfo = std::vector<LineInfo> {  };
        auto _ss = std::stringstream { innerText };

        auto _totalHeight = 0.0f;
        auto _totalWidth = 0.0f;
        for (std::string _line; std::getline(_ss, _line, '\n');) {
            LineInfo _lineInfo {
                    .line = _line
            };

            float _biggestHeight = font->getBiggestCharHeight();
            float _biggestWidth = 0.f;
            if(!_line.empty()) {
                for(auto _char : _line) {
                    _biggestWidth += (float)_chars[_char].advance.x;
                }
            }

            _lineInfo.biggestCharHeight = _biggestHeight;
            _linesInfo.push_back(_lineInfo);
            _totalHeight += _biggestHeight;
            _totalWidth = std::max(_totalWidth, _biggestWidth);
        }

        return std::tuple {_linesInfo, _totalWidth, _totalHeight};
    }

    void TextRenderer::drawAndFlush(std::vector<DrawAndFlushData>& _data, Transform& _transform, const ViewPort& _viewport) {
        if(!enabled) return;
        DrawAndFlushData _textData;
        _textData.textureID = getTexture();
        _textData.shaderID = shaderID;
        drawBatched(_textData.vertices, _textData.indices, _transform, _viewport);
        _data.push_back(_textData);
    }

}