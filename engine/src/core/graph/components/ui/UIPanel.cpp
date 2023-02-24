//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UIPanel.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/render/elements/Batch.h"
#include "core/render/elements/IRenderizable.h"
#include "core/render/elements/ShaderManager.h"
#include "core/render/elements/Texture.h"
#include "core/util/Vec.h"
#include "core/graph/components/Node.h"

namespace RDE {

    static void foo() {

    }

	static void foo2(RDE_MOUSE_BUTTON_ _mouseCode) {

    }

    UIPanel::UIPanel(Node* _node, Manager* _manager, Graph* _graph, const UIPanelConfig& _config) {
        data.RenderizableInnerData.texture = _config.texture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "panel") : _config.texture;
        data.RenderizableInnerData.color = _config.color;
        data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;

        node = _node;

        if(!_node->hasComponent<UIInteractable>()) {
            uiInteractable = _node->addComponent<UIInteractable>();
        }
                                                                                                           
        if(_config.stopFurtherClicks) {
            if(!node->hasComponent<CanvasEventStopper>()) {
                node->addComponent<CanvasEventStopper>();
            }
        } else {
            if (node->hasComponent<CanvasEventStopper>()) {
                node->removeComponent<CanvasEventStopper>();
            }
        }

        ((UITransform*)_node->getTransform())->setSize(_config.size);

        uiInteractable->onInnerMouseEntered.bind<&foo>();
        uiInteractable->onInnerMouseExited.bind<&foo>();
        uiInteractable->onInnerClicking.bind<&foo2>();
        uiInteractable->onInnerClickingReleased.bind<&foo2>();

        uiImage = _node->addComponent<UIImage>(UIImageConfig {
            .size = _config.size,
            .texture = data.RenderizableInnerData.texture,
            .color = _config.color,
            .imageRenderingType = RDE_IMAGE_RENDERING_TYPE_NINE_SLICE
        });

        uiImage->uiInteractable = uiInteractable;
        data.RenderizableInnerData.shader = uiImage->getShaderID();
    }

    
    SIZE_METHODS_DEFAULT_IMPL(UIPanel)
    INTERACTABLE_DEFAULT_IMPL(UIPanel)
    ENABLED_DEFAULT_IMPL(UIPanel)


    void UIPanel::setColor(const Color& _color) {
        SAFE_POINTER(uiImage, setColor(_color))
        data.RenderizableInnerData.color = _color;
    }

    Color UIPanel::getColor() const {
        return data.RenderizableInnerData.color;
    }


    void UIPanel::setLayer(int _layer) {
        SAFE_POINTER(uiImage, setLayer(_layer))
        data.RenderizableInnerData.layer = _layer;
    }

    int UIPanel::getLayer() const {
        return data.RenderizableInnerData.layer;
    }


    void UIPanel::setTexture(Texture* _texture) {
        SAFE_POINTER(uiImage, setTexture(_texture))
        data.RenderizableInnerData.texture = _texture;
    }

    Texture* UIPanel::getTexture() const {
        return data.RenderizableInnerData.texture;
    }


    void UIPanel::setOriginOffset(const Vec2F& _offset) {
        SAFE_POINTER(uiImage,setOriginOffset(_offset))
        data.originOffset = _offset;
    }

    Vec2F UIPanel::getOriginOffset() const {
        return data.originOffset;
    }


    void UIPanel::setShaderID(ShaderID _shaderID) {
        SAFE_POINTER(uiImage, setShaderID(_shaderID))
        data.RenderizableInnerData.shader = _shaderID;
    }

    ShaderID UIPanel::getShaderID() const {
        return data.RenderizableInnerData.shader;
    }
}