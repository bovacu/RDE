//
// Created by borja on 11/22/22.
//

#include "core/graph/components/DynamicSpriteRenderer.h"
#include "core/Engine.h"
#include "core/systems/uiSystem/Canvas.h"

namespace RDE {

    DynamicSpriteRenderer::DynamicSpriteRenderer(Node* _node, Scene* _scene, const DynamicSpriteRendererConfig& _config) :
    DynamicSpriteRenderer(_node, &_scene->engine->manager, _scene->getMainGraph(), _config) {  }

    DynamicSpriteRenderer::DynamicSpriteRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const DynamicSpriteRendererConfig& _config) :
    DynamicSpriteRenderer(_node, &_scene->engine->manager, _canvas->getGraph(), _config)  {  }

    DynamicSpriteRenderer::DynamicSpriteRenderer(Node* _node, Manager* _manager, Graph* _graph, const DynamicSpriteRendererConfig& _config) : IRenderizable(_node) {
        shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        IRenderizable::batchPriority = BatchPriority::SpritePriority;

        texture = new CPUTexture;

        if(_config.pixels == nullptr) {
            texture->init((int)_config.size.x, (int)_config.size.y, _config.imageType);
        } else {
            texture->init((int)_config.size.x, (int)_config.size.y, _config.pixels, _config.imageType);
        }

        auto [_transformMat, _] = _node->getTransform()->localToWorld();
        calculateGeometry(_transformMat, *_node->getTransform(), *_manager->sceneManager.getDisplayedScene()->getMainCamera()->getViewport());
    }

    void DynamicSpriteRenderer::calculateGeometry(glm::mat4& _transformMatrix, RDE::Transform& _transform, const RDE::ViewPort& _viewport) {
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

    std::string DynamicSpriteRenderer::getTexturePath() {
        return texture->getPath();
    }

    std::string DynamicSpriteRenderer::getTextureName() {
        return Util::String::getFileNameFromPath(texture->getPath());
    }

    std::string DynamicSpriteRenderer::getTextureExtension() {
        return Util::String::getFileExtension(texture->getPath());
    }

    void DynamicSpriteRenderer::drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const ViewPort& _viewport) {
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

    void DynamicSpriteRenderer::setPixel(int _x, int _y, const Color& _color) {
        texture->setPixel(_x, _y, _color);
        dirty = true;
    }

    Color DynamicSpriteRenderer::getPixel(int _x, int _y) {
        return texture->getPixel(_x, _y);
    }

    void DynamicSpriteRenderer::resizeImage(const Vec2<uint>& _newSize) {
        texture->resizeImage(_newSize);
        dirty = true;
    }

    DynamicSpriteRenderer::~DynamicSpriteRenderer() {
        delete texture;
    }

} // RDE