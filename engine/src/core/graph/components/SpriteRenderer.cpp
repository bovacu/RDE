//
// Created by borja on 9/5/22.
//

#include "core/graph/components/SpriteRenderer.h"
#include "core/render/elements/ViewPort.h"
#include "core/util/Color.h"
#include "core/util/Functions.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/Vertex.h"
#include "core/render/elements/Batch.h"
#include "core/graph/Scene.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/Engine.h"

namespace RDE {

    SpriteRenderer::SpriteRenderer(Node* _node, Scene* _scene, Texture* _texture) :
    SpriteRenderer(_node, &_scene->engine->manager, _scene->getMainGraph(), _texture) {  }

    SpriteRenderer::SpriteRenderer(Node* _node, Scene* _scene, Canvas* _canvas, Texture* _texture) :
    SpriteRenderer(_node, &_scene->engine->manager, _canvas->getGraph(), _texture)  {  }

    SpriteRenderer::SpriteRenderer(Node* _node, Manager* _manager, Graph* _graph, Texture* _texture) : IRenderizable(_node) {
        shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        IRenderizable::batchPriority = BatchPriority::SpritePriority;

        if(_texture == nullptr) {
            texture = _manager->textureManager.getSubTexture("defaultAssets", "sprite");
        } else {
            texture = _texture;
        }

        auto [_transformMat, _] = _node->getTransform()->localToWorld();
        calculateGeometry(_transformMat, *_node->getTransform(), *_manager->sceneManager.getDisplayedScene()->getMainCamera()->getViewport());
    }

    std::string SpriteRenderer::getTexturePath() {
        return texture->getPath();
    }

    std::string SpriteRenderer::getTextureName() {
        return Util::String::getFileNameFromPath(texture->getPath());
    }

    std::string SpriteRenderer::getTextureExtension() {
        return Util::String::getFileExtension(texture->getPath());
    }

    void SpriteRenderer::calculateGeometry(glm::mat4& _transformMatrix, Transform& _transform, const ViewPort& _viewport) {
        auto _screenPos = Util::Math::worldToScreenCoords(_viewport, {_transformMatrix[3][0], _transformMatrix[3][1]});
        _transformMatrix[3][0] = _screenPos.x;
        _transformMatrix[3][1] = _screenPos.y;

        Vec2F _textureOrigin = {(float)texture->getRegion().bottomLeftCorner.x, (float)texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)texture->getSpriteSheetSize().x, _textureOrigin.y / (float)texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)texture->getRegion().size.x, (float)texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)texture->getSpriteSheetSize().x, _textureTileSize.y / (float)texture->getSpriteSheetSize().y};
        auto _textureTileSizeOnScreen = Util::Math::worldToScreenSize(_viewport, _textureTileSize);

        glm::vec4 _bottomLeftTextureCorner  = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {  _textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner    = {  _textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner     = { -_textureTileSizeOnScreen.x,  _textureTileSizeOnScreen.y, 0.0f, 1.0f };

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

    void SpriteRenderer::drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const ViewPort& _viewport) {
        auto _vertexCount = _vertices.size();

        auto [_transformMat, _dirty] = _transform.localToWorld();
        if(_dirty || dirty) {
            calculateGeometry(_transformMat, _transform, _viewport);
            dirty = false;
        }

        _vertices.emplace_back(geometry[0]);
        _vertices.emplace_back(geometry[1]);
        _vertices.emplace_back(geometry[2]);
        _vertices.emplace_back(geometry[3]);

        _indices.emplace_back(_vertexCount + 0);
        _indices.emplace_back(_vertexCount + 1);
        _indices.emplace_back(_vertexCount + 2);

        _indices.emplace_back(_vertexCount + 2);
        _indices.emplace_back(_vertexCount + 3);
        _indices.emplace_back(_vertexCount + 0);
    }

}