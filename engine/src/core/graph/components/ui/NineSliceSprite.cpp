//
// Created by borja on 9/5/22.
//

#include "core/graph/components/NineSliceSprite.h"
#include "core/util/Functions.h"
#include "core/graph/components/SpriteRenderer.h"

namespace GDE {

    NineSliceSprite::NineSliceSprite(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, Texture* _texture) : UI(_nodeID, _canvas) {
        shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        texture = _texture;

        if(!_texture->nineSlice.isEnabled()) {
            spriteRenderer = _canvas->getGraph()->addComponent<SpriteRenderer>(_nodeID, _scene, _canvas, _texture);
        }

        nineSliceSize = _texture->getRegion().size;
        IRenderizable::batchPriority = BatchPriority::SpritePriority;
    }

    NineSlice& NineSliceSprite::getNineSlice() const {
        return texture->nineSlice;
    }

    void NineSliceSprite::drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) {
        if(!enabled) return;

        if(spriteRenderer) {
            spriteRenderer->drawBatched(_vertices, _indices, _transform, _viewport);
            return;
        }

        auto _rectsAmount = *(&texture->nineSlice.subRects + 1) - texture->nineSlice.subRects;
        for(auto _i = 0; _i < _rectsAmount; _i++) {
            auto _vertexCount = _vertices.size();
            auto& _subTextureRegion = texture->nineSlice.subRects[_i];
            auto [_transformMat, _dirty] = _transform.localToWorld();

            float _distortX = 1.f, _distortY = 1.f;
            auto _uiSize = nineSliceSize;
            auto _spriteSize = texture->getRegion().size;

            auto _bottomLeftCornerLocal = Vec2F { (float)(_subTextureRegion.bottomLeftCorner.x - texture->nineSlice.subRects[0].bottomLeftCorner.x), (float)(_subTextureRegion.bottomLeftCorner.y - texture->nineSlice.subRects[0].bottomLeftCorner.y) };
            Vec2F _position = {_transform.getPosition().x - (float)_spriteSize.x / 2.f + (float)_bottomLeftCornerLocal.x + (float)_subTextureRegion.size.x / 2.f, _transform.getPosition().y - (float)((float)_spriteSize.y / 2.f - _bottomLeftCornerLocal.y) + (float)_subTextureRegion.size.y / 2.f};

            if((float)_uiSize.x - (float)_spriteSize.x != 0) {
                auto _widthOfCorners = (float) texture->nineSlice.subRects[0].size.x + (float) texture->nineSlice.subRects[2].size.x;
                auto _totalDiffX = (float)_uiSize.x - _widthOfCorners;
                _distortX = _totalDiffX / ((float)_spriteSize.x - _widthOfCorners);

                auto _halfWidthOfDistortedMiddleRect = _distortX * _transformMat[0][0] * (float)texture->nineSlice.subRects[1].size.x / 2.f;
                auto _halfWidthOfOriginalMiddleRect = (float)texture->nineSlice.subRects[1].size.x / 2.f;
                if(_i == 0 || _i == 3 || _i == 6) _position.x -= _halfWidthOfDistortedMiddleRect - _halfWidthOfOriginalMiddleRect;
                if(_i == 2 || _i == 5 || _i == 8) _position.x += _halfWidthOfDistortedMiddleRect - _halfWidthOfOriginalMiddleRect;
            }

            if((float)_uiSize.y - (float)_spriteSize.y != 0) {
                auto _heightOfCorners = (float)texture->nineSlice.subRects[0].size.y + (float)texture->nineSlice.subRects[6].size.y;
                auto _totalDiffY = (float)_uiSize.y - _heightOfCorners;
                _distortY = _totalDiffY / ((float)_spriteSize.y - _heightOfCorners);

                auto _halfHeightOfDistortedMiddleRect = _distortY * _transformMat[1][1] * (float)texture->nineSlice.subRects[3].size.y / 2.f;
                auto _halfHeightOfOriginalMiddleRect = (float)texture->nineSlice.subRects[3].size.y / 2.f;
                if(_i == 0 || _i == 1 || _i == 2) _position.y -= _halfHeightOfDistortedMiddleRect - _halfHeightOfOriginalMiddleRect;
                if(_i == 6 || _i == 7 || _i == 8) _position.y += _halfHeightOfDistortedMiddleRect - _halfHeightOfOriginalMiddleRect;
            }

            auto _screenPos = Util::worldToScreenCoords(_viewport, {_position.x, _position.y});
            _transformMat[3][0] = _screenPos.x;
            _transformMat[3][1] = _screenPos.y;
            if(_i == 1 || _i == 4 || _i == 7) _transformMat[0][0] *= _distortX;
            if(_i == 3 || _i == 4 || _i == 5) _transformMat[1][1] *= _distortY;


            Vec2F _textureOrigin = {(float)_subTextureRegion.bottomLeftCorner.x, (float)_subTextureRegion.bottomLeftCorner.y};
            Vec2F _textureOriginNorm = {_textureOrigin.x / (float)texture->getSpriteSheetSize().x, _textureOrigin.y / (float)texture->getSpriteSheetSize().y};

            Vec2F _textureTileSize = {(float)_subTextureRegion.size.x, (float)_subTextureRegion.size.y};
            Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)texture->getSpriteSheetSize().x, _textureTileSize.y / (float)texture->getSpriteSheetSize().y};
            auto _textureTileSizeOnScreen = Util::worldToScreenSize(_viewport, _textureTileSize);

            glm::vec4 _bottomLeftTextureCorner  = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = {  _textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner    = {  _textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };

            glm::vec4 _color = { (float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f, (float)color.a / 255.f };

            glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x                         , _textureOriginNorm.y };
            glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y };
            glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y };
            glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x                         , _textureOriginNorm.y + _textureTileSizeNorm.y };

            _vertices.emplace_back(OpenGLVertex {_transformMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _color });
            _vertices.emplace_back(OpenGLVertex {_transformMat * _bottomRightTextureCorner, _bottomRightTextureCoord, _color });
            _vertices.emplace_back(OpenGLVertex {_transformMat * _topRightTextureCorner   , _topRightTextureCoord   , _color });
            _vertices.emplace_back(OpenGLVertex {_transformMat * _topLeftTextureCorner    , _topLeftTextureCoord    , _color });

            _indices.emplace_back(_vertexCount + 0);
            _indices.emplace_back(_vertexCount + 1);
            _indices.emplace_back(_vertexCount + 2);

            _indices.emplace_back(_vertexCount + 2);
            _indices.emplace_back(_vertexCount + 3);
            _indices.emplace_back(_vertexCount + 0);
        }
    }

    void NineSliceSprite::drawAndFlush(std::vector<DrawAndFlushData>& _data, Transform& _transform, const IViewPort& _viewport) {
        if(!enabled) return;

        DrawAndFlushData _nineSliceData;
        _nineSliceData.textureID = getTexture();
        _nineSliceData.shaderID = shaderID;
        drawBatched(_nineSliceData.vertices, _nineSliceData.indices, _transform, _viewport);
        _data.push_back(_nineSliceData);
    }

}