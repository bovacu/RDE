//
// Created by borja on 10/26/22.
//

#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/Engine.h"
#include "core/render/elements/Batch.h"
#include "core/render/elements/IRenderizable.h"
#include "core/graph/components/Node.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/render/elements/SpriteBatchRenderFunctions.h"

namespace RDE {


    UIImage::UIImage(Node* _node, Scene* _scene, Canvas* _canvas, const UIImageConfig& _config) :
    UIImage(_node, &_scene->engine->manager, _canvas->getGraph(), _config) {  }

    UIImage::UIImage(Node* _node, Manager* _manager, Graph* _graph, const UIImageConfig& _config) {

        RENDERIZABLE_UI_BASIC_PROPERTIES_INITIALIZATION(36, SPRITE_RENDERER_SHADER, BatchPriority::SpritePriority)

        data.RenderizableInnerData.texture = _config.texture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "sprite") : _config.texture;
        data.RenderizableInnerData.renderizableType = RenderizableType::RT_UI_IMAGE;
        setColor(_config.color);

        ((UITransform*)node->getTransform())->setSize(_config.size == -1 ? data.RenderizableInnerData.texture->getSize() : _config.size);

        imageRenderingType = _config.imageRenderingType;
        if(imageRenderingType == ImageRenderingType::NINE_SLICE && !data.RenderizableInnerData.texture->nineSlice.isEnabled()) {
            Util::Log::warn("UIImage cannot be rendered as 9Slice as the texture is not configured like so.");
            imageRenderingType = ImageRenderingType::NORMAL;
            return;
        }

        data.imageRenderingType = (uint)imageRenderingType;

        auto [_transformMat, _] = _node->getTransform()->localToWorld();
        calculateGeometryForUIImage(data, _transformMat, _node->getTransform(), _manager->sceneManager.getDisplayedScene()->getMainCamera()->getViewport());
    }


    RENDERIZABLE_UI_BASIC_METHODS_IMPL(UIImage, 
        ((UITransform*)node->getTransform())->getSize().x * ((UITransform*)node->getTransform())->getScale().x, 
        ((UITransform*)node->getTransform())->getSize().y * ((UITransform*)node->getTransform())->getScale().y,
        {
            ((UITransform*)node->getTransform())->setSize({ Util::Math::clampF(_size.x, 0, FLT_MAX), Util::Math::clampF(_size.y, 0, FLT_MAX) });
        })


    std::string UIImage::getTexturePath() {
        return data.RenderizableInnerData.texture->getPath();
    }

    std::string UIImage::getTextureName() {
        return Util::String::getFileNameFromPath(data.RenderizableInnerData.texture->getPath());
    }

    std::string UIImage::getTextureExtension() {
        return Util::String::getFileExtension(data.RenderizableInnerData.texture->getPath());
    }

    void UIImage::setImageRenderingType(ImageRenderingType _imageRenderingType) {
        if(_imageRenderingType == ImageRenderingType::NINE_SLICE && !data.RenderizableInnerData.texture->nineSlice.isEnabled()) {
            Util::Log::warn("UIImage cannot be rendered as 9Slice as the texture is not configured like so.");
            return;
        }

        for(auto& _vertex : data.RenderizableInnerData.vertices) {
            _vertex = {};
        }

        imageRenderingType = _imageRenderingType;
        data.RenderizableInnerData.dirty = true;
        data.imageRenderingType = (uint)_imageRenderingType;
    }

    void UIImage::setPartialRenderingInverted(bool _inverted) {
        partialRenderingInverted = _inverted;
        data.RenderizableInnerData.dirty = true;
    }

    void UIImage::setPartialRenderingPercentage(float _percentage) {
        _percentage = Util::Math::clampF(_percentage, 0, 1);
        partialRenderingPercentage = _percentage;
        data.RenderizableInnerData.dirty = true;
    }
}