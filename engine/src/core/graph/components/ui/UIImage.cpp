//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/Engine.h"

namespace GDE {


    UIImage::UIImage(Node* _node, Scene* _scene, Canvas* _canvas, Texture* _texture) :
    UIImage(_node, &_scene->engine->manager, _canvas->getGraph(), _texture) {  }

    UIImage::UIImage(Node* _node, Manager* _manager, Graph* _graph, Texture* _texture) :
    SpriteRenderer(_node, _manager, _graph, _texture) {  }

    void UIImage::calculateGeometry(glm::mat4& _transformMatrix, Transform& _transform, const IViewPort& _viewport) {
        auto _pivot = ((UITransform*)&_transform)->getPivot();
        auto _screenPos = Util::worldToScreenCoords(_viewport, {_transformMatrix[3][0],
                                                                _transformMatrix[3][1]
        });

        _transformMatrix[3][0] = _screenPos.x;
        _transformMatrix[3][1] = _screenPos.y;

        Vec2F _textureOrigin = {(float)texture->getRegion().bottomLeftCorner.x, (float)texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)texture->getSpriteSheetSize().x, _textureOrigin.y / (float)texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)texture->getRegion().size.x, (float)texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)texture->getSpriteSheetSize().x, _textureTileSize.y / (float)texture->getSpriteSheetSize().y};
        auto _textureTileSizeOnScreen = Util::worldToScreenSize(_viewport, _textureTileSize);

        //      SpriteRenderer              UIImage
        //  V3(-.5,.5)    V2(.5,.5)      V3(0,1)    V2(1,1)
        //      +---------+                 +---------+
        //      |         |                 |         |
        //      |    C    | C(0,0)          |         | C(0,0)
        //      |         |                 |         |
        //      +---------+                 C---------+
        //  V0(-.5,-.5)   V1(.5,-.5)     V0(0,0)    V1(1,0)
        glm::vec4 _bottomLeftTextureCorner  = { -_textureTileSizeOnScreen.x * _pivot.x * 2.f                                   , -_textureTileSizeOnScreen.y * _pivot.y * 2.f                                  , 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {  _textureTileSizeOnScreen.x * 2.f - _textureTileSizeOnScreen.x * _pivot.x * 2.f, -_textureTileSizeOnScreen.y * _pivot.y * 2.f                                  , 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner    = {  _textureTileSizeOnScreen.x * 2.f - _textureTileSizeOnScreen.x * _pivot.x * 2.f, _textureTileSizeOnScreen.y * 2.f - _textureTileSizeOnScreen.y * _pivot.y * 2.f, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x * _pivot.x * 2.f                                   , _textureTileSizeOnScreen.y * 2.f - _textureTileSizeOnScreen.y * _pivot.y * 2.f, 0.0f, 1.0f };

        glm::vec2 _bottomLeftTextureCoord   = { _textureOriginNorm.x                         , _textureOriginNorm.y };
        glm::vec2 _bottomRightTextureCoord  = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y };
        glm::vec2 _topRightTextureCoord     = { _textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y };
        glm::vec2 _topLeftTextureCoord      = { _textureOriginNorm.x                         , _textureOriginNorm.y + _textureTileSizeNorm.y };

        glm::vec4 _color = { (float)color.r / 255.f, (float)color.g/ 255.f,(float)color.b/ 255.f, (float)color.a/ 255.f };

        geometry[0] = OpenGLVertex {_transformMatrix * _bottomLeftTextureCorner , _bottomLeftTextureCoord , _color };
        geometry[1] = OpenGLVertex {_transformMatrix * _bottomRightTextureCorner, _bottomRightTextureCoord, _color };
        geometry[2] = OpenGLVertex {_transformMatrix * _topRightTextureCorner   , _topRightTextureCoord   , _color };
        geometry[3] = OpenGLVertex {_transformMatrix * _topLeftTextureCorner    , _topLeftTextureCoord    , _color };
    }
}