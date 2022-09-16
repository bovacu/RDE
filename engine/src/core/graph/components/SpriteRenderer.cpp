//
// Created by borja on 9/5/22.
//

#include "core/graph/components/SpriteRenderer.h"
#include "core/render/Camera.h"
#include "core/util/Functions.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/Vertex.h"
#include "core/render/elements/Batch.h"
#include "core/graph/Scene.h"

namespace GDE {

    SpriteRenderer::SpriteRenderer(const NodeID& _nodeId, Transform* _transform, Scene* _scene, Texture* _texture) : IRenderizable(_transform), texture(_texture) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic")->getShaderID();
        IRenderizable::batchPriority = BatchPriority::SpritePriority;
    }

    SpriteRenderer::SpriteRenderer(const NodeID& _nodeId, Transform* _transform, Scene* _scene) : IRenderizable(_transform) {
        shaderID = _scene->engine->manager.shaderManager.getShader("basic")->getShaderID();
        IRenderizable::batchPriority = BatchPriority::SpritePriority;
    }

    std::string SpriteRenderer::getTexturePath() {
        return texture->getPath();
    }

    std::string SpriteRenderer::getTextureName() {
        return Util::getFileNameFromPath(texture->getPath());
    }

    std::string SpriteRenderer::getTextureExtension() {
        return Util::getFileExtension(texture->getPath());
    }

    void SpriteRenderer::draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, const Transform& _transform, const IViewPort& _viewport) const {
        auto _vertexCount = _vertices.size();

        auto _transformMat = _transform.modelMatrix;
        auto _screenPos = Util::worldToScreenCoords(_viewport, {_transform.modelMatrix[3][0], _transform.modelMatrix[3][1]});
        _transformMat[3][0] = _screenPos.x;
        _transformMat[3][1] = _screenPos.y;

        Vec2F _textureOrigin = {(float)texture->getRegion().bottomLeftCorner.x, (float)texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)texture->getSpriteSheetSize().x, _textureOrigin.y / (float)texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)texture->getRegion().size.x, (float)texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)texture->getSpriteSheetSize().x, _textureTileSize.y / (float)texture->getSpriteSheetSize().y};
        auto _textureTileSizeOnScreen = Util::worldToScreenSize(_viewport, _textureTileSize);

        glm::vec4 _bottomLeftTextureCorner = { -_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _bottomRightTextureCorner = {_textureTileSizeOnScreen.x, -_textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topRightTextureCorner = {_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };
        glm::vec4 _topLeftTextureCorner = {-_textureTileSizeOnScreen.x, _textureTileSizeOnScreen.y, 0.0f, 1.0f };

        glm::vec4 _color = { (float)color.r / 255.f, (float)color.g/ 255.f,(float)color.b/ 255.f, (float)color.a/ 255.f };

        _vertices.emplace_back(OpenGLVertex {_transformMat * _bottomLeftTextureCorner, glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y), _color });
        _vertices.emplace_back(OpenGLVertex {_transformMat * _bottomRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y), _color });
        _vertices.emplace_back(OpenGLVertex {_transformMat * _topRightTextureCorner, glm::vec2(_textureOriginNorm.x + _textureTileSizeNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y), _color });
        _vertices.emplace_back(OpenGLVertex {_transformMat * _topLeftTextureCorner, glm::vec2(_textureOriginNorm.x, _textureOriginNorm.y + _textureTileSizeNorm.y), _color });

        _indices.emplace_back(_vertexCount + 0);
        _indices.emplace_back(_vertexCount + 1);
        _indices.emplace_back(_vertexCount + 2);

        _indices.emplace_back(_vertexCount + 2);
        _indices.emplace_back(_vertexCount + 3);
        _indices.emplace_back(_vertexCount + 0);
    }

}