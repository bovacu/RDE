//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UIText.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"

namespace GDE {

    UIText::UIText(Node* _node, Scene* _scene, Canvas* _canvas, const std::string& _text, Font* _font) :
            UIText(_node, &_scene->engine->manager, _canvas->getGraph(), _text, _font) {  }

    UIText::UIText(Node* _node, Manager* _manager, Graph* _graph, const std::string& _text, Font* _font) :
    TextRenderer(_node, _manager, _graph, _text, _font) {  }

    void UIText::drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) {
        // TODO: PIVOT CHANGES
//        if(!enabled) return;
//
//        auto* _uiTransform = (UITransform*)&_transform;
//        auto _pivot = _uiTransform->getPivot();
//        auto _anchor = _uiTransform->getAnchor();
//
//        auto* _atlas = font;
//        auto _atlasSize = _atlas->getSize();
//
//        float _x = 0;
//        float _y = 0;
//
//        auto* _chars = _atlas->getChars();
//
//        auto [_linesInfo, _, _totalHeight] = calculateLinesInfo(_chars);
//        const auto _numberOfLines = _linesInfo.size();
//        const auto _percentage = 1.f / (float)_numberOfLines;
//        auto _index = 0;
//        for(auto& _lineInfo : _linesInfo) {
//
//            // This is to adjust X, so it goes back to the right, and to set the Y to its proper height when in multiline.
//            if(_linesInfo.size() > 1) {
//                auto _amountToAdd = (float)_numberOfLines / 2.f - (float)_index;
//                _y = -(_totalHeight * newLineSize) * _percentage * _amountToAdd - (_totalHeight / 2.f) + _lineInfo.biggestCharHeight;
//                _index++;
//                _x = 0;
//            }
//
//            for(char _char : _lineInfo.line) {
//                auto _vertexCount = _vertices.size();
//
//                float _xPos = (_x - (_pivot.x * size.x) + (float)_chars[_char].bearing.x + spaceBetweenChars) * _transform.getModelMatrixScale().x;
//                float _yPos = (_y + (_pivot.y * size.y / 1.25f) - (float)_chars[_char].bearing.y) * _transform.getModelMatrixScale().x;
//
//                float _w = (float)_chars[_char].size.x * _transform.getModelMatrixScale().x;
//                float _h = (float)_chars[_char].size.y * _transform.getModelMatrixScale().x;
//
//                auto [_transformMat, _dirty] = _transform.localToWorld();
//                if(_dirty || dirty) {
//                    _transform.clearDirty();
//                    dirty = false;
//                }
//                auto _screenPos = Util::worldToScreenCoords(_viewport, { _transformMat[3][0], _transformMat[3][1] });
//                _transformMat[3][0] = _screenPos.x;
//                _transformMat[3][1] = _screenPos.y;
//
//                auto _textColor = color;
//                glm::vec4 _color = {(float)_textColor.r / 255.f, (float)_textColor.g/ 255.f,(float)_textColor.b/ 255.f, (float)_textColor.a/ 255.f};
//
//                auto _positionInScreen = Util::worldToScreenSize(_viewport, { _xPos, _yPos });
//                auto _sizeInScreen = Util::worldToScreenSize(_viewport, { _w, _h });
//
//                glm::vec4 _bottomLeftTextureCorner  = { _positionInScreen.x                  , -_positionInScreen.y                  , 0.0f, 1.0f };
//                glm::vec4 _bottomRightTextureCorner = { _positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y                  , 0.0f, 1.0f };
//                glm::vec4 _topLeftTextureCorner     = { _positionInScreen.x                  , -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };
//                glm::vec4 _topRightTextureCorner    = { _positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };
//
//                Vec2F _normSize = { (float)_chars[_char].size.x / _atlasSize.x, (float)_chars[_char].size.y / _atlasSize.y };
//
//                glm::vec2 _bottomLeftTextureCoord   = { _chars[_char].offset.x              , _chars[_char].offset.y               };
//                glm::vec2 _bottomRightTextureCoord  = { _chars[_char].offset.x + _normSize.x, _chars[_char].offset.y               };
//                glm::vec2 _topLeftTextureCoord      = { _chars[_char].offset.x              , _chars[_char].offset.y + _normSize.y };
//                glm::vec2 _topRightTextureCoord     = { _chars[_char].offset.x + _normSize.x, _chars[_char].offset.y + _normSize.y };
//
//                _vertices.emplace_back(OpenGLVertex   {_transformMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord   , _color });
//                _vertices.emplace_back(OpenGLVertex   {_transformMat * _bottomRightTextureCorner, _bottomRightTextureCoord  , _color });
//                _vertices.emplace_back(OpenGLVertex   {_transformMat * _topRightTextureCorner   , _topRightTextureCoord     , _color });
//                _vertices.emplace_back(OpenGLVertex   {_transformMat * _topLeftTextureCorner    , _topLeftTextureCoord      , _color });
//
//                _x += (float)_chars[_char].advance.x;
//
//                _indices.emplace_back(_vertexCount + 0);
//                _indices.emplace_back(_vertexCount + 1);
//                _indices.emplace_back(_vertexCount + 2);
//
//                _indices.emplace_back(_vertexCount + 2);
//                _indices.emplace_back(_vertexCount + 3);
//                _indices.emplace_back(_vertexCount + 0);
//            }
//        }
    }

    void UIText::drawAndFlush(std::vector<DrawAndFlushData>& _data, Transform& _transform, const IViewPort& _viewport) {
        TextRenderer::drawAndFlush(_data, _transform, _viewport);
    }


}