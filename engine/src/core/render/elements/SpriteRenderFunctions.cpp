#include "core/graph/components/TextRenderer.h"
#include "core/render/elements/SpriteBatchRenderFunctions.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/ViewPort.h"
#include "core/util/Functions.h"
#include "core/render/elements/IRenderizable.h"
#include "core/systems/uiSystem/FontManager.h"

namespace RDE {

	void calculateGeometryForSpriteRenderer(RenderizableInnerData& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport)  {
	    auto _screenPos = Util::Math::worldToScreenCoords(_viewport, {_transformMatrix[3][0], _transformMatrix[3][1]});
	    _transformMatrix[3][0] = _screenPos.x;
	    _transformMatrix[3][1] = _screenPos.y;

	    Vec2F _textureOrigin = {(float)_data.texture->getRegion().bottomLeftCorner.x, (float)_data.texture->getRegion().bottomLeftCorner.y};
	    Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_data.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)_data.texture->getSpriteSheetSize().y};

	    Vec2F _textureTileSize = {(float)_data.texture->getRegion().size.x, (float)_data.texture->getRegion().size.y};
	    Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)_data.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)_data.texture->getSpriteSheetSize().y};
	    auto _textureTileSizeOnScreen = Util::Math::worldToScreenSize(_viewport, _textureTileSize);

	    glm::vec4 _bottomLeftTextureCorner  = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
	    glm::vec4 _bottomRightTextureCorner = {  _textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
	    glm::vec4 _topRightTextureCorner    = {  _textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };
	    glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };

	    glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x                         , _textureOriginNorm.y };
	    glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y };
	    glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y };
	    glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x                         , _textureOriginNorm.y + _textureTileSizeNorm.y };

	    glm::vec4 _color = { (float)_data.color.r / 255.f, (float)_data.color.g/ 255.f,(float)_data.color.b/ 255.f, (float)_data.color.a/ 255.f };

	   _data.vertices[0] = OpenGLVertex {_transformMatrix * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _color };
	   _data.vertices[1] = OpenGLVertex {_transformMatrix * _bottomRightTextureCorner, _bottomRightTextureCoord, _color };
	   _data.vertices[2] = OpenGLVertex {_transformMatrix * _topRightTextureCorner   , _topRightTextureCoord   , _color };
	   _data.vertices[3] = OpenGLVertex {_transformMatrix * _topLeftTextureCorner    , _topLeftTextureCoord    , _color };
	}

	void drawBatchedSpriteRenderer(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport) {
        auto _vertexCount = _batch->vertexBuffer.size();

        auto [_transformMat, _dirty] = _transform->localToWorld();
        if(_dirty || _data.dirty) {
            calculateGeometryForSpriteRenderer(_data, _transformMat, _transform, _viewport);
            _data.dirty = false;
        }

        _batch->vertexBuffer.emplace_back(_data.vertices[0]);
        _batch->vertexBuffer.emplace_back(_data.vertices[1]);
        _batch->vertexBuffer.emplace_back(_data.vertices[2]);
        _batch->vertexBuffer.emplace_back(_data.vertices[3]);

        _batch->indexBuffer.emplace_back(_vertexCount + 0);
        _batch->indexBuffer.emplace_back(_vertexCount + 1);
        _batch->indexBuffer.emplace_back(_vertexCount + 2);

        _batch->indexBuffer.emplace_back(_vertexCount + 2);
        _batch->indexBuffer.emplace_back(_vertexCount + 3);
        _batch->indexBuffer.emplace_back(_vertexCount + 0);
    }




    void calculateGeometryForTextRenderer(RenderizableInnerData& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {

    }

    void drawBatchedTextRenderer(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport) {
    	TextRenderer* _textRenderer = (TextRenderer*)_data.extraInfo;

    	auto* _atlas = _textRenderer->getFont();
        auto _atlasSize = _atlas->getSize();

        float _x = 0;
        float _y = 0;

        auto* _chars = _atlas->getChars();

        auto [_linesInfo, _, _totalHeight] = _textRenderer->calculateLinesInfo(_chars);
        const auto _numberOfLines = _linesInfo.size();
        const auto _percentage = 1.f / (float)_numberOfLines;

        const float _nonUIPivotY = 0.4f;
        const float _nonUIPivotX = 0.5f;

        auto _index = 0;
        for(auto& _lineInfo : _linesInfo) {

            // This is to adjust X, so it goes back to the right, and to set the Y to its proper height when in multiline.
            if(_linesInfo.size() > 1) {
                auto _amountToAdd = (float)_numberOfLines / 2.f - (float)_index;
                _y = -(_totalHeight * _textRenderer->getNewLineSize()) * _percentage * _amountToAdd - (_totalHeight / 2.f) + _lineInfo.biggestCharHeight;
                _index++;
                _x = 0;
            }

            for(char _char : _lineInfo.line) {
                auto _vertexCount = _batch->vertexBuffer.size();

                float _xPos = (_x - (_textRenderer->getTextSize().x * _nonUIPivotX) + (float)_chars[_char].bearing.x + _textRenderer->getSpacesBetweenChars()) * _transform->getModelMatrixScale().x;
                float _yPos = (_y + (_textRenderer->getTextSize().y * _nonUIPivotY) - (float)_chars[_char].bearing.y) * _transform->getModelMatrixScale().x;

                float _w = (float)_chars[_char].size.x * _transform->getModelMatrixScale().x;
                float _h = (float)_chars[_char].size.y * _transform->getModelMatrixScale().x;

                auto [_transformMat, _dirty] = _transform->localToWorld();
                if(_dirty || _data.dirty) {
                    _data.dirty = false;
                }
                auto _screenPos = Util::Math::worldToScreenCoords(_viewport, { _transformMat[3][0], _transformMat[3][1] });
                _transformMat[3][0] = _screenPos.x;
                _transformMat[3][1] = _screenPos.y;

                auto _textColor = _data.color;
                glm::vec4 _color = {(float)_textColor.r / 255.f, (float)_textColor.g/ 255.f,(float)_textColor.b/ 255.f, (float)_textColor.a/ 255.f};

                auto _positionInScreen = Util::Math::worldToScreenSize(_viewport, { _xPos, _yPos });
                auto _sizeInScreen = Util::Math::worldToScreenSize(_viewport, { _w, _h });

                glm::vec4 _bottomLeftTextureCorner  = { _positionInScreen.x                  , -_positionInScreen.y                  , 0.0f, 1.0f };
                glm::vec4 _bottomRightTextureCorner = { _positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y                  , 0.0f, 1.0f };
                glm::vec4 _topLeftTextureCorner     = { _positionInScreen.x                  , -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };
                glm::vec4 _topRightTextureCorner    = { _positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };

                Vec2F _normSize = { (float)_chars[_char].size.x / _atlasSize.x, (float)_chars[_char].size.y / _atlasSize.y };

                glm::vec2 _bottomLeftTextureCoord   = { _chars[_char].offset.x              , _chars[_char].offset.y               };
                glm::vec2 _bottomRightTextureCoord  = { _chars[_char].offset.x + _normSize.x, _chars[_char].offset.y               };
                glm::vec2 _topLeftTextureCoord      = { _chars[_char].offset.x              , _chars[_char].offset.y + _normSize.y };
                glm::vec2 _topRightTextureCoord     = { _chars[_char].offset.x + _normSize.x, _chars[_char].offset.y + _normSize.y };

                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord   , _color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _bottomRightTextureCorner, _bottomRightTextureCoord  , _color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _topRightTextureCorner   , _topRightTextureCoord     , _color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _topLeftTextureCorner    , _topLeftTextureCoord      , _color });

                _x += (float)_chars[_char].advance.x;

                _batch->indexBuffer.emplace_back(_vertexCount + 0);
                _batch->indexBuffer.emplace_back(_vertexCount + 1);
                _batch->indexBuffer.emplace_back(_vertexCount + 2);

                _batch->indexBuffer.emplace_back(_vertexCount + 2);
                _batch->indexBuffer.emplace_back(_vertexCount + 3);
                _batch->indexBuffer.emplace_back(_vertexCount + 0);
            }
        }
    }




    void calculateGeometryForUIText(RenderizableInnerData& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {

    }

    void drawBatchedUIText(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport) {

    }




    void calculateGeometryForUIImage(RenderizableInnerData& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {

    }

    void drawBatchedUIImage(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport) {

    }


}