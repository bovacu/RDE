//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/Engine.h"

namespace RDE {


    UIImage::UIImage(Node* _node, Scene* _scene, Canvas* _canvas, const UIImageConfig& _config) :
    UIImage(_node, &_scene->engine->manager, _canvas->getGraph(), _config) {  }

    UIImage::UIImage(Node* _node, Manager* _manager, Graph* _graph, const UIImageConfig& _config) : UI(_node, &_config) {
        shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        IRenderizable::batchPriority = BatchPriority::SpritePriority;

        texture = _config.texture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "sprite") : _config.texture;
        setColor(_config.color);

        ((UITransform*)node->getTransform())->setSize(texture->getSize());

        auto [_transformMat, _] = _node->getTransform()->localToWorld();
        calculateGeometry(_transformMat, *_node->getTransform(), *_manager->sceneManager.getDisplayedScene()->getMainCamera()->getViewport());
    }

    void UIImage::calculateGeometry(glm::mat4& _transformMatrix, Transform& _transform, const ViewPort& _viewport) {
        auto _originOffset = UI::getOriginOffset();
        auto _screenPos = Util::Math::worldToScreenCoordsUI(_viewport, { _transformMatrix[3][0] + _originOffset.x, _transformMatrix[3][1] + _originOffset.y });

        _transformMatrix[3][0] = _screenPos.x;
        _transformMatrix[3][1] = _screenPos.y;

        Vec2F _textureOrigin = {(float)texture->getRegion().bottomLeftCorner.x, (float)texture->getRegion().bottomLeftCorner.y};
        Vec2F _textureOriginNorm = {_textureOrigin.x / (float)texture->getSpriteSheetSize().x, _textureOrigin.y / (float)texture->getSpriteSheetSize().y};

        Vec2F _textureTileSize = {(float)texture->getRegion().size.x, (float)texture->getRegion().size.y};
        Vec2F _textureTileSizeNorm = {_textureTileSize.x / (float)texture->getSpriteSheetSize().x, _textureTileSize.y / (float)texture->getSpriteSheetSize().y};
        auto _textureTileSizeOnScreen = Util::Math::worldToScreenSizeUI(_viewport, _textureTileSize);


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

    std::string UIImage::getTexturePath() {
        return texture->getPath();
    }

    std::string UIImage::getTextureName() {
        return Util::String::getFileNameFromPath(texture->getPath());
    }

    std::string UIImage::getTextureExtension() {
        return Util::String::getFileExtension(texture->getPath());
    }

    void
    UIImage::drawBatched(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const ViewPort& _viewport) {
        auto _vertexCount = _vertices.size();

        auto [_transformMat, _dirty] = _transform.localToWorld();
        calculateGeometry(_transformMat, _transform, _viewport);
        if(_dirty || dirty) {
            _transform.clearDirty();
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

    // TODO: implement interactable for UIImage
    void UIImage::setInteractable(bool _interactable) {

    }

    bool UIImage::isInteractable() {
        return false;
    }
}