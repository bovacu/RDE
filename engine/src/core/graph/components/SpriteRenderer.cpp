//
// Created by borja on 9/5/22.
//

#include "core/graph/components/SpriteRenderer.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/elements/ViewPort.h"
#include "core/util/Color.h"
#include "core/util/Functions.h"
#include "core/graph/components/Transform.h"
#include "core/render/elements/Vertex.h"
#include "core/render/elements/Batch.h"
#include "core/graph/Scene.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/Engine.h"
#include "core/util/Rect.h"
#include "core/util/Vec.h"
#include <iterator>
#include "core/render/elements/SpriteBatchRenderFunctions.h"

namespace RDE {

    SpriteRenderer::SpriteRenderer(Node* _node, Scene* _scene, const SpriteRendererConfig& _config) :
    SpriteRenderer(_node, &_scene->engine->manager, _scene->graph, _config) {  }

    SpriteRenderer::SpriteRenderer(Node* _node, Scene* _scene, Canvas* _canvas, const SpriteRendererConfig& _config) :
    SpriteRenderer(_node, &_scene->engine->manager, _canvas->graph, _config)  {  }

    SpriteRenderer::SpriteRenderer(Node* _node, Manager* _manager, Graph* _graph, const SpriteRendererConfig& _config) {
        
        RENDERIZABLE_BASIC_PROPERTIES_INITIALIZATION(4, SPRITE_RENDERER_SHADER, BatchPriority::SpritePriority)

        data.texture = _config.texture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "sprite") : _config.texture;
        data.renderizableType = RenderizableType::RT_SPRITE;
        
        setColor(_config.color);

        auto [_transformMat, _] = _node->getTransform()->localToWorld();
        calculateGeometryForSpriteRenderer(data, _transformMat, _node->getTransform(), _manager->sceneManager.getDisplayedScene()->mainCamera->getViewport());
    }

    RENDERIZABLE_BASIC_METHODS_IMPL(SpriteRenderer, (float)data.texture->getSize().x * node->getTransform()->getScale().x, (float)data.texture->getSize().y * node->getTransform()->getScale().y)

    std::string SpriteRenderer::getTexturePath() {
        return data.texture->getPath();
    }

    std::string SpriteRenderer::getTextureName() {
        return Util::String::getFileNameFromPath(data.texture->getPath());
    }

    std::string SpriteRenderer::getTextureExtension() {
        return Util::String::getFileExtension(data.texture->getPath());
    }

}