//
// Created by borja on 11/22/22.
//

#include "core/graph/components/DynamicSpriteRenderer.h"
#include "core/Engine.h"
#include "core/graph/Scene.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/elements/Texture.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/render/elements/SpriteBatchRenderFunctions.h"

namespace RDE {

    DynamicSpriteRenderer::DynamicSpriteRenderer(Node* _node, Scene* _scene, const DynamicSpriteRendererConfig& _config) :
    DynamicSpriteRenderer(_node, &_scene->engine->manager, _scene->graph, _config) {  }

    DynamicSpriteRenderer::DynamicSpriteRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const DynamicSpriteRendererConfig& _config) :
    DynamicSpriteRenderer(_node, &_scene->engine->manager, _canvas->graph, _config)  {  }

    DynamicSpriteRenderer::DynamicSpriteRenderer(Node* _node, Manager* _manager, Graph* _graph, const DynamicSpriteRendererConfig& _config) {

        RENDERIZABLE_BASIC_PROPERTIES_INITIALIZATION(4, SPRITE_RENDERER_SHADER, RDE_BATCH_PRIORITY_SPRITE)
        
        data.texture = new CPUTexture;

        if(_config.pixels == nullptr) {
			((CPUTexture*)data.texture)->init((int)_config.size.x, (int)_config.size.y, (RDE_IMAGE_EXTENSION_)_config.imageType);
        } else {
			((CPUTexture*)data.texture)->init((int)_config.size.x, (int)_config.size.y, _config.pixels, (RDE_IMAGE_EXTENSION_)_config.imageType);
        }

        auto [_transformMat, _] = _node->getTransform()->localToWorld();
        calculateGeometryForSpriteRenderer(data, _transformMat, _node->getTransform(), _manager->sceneManager.getDisplayedScene()->mainCamera->getViewport());
    }

    RENDERIZABLE_BASIC_METHODS_IMPL(DynamicSpriteRenderer, (float)data.texture->getSize().x * node->getTransform()->getScale().x, (float)data.texture->getSize().y * node->getTransform()->getScale().y)

    std::string DynamicSpriteRenderer::getTexturePath() {
        return data.texture->getPath();
    }

    std::string DynamicSpriteRenderer::getTextureName() {
        return Util::String::getFileNameFromPath(data.texture->getPath());
    }

    std::string DynamicSpriteRenderer::getTextureExtension() {
        return Util::String::getFileExtension(data.texture->getPath());
    }

    void DynamicSpriteRenderer::setPixel(int _x, int _y, const Color& _color) {
        ((CPUTexture*)data.texture)->setPixel(_x, _y, _color);
        data.dirty = true;
    }

    Color DynamicSpriteRenderer::getPixel(int _x, int _y) {
        return ((CPUTexture*)data.texture)->getPixel(_x, _y);
    }

    void DynamicSpriteRenderer::resizeImage(const Vec2<uint>& _newSize) {
        ((CPUTexture*)data.texture)->resizeImage(_newSize);
        data.dirty = true;
    }

    DynamicSpriteRenderer::~DynamicSpriteRenderer() {
        delete ((CPUTexture*)data.texture);
    }

} // RDE