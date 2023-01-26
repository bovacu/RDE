#include "core/graph/components/TextRenderer.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UIText.h"
#include "core/render/elements/SpriteBatchRenderFunctions.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/ViewPort.h"
#include "core/util/Functions.h"
#include "core/render/elements/IRenderizable.h"
#include "core/systems/uiSystem/FontManager.h"
#include "core/graph/components/ui/UITransform.h"

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

        auto _uint32Color = Util::Math::colorToUint32_t(_data.color);

	   _data.vertices[0] = OpenGLVertex {_transformMatrix * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _uint32Color };
	   _data.vertices[1] = OpenGLVertex {_transformMatrix * _bottomRightTextureCorner, _bottomRightTextureCoord, _uint32Color };
	   _data.vertices[2] = OpenGLVertex {_transformMatrix * _topRightTextureCorner   , _topRightTextureCoord   , _uint32Color };
	   _data.vertices[3] = OpenGLVertex {_transformMatrix * _topLeftTextureCorner    , _topLeftTextureCoord    , _uint32Color };
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


        _batch->vertexBuffer.emplace_back(_data.vertices[2]);
        _batch->vertexBuffer.emplace_back(_data.vertices[3]);
        _batch->vertexBuffer.emplace_back(_data.vertices[0]);
    }




    void calculateGeometryForTextRenderer(RenderizableInnerData& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {

    }

    void drawBatchedTextRenderer(RenderizableInnerData& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport) {
    	auto* _textRenderer = (TextRenderer*)_data.extraInfo;

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

                auto _uint32Color = Util::Math::colorToUint32_t(_data.color);

                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord   , _uint32Color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _bottomRightTextureCorner, _bottomRightTextureCoord  , _uint32Color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _topRightTextureCorner   , _topRightTextureCoord     , _uint32Color });

                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _topRightTextureCorner   , _topRightTextureCoord     , _uint32Color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _topLeftTextureCorner    , _topLeftTextureCoord      , _uint32Color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord   , _uint32Color });

                _x += (float)_chars[_char].advance.x;
            }
        }
    }




    void calculateGeometryForUIText(RenderizableInnerDataUI& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {

    }

    void drawBatchedUIText(RenderizableInnerDataUI& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport) {
        auto* _textRenderer = (UIText*)_data.RenderizableInnerData.extraInfo;
        auto _originOffset = _data.originOffset;

        auto* _atlas = _textRenderer->getFont();
        auto _atlasSize = _atlas->getSize();

        float _x = 0;
        float _y = 0;

        auto* _chars = _atlas->getChars();

        auto [_linesInfo, _, _totalHeight] = _textRenderer->calculateLinesInfo(_chars);
        const auto _numberOfLines = _linesInfo.size();
        const auto _percentage = 1.f / (float)_numberOfLines;
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

                float _xPos = (_x - (_textRenderer->getTextSize().x) + (float)_chars[_char].bearing.x + _textRenderer->getSpacesBetweenChars()) * _transform->getModelMatrixScale().x;
                float _yPos = (_y + (_textRenderer->getTextSize().y * 0.75f) - (float)_chars[_char].bearing.y) * _transform->getModelMatrixScale().x;

                float _w = (float)_chars[_char].size.x * _transform->getModelMatrixScale().x;
                float _h = (float)_chars[_char].size.y * _transform->getModelMatrixScale().x;

                auto [_transformMat, _dirty] = _transform->localToWorld();
                if(_dirty || _data.RenderizableInnerData.dirty) {
                    _data.RenderizableInnerData.dirty = false;
                }
                auto _screenPos = Util::Math::worldToScreenCoordsUI(_viewport, { _transformMat[3][0] + _originOffset.x * _transform->getModelMatrixScale().x,
                                                                           _transformMat[3][1] + _originOffset.y  * _transform->getModelMatrixScale().y });
                _transformMat[3][0] = _screenPos.x;
                _transformMat[3][1] = _screenPos.y;

                auto _positionInScreen = Util::Math::worldToScreenSizeUI(_viewport, { _xPos, _yPos });
                auto _sizeInScreen = Util::Math::worldToScreenSizeUI(_viewport, { _w, _h });

                glm::vec4 _bottomLeftTextureCorner  = { _positionInScreen.x                  , -_positionInScreen.y                  , 0.0f, 1.0f };
                glm::vec4 _bottomRightTextureCorner = { _positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y                  , 0.0f, 1.0f };
                glm::vec4 _topLeftTextureCorner     = { _positionInScreen.x                  , -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };
                glm::vec4 _topRightTextureCorner    = { _positionInScreen.x + _sizeInScreen.x, -_positionInScreen.y - _sizeInScreen.y, 0.0f, 1.0f };

                Vec2F _normSize = { (float)_chars[_char].size.x / _atlasSize.x, (float)_chars[_char].size.y / _atlasSize.y };

                glm::vec2 _bottomLeftTextureCoord   = { _chars[_char].offset.x              , _chars[_char].offset.y               };
                glm::vec2 _bottomRightTextureCoord  = { _chars[_char].offset.x + _normSize.x, _chars[_char].offset.y               };
                glm::vec2 _topLeftTextureCoord      = { _chars[_char].offset.x              , _chars[_char].offset.y + _normSize.y };
                glm::vec2 _topRightTextureCoord     = { _chars[_char].offset.x + _normSize.x, _chars[_char].offset.y + _normSize.y };

                auto _uint32Color = Util::Math::colorToUint32_t(_data.RenderizableInnerData.color);

                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord   , _uint32Color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _bottomRightTextureCorner, _bottomRightTextureCoord  , _uint32Color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _topRightTextureCorner   , _topRightTextureCoord     , _uint32Color });

                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _topRightTextureCorner   , _topRightTextureCoord     , _uint32Color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _topLeftTextureCorner    , _topLeftTextureCoord      , _uint32Color });
                _batch->vertexBuffer.emplace_back(OpenGLVertex   {_transformMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord   , _uint32Color });

                _x += (float)_chars[_char].advance.x;
            }
        }
    }





    void calculateNormalGeometry(RenderizableInnerDataUI& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {
        auto _uiT = (UITransform*)_transform;
        auto _originOffset = _data.originOffset;
        auto _screenPos = Util::Math::worldToScreenCoordsUI(_viewport, { _transformMatrix[3][0] + _originOffset.x, _transformMatrix[3][1] + _originOffset.y });

        _transformMatrix[3][0] = _screenPos.x;
        _transformMatrix[3][1] = _screenPos.y;

        auto _uiSizeScale = Vec2F { _uiT->getSize().x / _data.RenderizableInnerData.texture->getSize().x, _uiT->getSize().y / _data.RenderizableInnerData.texture->getSize().y };
        _transformMatrix[0][0] *= _uiSizeScale.x;
        _transformMatrix[1][1] *= _uiSizeScale.y;

        Vec2F _textureOrigin = {(float)_data.RenderizableInnerData.texture->getRegion().bottomLeftCorner.x, (float)_data.RenderizableInnerData.texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)_data.RenderizableInnerData.texture->getRegion().size.x, (float)_data.RenderizableInnerData.texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y};
        auto _textureTileSizeOnScreen = Util::Math::worldToScreenSizeUI(_viewport, _textureTileSize);


        glm::vec4 _bottomLeftTextureCorner  = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {  _textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner    = {  _textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };

        glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x                         , _textureOriginNorm.y };
        glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y };
        glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y };
        glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x                         , _textureOriginNorm.y + _textureTileSizeNorm.y };

        auto _uint32Color = Util::Math::colorToUint32_t(_data.RenderizableInnerData.color);

        _data.RenderizableInnerData.vertices[0] = OpenGLVertex {_transformMatrix * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _uint32Color };
        _data.RenderizableInnerData.vertices[1] = OpenGLVertex {_transformMatrix * _bottomRightTextureCorner, _bottomRightTextureCoord, _uint32Color };
        _data.RenderizableInnerData.vertices[2] = OpenGLVertex {_transformMatrix * _topRightTextureCorner   , _topRightTextureCoord   , _uint32Color };
        _data.RenderizableInnerData.vertices[3] = OpenGLVertex {_transformMatrix * _topLeftTextureCorner    , _topLeftTextureCoord    , _uint32Color };
    }

    void calculate9SliceGeometry(RenderizableInnerDataUI& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {
        // TODO (RDE): this calculations are an absolute mess, I need to clean this up, rename variables, make it clearer and remove redundant operations, but I'm scared.
        auto _rectsAmount = sizeof(_data.RenderizableInnerData.texture->nineSlice.subRects) / sizeof(IntRect);
        auto _originOffset = _data.originOffset;
        auto _uiSize = ((UITransform*)_transform)->getSize();

        for(auto _i = 0; _i < _rectsAmount; _i++) {
            auto& _subTextureRegion = _data.RenderizableInnerData.texture->nineSlice.subRects[_i];

            float _distortX = 1.f, _distortY = 1.f;
            auto _spriteSize = _data.RenderizableInnerData.texture->getRegion().size;

            auto _bottomLeftCornerLocal = Vec2F { (float)(_subTextureRegion.bottomLeftCorner.x - _data.RenderizableInnerData.texture->nineSlice.subRects[0].bottomLeftCorner.x), (float)(_subTextureRegion.bottomLeftCorner.y - _data.RenderizableInnerData.texture->nineSlice.subRects[0].bottomLeftCorner.y) };
            Vec2F _position = { -(float)_spriteSize.x / 2.f + (float)_bottomLeftCornerLocal.x + (float)_subTextureRegion.size.x / 2.f ,
                                -(float)((float)_spriteSize.y / 2.f - _bottomLeftCornerLocal.y) + (float)_subTextureRegion.size.y / 2.f
            };
            Vec2F _scale = { 1, 1 };
            Vec2F _noResizableScale = { 1, 1 };

            // Resize and replace cross-X elements
            if((float)_uiSize.x - (float)_spriteSize.x != 0) {
                auto _widthOfCorners = (float) _data.RenderizableInnerData.texture->nineSlice.subRects[0].size.x + (float) _data.RenderizableInnerData.texture->nineSlice.subRects[2].size.x;
                auto _totalDiffX = (float)_uiSize.x - _widthOfCorners;
                _distortX = _totalDiffX / ((float)_spriteSize.x - _widthOfCorners);
                if(_distortX < 0) _distortX = 0;
                _scale.x *= _distortX;

                auto _halfWidthOfDistortedMiddleRect = _distortX * _transform->getScale().y * (float)_data.RenderizableInnerData.texture->nineSlice.subRects[1].size.x / 2.f;
                auto _halfWidthOfOriginalMiddleRect = (float)_data.RenderizableInnerData.texture->nineSlice.subRects[1].size.x / 2.f;
                if(_i == 0 || _i == 3 || _i == 6) _position.x -= _halfWidthOfDistortedMiddleRect - _halfWidthOfOriginalMiddleRect;
                if(_i == 2 || _i == 5 || _i == 8) _position.x += _halfWidthOfDistortedMiddleRect - _halfWidthOfOriginalMiddleRect;
            }

            // Resize and replace cross-Y elements
            if((float)_uiSize.y - (float)_spriteSize.y != 0) {
                auto _heightOfCorners = (float)_data.RenderizableInnerData.texture->nineSlice.subRects[0].size.y + (float)_data.RenderizableInnerData.texture->nineSlice.subRects[6].size.y;
                auto _totalDiffY = (float)_uiSize.y - _heightOfCorners;
                _distortY = _totalDiffY / ((float)_spriteSize.y - _heightOfCorners);
                if(_distortY < 0) _distortY = 0;
                _scale.y *= _distortY;

                auto _halfHeightOfDistortedMiddleRect = _distortY * _transform->getScale().y * (float)_data.RenderizableInnerData.texture->nineSlice.subRects[3].size.y / 2.f;
                auto _halfHeightOfOriginalMiddleRect = (float)_data.RenderizableInnerData.texture->nineSlice.subRects[3].size.y / 2.f;
                if(_i == 0 || _i == 1 || _i == 2) _position.y -= _halfHeightOfDistortedMiddleRect - _halfHeightOfOriginalMiddleRect;
                if(_i == 6 || _i == 7 || _i == 8) _position.y += _halfHeightOfDistortedMiddleRect - _halfHeightOfOriginalMiddleRect;
            }

            float _sizeOfXCorners = (float)_data.RenderizableInnerData.texture->nineSlice.subRects[0].size.x + (float)_data.RenderizableInnerData.texture->nineSlice.subRects[2].size.x;
            float _propX = _uiSize.x / _sizeOfXCorners;
            if(_propX < 0) _propX = 0.f;
            float _toMoveX = ((float)_subTextureRegion.size.x - ((float)_subTextureRegion.size.x * _propX)) * 0.5f;

            float _sizeOfYCorners = (float)_data.RenderizableInnerData.texture->nineSlice.subRects[0].size.y + (float)_data.RenderizableInnerData.texture->nineSlice.subRects[6].size.y;
            float _propY = _uiSize.y / _sizeOfYCorners;
            if(_propY < 0) _propY = 0.f;
            float _toMoveY = ((float)_subTextureRegion.size.y - ((float)_subTextureRegion.size.y * _propY)) * 0.5f;

            // Scale and move elements that are not meant to be resized if the X is lower than the minimum size
            if(_sizeOfXCorners > _uiSize.x) {

                if(_i == 0 || _i == 6) {
                    _position.x += _toMoveX;
                    _noResizableScale.x *= _propX;
                }

                if(_i == 2 || _i == 8) {
                    _position.x -= _toMoveX;
                    _noResizableScale.x *= _propX;
                }

                if(_i == 7 || _i == 1 || _i == 4) {
                    _noResizableScale.x *= _propX;
                }

                if(_i == 3 || _i == 5) {
                    _noResizableScale.x *= _propX;
                    _position.x += _toMoveX * (_i == 3 ? 1.f : -1.f);
                }
            }

            // Scale and move elements that are not meant to be resized if the Y is lower than the minimum size
            if(_sizeOfYCorners > _uiSize.y) {

                if(_i == 0 || _i == 2) {
                    _position.y += _toMoveY;
                    _noResizableScale.y *= _propY;
                }

                if(_i == 6 || _i == 8) {
                    _position.y -= _toMoveY;
                    _noResizableScale.y *= _propY;
                }

                if(_i == 3 || _i == 5 || _i == 4) {
                    _noResizableScale.y *= _propY;
                }

                if(_i == 1 || _i == 7) {
                    _noResizableScale.y *= _propY;
                    _position.y += _toMoveY * (_i == 1 ? 1.f : -1.f);
                }
            }

            glm::mat4 _current9SliceMat = glm::translate(glm::mat4(1.f), glm::vec3 { _position.x, _position.y, 0 });

            if(_i == 1 || _i == 4 || _i == 7) _current9SliceMat[0][0] *= _scale.x;
            if(_i == 3 || _i == 4 || _i == 5) _current9SliceMat[1][1] *= _scale.y;
            if(_i == 3 || _i == 5 || _i == 4) _current9SliceMat[0][0] *= _noResizableScale.x;
            if(_i == 1 || _i == 4 || _i == 7) _current9SliceMat[1][1] *= _noResizableScale.y;

            if(_i == 0 || _i == 2 || _i == 6 || _i == 8) {
                _current9SliceMat[0][0] *= _noResizableScale.x;
                _current9SliceMat[1][1] *= _noResizableScale.y;
            }

            _current9SliceMat = _transformMatrix * _current9SliceMat;

            auto _subTextureReposition = Vec2F { _originOffset.x, _originOffset.y };
            auto _screenPos = Util::Math::worldToScreenCoordsUI(_viewport, { _current9SliceMat[3][0] + _subTextureReposition.x, _current9SliceMat[3][1] + _subTextureReposition.y });
            _current9SliceMat[3][0] = _screenPos.x;
            _current9SliceMat[3][1] = _screenPos.y;


            Vec2F _textureOrigin = {(float)_subTextureRegion.bottomLeftCorner.x, (float)_subTextureRegion.bottomLeftCorner.y};
            Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y};

            Vec2F _textureTileSize = {(float)_subTextureRegion.size.x, (float)_subTextureRegion.size.y};
            Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y};
            auto _textureTileSizeOnScreen = Util::Math::worldToScreenSizeUI(_viewport, _textureTileSize);

            glm::vec4 _bottomLeftTextureCorner  = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = {  _textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner    = {  _textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };

            glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x                         , _textureOriginNorm.y                          };
            glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y                          };
            glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y };
            glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x                         , _textureOriginNorm.y + _textureTileSizeNorm.y };


            auto _uint32Color = Util::Math::colorToUint32_t(_data.RenderizableInnerData.color);

            _data.RenderizableInnerData.vertices[(_i * 4) + 0] = OpenGLVertex { _current9SliceMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _uint32Color };
            _data.RenderizableInnerData.vertices[(_i * 4) + 1] = OpenGLVertex { _current9SliceMat * _bottomRightTextureCorner, _bottomRightTextureCoord, _uint32Color };
            _data.RenderizableInnerData.vertices[(_i * 4) + 2] = OpenGLVertex { _current9SliceMat * _topRightTextureCorner   , _topRightTextureCoord   , _uint32Color };
            _data.RenderizableInnerData.vertices[(_i * 4) + 3] = OpenGLVertex { _current9SliceMat * _topLeftTextureCorner    , _topLeftTextureCoord    , _uint32Color };
        }
    }

    void calculatePartialHGeometry(RenderizableInnerDataUI& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {
        auto _uiT = (UITransform*)_transform;
        auto _originOffset = _data.originOffset;
        auto _screenPos = Util::Math::worldToScreenCoordsUI(_viewport, { _transformMatrix[3][0] + _originOffset.x, _transformMatrix[3][1] + _originOffset.y });

        _transformMatrix[3][0] = _screenPos.x;
        _transformMatrix[3][1] = _screenPos.y;

        auto _uiSizeScale = Vec2F { _uiT->getSize().x / _data.RenderizableInnerData.texture->getSize().x, _uiT->getSize().y / _data.RenderizableInnerData.texture->getSize().y };
        _transformMatrix[0][0] *= _uiSizeScale.x;
        _transformMatrix[1][1] *= _uiSizeScale.y;

        Vec2F _textureOrigin = {(float)_data.RenderizableInnerData.texture->getRegion().bottomLeftCorner.x, (float)_data.RenderizableInnerData.texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)_data.RenderizableInnerData.texture->getRegion().size.x, (float)_data.RenderizableInnerData.texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = { _textureTileSize.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y };
        auto _textureTileSizeOnScreen = Util::Math::worldToScreenSizeUI(_viewport, _textureTileSize);

        auto _partialRenderCorner = _textureTileSizeOnScreen.x * (1.f - _data.partialRenderingPercentage) * 2.f;
        auto _inv = _data.partialRenderingInverted;
        glm::vec4 _bottomLeftTextureCorner  = { -_textureTileSizeOnScreen.x + ( _inv ? _partialRenderCorner : 0), -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {  _textureTileSizeOnScreen.x - (!_inv ? _partialRenderCorner : 0), -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner    = {  _textureTileSizeOnScreen.x - (!_inv ? _partialRenderCorner : 0),  _textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x + ( _inv ? _partialRenderCorner : 0),  _textureTileSizeOnScreen.y, 0.0f, 1.0f };

        auto _partialRenderCoord = _textureTileSizeNorm.x * (1.f - _data.partialRenderingPercentage);
        glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x + (_inv ? _textureTileSizeNorm.x - _textureTileSizeNorm.x * _data.partialRenderingPercentage : 0), _textureOriginNorm.y };
        glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x - (!_inv ? _partialRenderCoord : 0)                                     , _textureOriginNorm.y };
        glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _textureTileSizeNorm.x - (!_inv ? _partialRenderCoord : 0)                                     , _textureOriginNorm.y + _textureTileSizeNorm.y };
        glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x + (_inv ? _textureTileSizeNorm.x - _textureTileSizeNorm.x * _data.partialRenderingPercentage : 0), _textureOriginNorm.y + _textureTileSizeNorm.y };

        auto _uint32Color = Util::Math::colorToUint32_t(_data.RenderizableInnerData.color);

        _data.RenderizableInnerData.vertices[0] = OpenGLVertex {_transformMatrix * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _uint32Color };
        _data.RenderizableInnerData.vertices[1] = OpenGLVertex {_transformMatrix * _bottomRightTextureCorner, _bottomRightTextureCoord, _uint32Color };
        _data.RenderizableInnerData.vertices[2] = OpenGLVertex {_transformMatrix * _topRightTextureCorner   , _topRightTextureCoord   , _uint32Color };
        _data.RenderizableInnerData.vertices[3] = OpenGLVertex {_transformMatrix * _topLeftTextureCorner    , _topLeftTextureCoord    , _uint32Color };
    }

    void calculatePartialVGeometry(RenderizableInnerDataUI& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {
        auto _uiT = (UITransform*)_transform;
        auto _originOffset = _data.originOffset;
        auto _screenPos = Util::Math::worldToScreenCoordsUI(_viewport, { _transformMatrix[3][0] + _originOffset.x, _transformMatrix[3][1] + _originOffset.y });

        _transformMatrix[3][0] = _screenPos.x;
        _transformMatrix[3][1] = _screenPos.y;

        auto _uiSizeScale = Vec2F { _uiT->getSize().x / _data.RenderizableInnerData.texture->getSize().x, _uiT->getSize().y / _data.RenderizableInnerData.texture->getSize().y };
        _transformMatrix[0][0] *= _uiSizeScale.x;
        _transformMatrix[1][1] *= _uiSizeScale.y;

        Vec2F _textureOrigin = {(float)_data.RenderizableInnerData.texture->getRegion().bottomLeftCorner.x, (float)_data.RenderizableInnerData.texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)_data.RenderizableInnerData.texture->getRegion().size.x, (float)_data.RenderizableInnerData.texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = { _textureTileSize.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y };
        auto _textureTileSizeOnScreen = Util::Math::worldToScreenSizeUI(_viewport, _textureTileSize);

        auto _partialRenderCorner = _textureTileSizeOnScreen.y * (1.f - _data.partialRenderingPercentage) * 2.f;
        auto _inv = _data.partialRenderingInverted;
        glm::vec4 _bottomLeftTextureCorner  = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y + (_inv ? _partialRenderCorner : 0), 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {  _textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y + (_inv ? _partialRenderCorner : 0), 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner    = {  _textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y - (!_inv ? _partialRenderCorner : 0), 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y - (!_inv ? _partialRenderCorner : 0), 0.0f, 1.0f };

        auto _partialRenderCoord = _textureTileSizeNorm.y * (1.f - _data.partialRenderingPercentage);
        glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x                         , _textureOriginNorm.y + (_inv ? _textureTileSizeNorm.y - _textureTileSizeNorm.y * _data.partialRenderingPercentage : 0) };
        glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + (_inv ? _textureTileSizeNorm.y - _textureTileSizeNorm.y * _data.partialRenderingPercentage : 0) };
        glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y - (!_inv ? _partialRenderCoord : 0)                                      };
        glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x                         , _textureOriginNorm.y + _textureTileSizeNorm.y - (!_inv ? _partialRenderCoord : 0)                                      };

        auto _uint32Color = Util::Math::colorToUint32_t(_data.RenderizableInnerData.color);

        _data.RenderizableInnerData.vertices[0] = OpenGLVertex {_transformMatrix * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _uint32Color };
        _data.RenderizableInnerData.vertices[1] = OpenGLVertex {_transformMatrix * _bottomRightTextureCorner, _bottomRightTextureCoord, _uint32Color };
        _data.RenderizableInnerData.vertices[2] = OpenGLVertex {_transformMatrix * _topRightTextureCorner   , _topRightTextureCoord   , _uint32Color };
        _data.RenderizableInnerData.vertices[3] = OpenGLVertex {_transformMatrix * _topLeftTextureCorner    , _topLeftTextureCoord    , _uint32Color };
    }

    void calculatePartialRGeometry(RenderizableInnerDataUI& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {
        auto _uiT = (UITransform*)_transform;
        auto _originOffset = _data.originOffset;
        auto _screenPos = Util::Math::worldToScreenCoordsUI(_viewport, { _transformMatrix[3][0] + _originOffset.x, _transformMatrix[3][1] + _originOffset.y });

        _transformMatrix[3][0] = _screenPos.x;
        _transformMatrix[3][1] = _screenPos.y;

        auto _uiSizeScale = Vec2F { _uiT->getSize().x / _data.RenderizableInnerData.texture->getSize().x, _uiT->getSize().y / _data.RenderizableInnerData.texture->getSize().y };
        _transformMatrix[0][0] *= _uiSizeScale.x;
        _transformMatrix[1][1] *= _uiSizeScale.y;

        Vec2F _textureOrigin = {(float)_data.RenderizableInnerData.texture->getRegion().bottomLeftCorner.x, (float)_data.RenderizableInnerData.texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureOrigin.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)_data.RenderizableInnerData.texture->getRegion().size.x, (float)_data.RenderizableInnerData.texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = { _textureTileSize.x / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().x, _textureTileSize.y / (float)_data.RenderizableInnerData.texture->getSpriteSheetSize().y };
        auto _textureTileSizeOnScreen = Util::Math::worldToScreenSizeUI(_viewport, _textureTileSize);

        // Bottom-Right square
        if(_data.partialRenderingPercentage > .75f) {

            // Multiply by 8 because we divide the whole rectangle into 8 parts
            float _v1 = 0.f;
            if(_data.partialRenderingPercentage == 1.f)    _v1 = 0;
            if(_data.partialRenderingPercentage < 1.f)     _v1 = (1.f - _data.partialRenderingPercentage) * _textureTileSizeOnScreen.x * 8.f;
            if(_data.partialRenderingPercentage <= 0.875f) _v1 = 0.125f * _textureTileSizeOnScreen.x * 8.f;

            float _v2 = 0.f;
            if(_data.partialRenderingPercentage == 0.875f) _v2 = 0;
            if(_data.partialRenderingPercentage < 0.875f)  _v2 = (0.875f - _data.partialRenderingPercentage) * _textureTileSizeOnScreen.y * 8.f;
            if(_data.partialRenderingPercentage <= 0.75f)  _v2 = 0.25f * _textureTileSizeOnScreen.y * 8.f;
            glm::vec4 _TLCorner = { 0                         , 0                                 , 0.0f, 1.0f };
            glm::vec4 _BLCorner = { _v1                       , -_textureTileSizeOnScreen.y + _v2 , 0.0f, 1.0f };
            glm::vec4 _BRCorner = { _textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y + _v2 , 0.0f, 1.0f };
            glm::vec4 _TRCorner = { _textureTileSizeOnScreen.x, 0                                 , 0.0f, 1.0f };

            // Multiply by 4 because we divide the rectangle in 8 parts, but the texture is already on the left.
            float _v1C = 0.f;
            if(_data.partialRenderingPercentage == 1.f)    _v1C = 0;
            if(_data.partialRenderingPercentage < 1.f)     _v1C = (1.f - _data.partialRenderingPercentage) * _textureTileSizeNorm.x * 4.f;
            if(_data.partialRenderingPercentage <= 0.875f) _v1C = 0.125f * _textureTileSizeNorm.x * 4.f;

            float _v2C = 0.f;
            if(_data.partialRenderingPercentage == 0.875f) _v2C = 0;
            if(_data.partialRenderingPercentage < 0.875f)  _v2C = (0.875f - _data.partialRenderingPercentage) * _textureTileSizeNorm.y * 4.f;
            if(_data.partialRenderingPercentage <= 0.75f)  _v2C = 0.25f * _textureTileSizeNorm.y * 4.f;
            glm::vec2 _TLCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x * 0.5f       , _textureOriginNorm.y + _textureTileSizeNorm.y * 0.5f };
            glm::vec2 _BLCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x * 0.5f + _v1C, _textureOriginNorm.y + _v2C                          };
            glm::vec2 _BRCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x              , _textureOriginNorm.y + _v2C                          };
            glm::vec2 _TRCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x              , _textureOriginNorm.y + _textureTileSizeNorm.y * 0.5f };

            auto _uint32Color = Util::Math::colorToUint32_t(_data.RenderizableInnerData.color);

            _data.RenderizableInnerData.vertices[0] = OpenGLVertex {_transformMatrix * _TLCorner, _TLCoord, _uint32Color };
            _data.RenderizableInnerData.vertices[1] = OpenGLVertex {_transformMatrix * _BLCorner, _BLCoord, _uint32Color };
            _data.RenderizableInnerData.vertices[2] = OpenGLVertex {_transformMatrix * _BRCorner, _BRCoord, _uint32Color };
            _data.RenderizableInnerData.vertices[3] = OpenGLVertex {_transformMatrix * _TRCorner, _TRCoord, _uint32Color };
        }

        // Top-Right square
        if(_data.partialRenderingPercentage > 0.5f) {
            // Multiply by 8 because we divide the whole rectangle into 8 parts
            float _v1 = 0.f;
            if(_data.partialRenderingPercentage == 0.75f)  _v1 = 0;
            if(_data.partialRenderingPercentage < 0.75f)   _v1 = (0.75f - _data.partialRenderingPercentage) * _textureTileSizeOnScreen.y * 8.f;
            if(_data.partialRenderingPercentage <= 0.625f) _v1 = 0.125f * _textureTileSizeOnScreen.y * 8.f;

            float _v2 = 0.f;
            if(_data.partialRenderingPercentage == 0.625f) _v2 = 0;
            if(_data.partialRenderingPercentage < 0.625f)  _v2 = (0.625f - _data.partialRenderingPercentage) * _textureTileSizeOnScreen.x * 8.f;
            if(_data.partialRenderingPercentage <= 0.5f)   _v2 = 0.25f * _textureTileSizeOnScreen.x * 8.f;

            glm::vec4 _bottomLeftTextureCorner  = { 0                               , 0                         , 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = { _textureTileSizeOnScreen.x - _v2, _v1                       , 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner    = { _textureTileSizeOnScreen.x - _v2, _textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner     = { 0                               , _textureTileSizeOnScreen.y, 0.0f, 1.0f };


            // Multiply by 4 because we divide the rectangle in 8 parts, but the texture is already on the left.
            float _v1C = 0.f;
            if(_data.partialRenderingPercentage == 0.75f)  _v1C = 0;
            if(_data.partialRenderingPercentage < 0.75f)   _v1C = (0.75f - _data.partialRenderingPercentage) * _textureTileSizeNorm.y * 4.f;
            if(_data.partialRenderingPercentage <= 0.625f) _v1C = 0.125f * _textureTileSizeNorm.y * 4.f;

            float _v2C = 0.f;
            if(_data.partialRenderingPercentage == 0.625f) _v2C = 0;
            if(_data.partialRenderingPercentage < 0.625f)  _v2C = (0.625f - _data.partialRenderingPercentage) * _textureTileSizeNorm.x * 4.f;
            if(_data.partialRenderingPercentage <= 0.5f)   _v2C = 0.25f * _textureTileSizeNorm.x * 4.f;

            glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x + _textureTileSizeNorm.x * 0.5f, _textureOriginNorm.y + _textureTileSizeNorm.y * 0.5f        };
            glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x - _v2C, _textureOriginNorm.y + _textureTileSizeNorm.y * 0.5f + _v1C };
            glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _textureTileSizeNorm.x - _v2C, _textureOriginNorm.y + _textureTileSizeNorm.y               };
            glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x + _textureTileSizeNorm.x * 0.5f, _textureOriginNorm.y + _textureTileSizeNorm.y               };

            auto _uint32Color = Util::Math::colorToUint32_t(_data.RenderizableInnerData.color);

            _data.RenderizableInnerData.vertices[4] = OpenGLVertex {_transformMatrix * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _uint32Color };
            _data.RenderizableInnerData.vertices[5] = OpenGLVertex {_transformMatrix * _bottomRightTextureCorner, _bottomRightTextureCoord, _uint32Color };
            _data.RenderizableInnerData.vertices[6] = OpenGLVertex {_transformMatrix * _topRightTextureCorner   , _topRightTextureCoord   , _uint32Color };
            _data.RenderizableInnerData.vertices[7] = OpenGLVertex {_transformMatrix * _topLeftTextureCorner    , _topLeftTextureCoord    , _uint32Color };
        }

        // Top-Left square
        if(_data.partialRenderingPercentage > 0.25f) {
            // Multiply by 8 because we divide the whole rectangle into 8 parts.
            float _v1 = 0.f;
            if(_data.partialRenderingPercentage == 0.5f)   _v1 = 0;
            if(_data.partialRenderingPercentage < 0.5f)    _v1 = (0.5f - _data.partialRenderingPercentage) * _textureTileSizeOnScreen.x * 8.f;
            if(_data.partialRenderingPercentage <= 0.375f) _v1 = 0.125f * _textureTileSizeOnScreen.x * 8.f;

            float _v2 = 0.f;
            if(_data.partialRenderingPercentage == 0.375f) _v2 = 0;
            if(_data.partialRenderingPercentage < 0.375f)  _v2 = (0.375f - _data.partialRenderingPercentage) * _textureTileSizeOnScreen.y * 8.f;
            if(_data.partialRenderingPercentage <= 0.125f) _v2 = 0.25f * _textureTileSizeOnScreen.y * 8.f;

            glm::vec4 _bottomLeftTextureCorner  = {  0                         , 0                                 , 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = { -_v1                       , _textureTileSizeOnScreen.y - _v2  , 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner    = { -_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y - _v2  , 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x, 0                                 , 0.0f, 1.0f };

            // Multiply by 4 because we divide the rectangle in 8 parts, but the texture is already on the left.
            float _v1C = 0.f;
            if(_data.partialRenderingPercentage == 0.5f)   _v1C = 0;
            if(_data.partialRenderingPercentage < 0.5f)    _v1C = (0.5f - _data.partialRenderingPercentage) * _textureTileSizeNorm.x * 4.f;
            if(_data.partialRenderingPercentage <= 0.375f) _v1C = 0.125f * _textureTileSizeNorm.x * 4.f;

            float _v2C = 0.f;
            if(_data.partialRenderingPercentage == 0.375f) _v2C = 0;
            if(_data.partialRenderingPercentage < 0.375f)  _v2C = (0.375f - _data.partialRenderingPercentage) * _textureTileSizeNorm.y * 4.f;
            if(_data.partialRenderingPercentage <= 0.125f) _v2C = 0.25f * _textureTileSizeNorm.y * 4.f;

            glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x + _textureTileSizeNorm.x * 0.5f       , _textureOriginNorm.y + _textureTileSizeNorm.y * 0.5f };
            glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x * 0.5f - _v1C, _textureOriginNorm.y + _textureTileSizeNorm.y - _v2C };
            glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x                                       , _textureOriginNorm.y + _textureTileSizeNorm.y - _v2C };
            glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x                                       , _textureOriginNorm.y + _textureTileSizeNorm.y * 0.5f };

            auto _uint32Color = Util::Math::colorToUint32_t(_data.RenderizableInnerData.color);

            _data.RenderizableInnerData.vertices[8]  = OpenGLVertex { _transformMatrix * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _uint32Color };
            _data.RenderizableInnerData.vertices[9]  = OpenGLVertex { _transformMatrix * _bottomRightTextureCorner, _bottomRightTextureCoord, _uint32Color };
            _data.RenderizableInnerData.vertices[10] = OpenGLVertex { _transformMatrix * _topRightTextureCorner   , _topRightTextureCoord   , _uint32Color };
            _data.RenderizableInnerData.vertices[11] = OpenGLVertex { _transformMatrix * _topLeftTextureCorner    , _topLeftTextureCoord    , _uint32Color };
        }

        // Bottom-Left square
        if(_data.partialRenderingPercentage > 0) {
            // Multiply by 8 because we divide the whole rectangle into 8 parts
            float _v1 = 0.f;
            if(_data.partialRenderingPercentage == 0.25f)   _v1 = 0;
            if(_data.partialRenderingPercentage < 0.25)     _v1 = (0.25f - _data.partialRenderingPercentage) * _textureTileSizeOnScreen.y * 8.f;
            if(_data.partialRenderingPercentage <= 0.125f)  _v1 = 0.125f * _textureTileSizeOnScreen.y * 8.f;

            float _v2 = 0.f;
            if(_data.partialRenderingPercentage == 0.125) _v2 = 0;
            if(_data.partialRenderingPercentage < 0.125)  _v2 = (0.125f - _data.partialRenderingPercentage) * _textureTileSizeOnScreen.x * 8.f;
            if(_data.partialRenderingPercentage <= 0.f)   _v2 = 0.25f * _textureTileSizeOnScreen.x * 8.f;

            glm::vec4 _bottomLeftTextureCorner  = {  0                               ,  0                                 , 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = { -_textureTileSizeOnScreen.x + _v2, -_v1                               , 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner    = { -_textureTileSizeOnScreen.x + _v2, -_textureTileSizeOnScreen.y        , 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner     = {  0                               , -_textureTileSizeOnScreen.y        , 0.0f, 1.0f };

            // Multiply by 4 because we divide the rectangle in 8 parts, but the texture is already on the left.
            float _v1C = 0.f;
            if(_data.partialRenderingPercentage == 0.25f)   _v1C = 0;
            if(_data.partialRenderingPercentage < 0.25)     _v1C = (0.25f - _data.partialRenderingPercentage) * _textureTileSizeNorm.y * 4.f;
            if(_data.partialRenderingPercentage <= 0.125f)  _v1C = 0.125f * _textureTileSizeNorm.y * 4.f;

            float _v2C = 0.f;
            if(_data.partialRenderingPercentage == 0.125) _v2C = 0;
            if(_data.partialRenderingPercentage < 0.125)  _v2C = (0.125f - _data.partialRenderingPercentage) * _textureTileSizeNorm.x * 4.f;
            if(_data.partialRenderingPercentage <= 0.f)   _v2C = 0.25f * _textureTileSizeNorm.x * 4.f;

            glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x + _textureTileSizeNorm.x * 0.5f, _textureOriginNorm.y + _textureTileSizeNorm.y * 0.5f        };
            glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _v2C                         , _textureOriginNorm.y + _textureTileSizeNorm.y * 0.5f - _v1C };
            glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _v2C                         , _textureOriginNorm.y                                        };
            glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x + _textureTileSizeNorm.x * 0.5f, _textureOriginNorm.y                                        };

            auto _uint32Color = Util::Math::colorToUint32_t(_data.RenderizableInnerData.color);

            _data.RenderizableInnerData.vertices[12] = OpenGLVertex {_transformMatrix * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _uint32Color };
            _data.RenderizableInnerData.vertices[13] = OpenGLVertex {_transformMatrix * _bottomRightTextureCorner, _bottomRightTextureCoord, _uint32Color };
            _data.RenderizableInnerData.vertices[14] = OpenGLVertex {_transformMatrix * _topRightTextureCorner   , _topRightTextureCoord   , _uint32Color };
            _data.RenderizableInnerData.vertices[15] = OpenGLVertex {_transformMatrix * _topLeftTextureCorner    , _topLeftTextureCoord    , _uint32Color };
        }
    }

    void calculateGeometryForUIImage(RenderizableInnerDataUI& _data, glm::mat4& _transformMatrix, Transform* _transform, const ViewPort* _viewport) {
        switch ((ImageRenderingType)_data.imageRenderingType) {
                case NORMAL:
                    calculateNormalGeometry(_data, _transformMatrix, _transform, _viewport); break;
                case NINE_SLICE:
                    calculate9SliceGeometry(_data, _transformMatrix, _transform, _viewport); break;
                case PARTIAL_VERTICAL:
                    calculatePartialVGeometry(_data, _transformMatrix, _transform, _viewport); break;
                case PARTIAL_HORIZONTAL:
                    calculatePartialHGeometry(_data, _transformMatrix, _transform, _viewport); break;
                case PARTIAL_RADIAL:
                    calculatePartialRGeometry(_data, _transformMatrix, _transform, _viewport); break;
            }

    }


    void batchFourVertexGeometry(RenderizableInnerDataUI& _data,std::vector<OpenGLVertex>& _vertices) {
        auto _vertexCount = _vertices.size();

        _vertices.emplace_back(_data.RenderizableInnerData.vertices[0]);
        _vertices.emplace_back(_data.RenderizableInnerData.vertices[1]);
        _vertices.emplace_back(_data.RenderizableInnerData.vertices[2]);

        _vertices.emplace_back(_data.RenderizableInnerData.vertices[2]);
        _vertices.emplace_back(_data.RenderizableInnerData.vertices[3]);
        _vertices.emplace_back(_data.RenderizableInnerData.vertices[0]);
    }

    void batch9SliceVertexGeometry(RenderizableInnerDataUI& _data, std::vector<OpenGLVertex>& _vertices) {
        for(auto _i = 0; _i < 9; _i++) {
            auto _vertexCount = _vertices.size();

            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 0]);
            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 1]);
            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 2]);

            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 2]);
            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 3]);
            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 0]);
        }
    }

    void batchPartialCircularVertexGeometry(RenderizableInnerDataUI& _data, std::vector<OpenGLVertex>& _vertices) {
        auto _squares = 0;

        if(_data.partialRenderingPercentage <= 0.75f) _squares++;
        if(_data.partialRenderingPercentage <= 0.5f)  _squares++;
        if(_data.partialRenderingPercentage <= 0.25f) _squares++;
        if(_data.partialRenderingPercentage <= 0.f)   _squares++;

        for(auto _i = 3; _i >= _squares; _i--) {
            auto _vertexCount = _vertices.size();

            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 0]);
            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 1]);
            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 2]);

            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 2]);
            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 3]);
            _vertices.push_back(_data.RenderizableInnerData.vertices[(_i * 4) + 0]);
        }
    }

    void drawBatchedUIImage(RenderizableInnerDataUI& _data, Batch* _batch, Transform* _transform, const ViewPort* _viewport) {
        auto [_transformMat, _dirty] = _transform->localToWorld();
        if(_dirty || _data.RenderizableInnerData.dirty) {
            switch (_data.imageRenderingType) {
                case NORMAL:
                    calculateNormalGeometry(_data, _transformMat, _transform, _viewport); break;
                case NINE_SLICE:
                    calculate9SliceGeometry(_data, _transformMat, _transform, _viewport); break;
                case PARTIAL_VERTICAL:
                    calculatePartialVGeometry(_data, _transformMat, _transform, _viewport); break;
                case PARTIAL_HORIZONTAL:
                    calculatePartialHGeometry(_data, _transformMat, _transform, _viewport); break;
                case PARTIAL_RADIAL:
                    calculatePartialRGeometry(_data, _transformMat, _transform, _viewport); break;
            }

            _data.RenderizableInnerData.dirty = false;
        }

        switch ((ImageRenderingType)_data.imageRenderingType) {
            case NORMAL:
            case PARTIAL_VERTICAL:
            case PARTIAL_HORIZONTAL:
                batchFourVertexGeometry(_data, _batch->vertexBuffer); break;
            case NINE_SLICE:
                batch9SliceVertexGeometry(_data, _batch->vertexBuffer); break;
            case PARTIAL_RADIAL:
                batchPartialCircularVertexGeometry(_data, _batch->vertexBuffer); break;
        }
    }


}