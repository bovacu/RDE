//
// Created by borja on 9/5/22.
//

#include "core/graph/components/NineSliceSprite.h"
#include "core/util/Functions.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/Engine.h"

namespace GDE {

    NineSliceSprite::NineSliceSprite(Node* _node, Scene* _scene, Canvas* _canvas, Texture* _texture) :
    NineSliceSprite(_node, &_scene->engine->manager, _canvas->getGraph(), _texture) {  }

    NineSliceSprite::NineSliceSprite(Node* _node, Manager* _manager, Graph* _graph, Texture* _texture) : UI(_node) {
        shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        texture = _texture;

        if(!_texture->nineSlice.isEnabled()) {
            spriteRenderer = _node->addComponent<UIImage>(_texture);
        }

        nineSliceSize = _texture->getRegion().size;
        IRenderizable::batchPriority = BatchPriority::SpritePriority;

        auto [_transformMat, _] = _node->getTransform()->localToWorld();
        calculateGeometry(_transformMat, *_node->getTransform(), *_manager->sceneManager.getDisplayedScene()->getMainCamera()->getViewport());
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

        auto [_transformMat, _dirty] = _transform.localToWorld();
        if(_dirty || dirty) {
            calculateGeometry(_transformMat, _transform, _viewport);
            dirty = true;
        }

        for(auto _i = 0; _i < 9; _i++) {
            auto _vertexCount = _vertices.size();

            _vertices.push_back(geometry[(_i * 4) + 0]);
            _vertices.push_back(geometry[(_i * 4) + 1]);
            _vertices.push_back(geometry[(_i * 4) + 2]);
            _vertices.push_back(geometry[(_i * 4) + 3]);

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

    void NineSliceSprite::setInteractable(bool _interactable) {
        interaction->interactable = _interactable;

        if(!interaction->interactable) {
            color = Color::Disabled;
        } else {
            color = color == Color::Disabled ? Color::White : color;
        }
    }

    bool NineSliceSprite::isInteractable() {
        return interaction->interactable;
    }

    void NineSliceSprite::setSize(const Vec2F& _size) {
        nineSliceSize = _size;
        if(nineSliceSize.x < 0) nineSliceSize.x = 0;
        if(nineSliceSize.y < 0) nineSliceSize.y = 0;
        dirty = true;
    }

    void NineSliceSprite::calculateGeometry(glm::mat4& _transformMat, Transform& _transform, const IViewPort& _viewport) {
        auto _rectsAmount = *(&texture->nineSlice.subRects + 1) - texture->nineSlice.subRects;
        auto _pivot = ((UITransform*)&_transform)->getPivot();
        for(auto _i = 0; _i < _rectsAmount; _i++) {
//            if(_i > 0) continue;
            auto& _subTextureRegion = texture->nineSlice.subRects[_i];

            float _distortX = 1.f, _distortY = 1.f;
            auto _uiSize = nineSliceSize;
            auto _spriteSize = texture->getRegion().size;

            auto _bottomLeftCornerLocal = Vec2F { (float)(_subTextureRegion.bottomLeftCorner.x - texture->nineSlice.subRects[0].bottomLeftCorner.x), (float)(_subTextureRegion.bottomLeftCorner.y - texture->nineSlice.subRects[0].bottomLeftCorner.y) };
            Vec2F _position = { -(float)_spriteSize.x / 2.f + (float)_bottomLeftCornerLocal.x + (float)_subTextureRegion.size.x / 2.f, -(float)((float)_spriteSize.y / 2.f - _bottomLeftCornerLocal.y) + (float)_subTextureRegion.size.y / 2.f};
            Vec2F _scale = { 1, 1 };
            Vec2F _noResizableScale = { 1, 1 };

            if((float)_uiSize.x - (float)_spriteSize.x != 0) {
                auto _widthOfCorners = (float) texture->nineSlice.subRects[0].size.x + (float) texture->nineSlice.subRects[2].size.x;
                auto _totalDiffX = (float)_uiSize.x - _widthOfCorners;
                _distortX = _totalDiffX / ((float)_spriteSize.x - _widthOfCorners);
                if(_distortX < 0) _distortX = 0;
                _scale.x *= _distortX;

                auto _halfWidthOfDistortedMiddleRect = _distortX * _transform.getScale().y * (float)texture->nineSlice.subRects[1].size.x / 2.f;
                auto _halfWidthOfOriginalMiddleRect = (float)texture->nineSlice.subRects[1].size.x / 2.f;
                if(_i == 0 || _i == 3 || _i == 6) _position.x -= _halfWidthOfDistortedMiddleRect - _halfWidthOfOriginalMiddleRect;
                if(_i == 2 || _i == 5 || _i == 8) _position.x += _halfWidthOfDistortedMiddleRect - _halfWidthOfOriginalMiddleRect;
            }

            if((float)_uiSize.y - (float)_spriteSize.y != 0) {
                auto _heightOfCorners = (float)texture->nineSlice.subRects[0].size.y + (float)texture->nineSlice.subRects[6].size.y;
                auto _totalDiffY = (float)_uiSize.y - _heightOfCorners;
                _distortY = _totalDiffY / ((float)_spriteSize.y - _heightOfCorners);
                if(_distortY < 0) _distortY = 0;
                _scale.y *= _distortY;

                auto _halfHeightOfDistortedMiddleRect = _distortY * _transform.getScale().y * (float)texture->nineSlice.subRects[3].size.y / 2.f;
                auto _halfHeightOfOriginalMiddleRect = (float)texture->nineSlice.subRects[3].size.y / 2.f;
                if(_i == 0 || _i == 1 || _i == 2) _position.y -= _halfHeightOfDistortedMiddleRect - _halfHeightOfOriginalMiddleRect;
                if(_i == 6 || _i == 7 || _i == 8) _position.y += _halfHeightOfDistortedMiddleRect - _halfHeightOfOriginalMiddleRect;
            }

            float _sizeOfXCorners = (float)texture->nineSlice.subRects[0].size.x + (float)texture->nineSlice.subRects[2].size.x;
            float _propX = _uiSize.x / _sizeOfXCorners;
            if(_propX < 0) _propX = 0.f;
            float _toMoveX = ((float)_subTextureRegion.size.x - ((float)_subTextureRegion.size.x * _propX)) * 0.5f;

            float _sizeOfYCorners = (float)texture->nineSlice.subRects[0].size.y + (float)texture->nineSlice.subRects[6].size.y;
            float _propY = _uiSize.y / _sizeOfYCorners;
            if(_propY < 0) _propY = 0.f;
            float _toMoveY = ((float)_subTextureRegion.size.y - ((float)_subTextureRegion.size.y * _propY)) * 0.5f;

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

            _current9SliceMat = _transformMat * _current9SliceMat;

            auto _subTextureReposition = Vec2F { _uiSize.x / 2.f - ((float)_subTextureRegion.size.x * 0.5f - (float)_subTextureRegion.size.x * _pivot.x) * _current9SliceMat[0][0], _uiSize.y / 2.f - ((float)_subTextureRegion.size.y * 0.5f - (float)_subTextureRegion.size.y * _pivot.y) * _current9SliceMat[1][1] };
            auto _screenPos = Util::worldToScreenCoords(_viewport, {_current9SliceMat[3][0] + _subTextureReposition.x - _uiSize.x * _pivot.x, _current9SliceMat[3][1] + _subTextureReposition.y - _uiSize.y * _pivot.y });
            _current9SliceMat[3][0] = _screenPos.x;
            _current9SliceMat[3][1] = _screenPos.y;


            Vec2F _textureOrigin = {(float)_subTextureRegion.bottomLeftCorner.x, (float)_subTextureRegion.bottomLeftCorner.y};
            Vec2F _textureOriginNorm = {_textureOrigin.x / (float)texture->getSpriteSheetSize().x, _textureOrigin.y / (float)texture->getSpriteSheetSize().y};

            Vec2F _textureTileSize = {(float)_subTextureRegion.size.x, (float)_subTextureRegion.size.y};
            Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)texture->getSpriteSheetSize().x, _textureTileSize.y / (float)texture->getSpriteSheetSize().y};
            auto _textureTileSizeOnScreen = Util::worldToScreenSize(_viewport, _textureTileSize);

            glm::vec4 _bottomLeftTextureCorner  = { -_textureTileSizeOnScreen.x * _pivot.x * 2.f                                   , -_textureTileSizeOnScreen.y * _pivot.y * 2.f                                   , 0.0f, 1.0f };
            glm::vec4 _bottomRightTextureCorner = {  _textureTileSizeOnScreen.x * 2.f - _textureTileSizeOnScreen.x * _pivot.x * 2.f, -_textureTileSizeOnScreen.y * _pivot.y * 2.f                                   , 0.0f, 1.0f };
            glm::vec4 _topRightTextureCorner    = {  _textureTileSizeOnScreen.x * 2.f - _textureTileSizeOnScreen.x * _pivot.x * 2.f,  _textureTileSizeOnScreen.y * 2.f - _textureTileSizeOnScreen.y * _pivot.y * 2.f, 0.0f, 1.0f };
            glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x * _pivot.x * 2.f                                   ,  _textureTileSizeOnScreen.y * 2.f - _textureTileSizeOnScreen.y * _pivot.y * 2.f, 0.0f, 1.0f };

            glm::vec4 _color = { (float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f, (float)color.a / 255.f };

            glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x                         , _textureOriginNorm.y };
            glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y };
            glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y };
            glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x                         , _textureOriginNorm.y + _textureTileSizeNorm.y };

            geometry[(_i * 4) + 0] = OpenGLVertex {_current9SliceMat * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _color };
            geometry[(_i * 4) + 1] = OpenGLVertex {_current9SliceMat * _bottomRightTextureCorner, _bottomRightTextureCoord, _color };
            geometry[(_i * 4) + 2] = OpenGLVertex {_current9SliceMat * _topRightTextureCorner   , _topRightTextureCoord   , _color };
            geometry[(_i * 4) + 3] = OpenGLVertex {_current9SliceMat * _topLeftTextureCorner    , _topLeftTextureCoord    , _color };
        }
    }

}