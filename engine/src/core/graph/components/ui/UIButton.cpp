//
// Created by borja on 9/22/22.
//

#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UIText.h"
#include "core/Engine.h"
#include "core/graph/Scene.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/render/elements/IRenderizable.h"
#include "core/graph/components/Node.h"
#include "core/systems/uiSystem/Canvas.h"

namespace RDE {

    UIButton::UIButton(Node* _node, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config) :
    UIButton(_node, &_scene->engine->manager, _canvas->graph, _config) {  }

    UIButton::UIButton(Node* _node, Manager* _manager, Graph* _graph, const UIButtonConfig& _config) {
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

        data.RenderizableInnerData.texture = _config.idleTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "buttonDark") : _config.idleTexture;

        statesTextures[0] = data.RenderizableInnerData.texture;
        statesTextures[1] = _config.selectedTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "buttonDarkHighlited") :
                            _config.selectedTexture;
        statesTextures[2] = _config.clickedTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "buttonDarkPressed") :
                            _config.clickedTexture;
        statesTextures[3] = _config.disabledTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "buttonDarkLock") :
                            _config.disabledTexture;

        uiInteractable->onInnerMouseEntered.bind<&UIButton::onMouseEntered>(this);
        uiInteractable->onInnerMouseExited.bind<&UIButton::onMouseExited>(this);
        uiInteractable->onInnerClicking.bind<&UIButton::onMouseClicked>(this);
        uiInteractable->onInnerClickingReleased.bind<&UIButton::onMouseReleased>(this);

        ((UITransform*)node->getTransform())->setSize(_config.buttonSize);

        uiImage = _node->addComponent<UIImage>(UIImageConfig {
            .size = _config.buttonSize,
            .texture = data.RenderizableInnerData.texture,
            .color = _config.buttonColor,
            .imageRenderingType = RDE_IMAGE_RENDERING_TYPE_NINE_SLICE
        });
        uiImage->uiInteractable = uiInteractable;
        data.RenderizableInnerData.shader = uiImage->getShaderID();
        data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;

        textNode = _graph->createNode("Text", _node);
        textRenderer = textNode->addComponent<UIText>(UITextConfig {
            .font = _config.font,
            .text = _config.text,
            .textColor = _config.textColor
        });
		textRenderer->data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;
    }

    void UIButton::onMouseEntered() {
        if(!uiInteractable->interactable) return;
        SAFE_POINTER(uiImage, setTexture(statesTextures[1]))
    }

    void UIButton::onMouseExited() {
        if(!uiInteractable->interactable) return;
        SAFE_POINTER(uiImage, setTexture(statesTextures[0]))
    }

	void UIButton::onMouseClicked(RDE_MOUSE_BUTTON_ _mouseCode) {
        if(!uiInteractable->interactable) return;
        SAFE_POINTER(uiImage, setTexture(statesTextures[2]))
    }

	void UIButton::onMouseReleased(RDE_MOUSE_BUTTON_ _mouseCode) {
        if(!uiInteractable->interactable) return;

        if(uiInteractable->mouseInnerStatus == RDE_MOUSE_STATUS_EXITED) {
            SAFE_POINTER(uiImage, setTexture(statesTextures[0]))
        } else {
            SAFE_POINTER(uiImage, setTexture(statesTextures[1]))
        }
    }


    ENABLED_DEFAULT_IMPL(UIButton)
    SIZE_METHODS_DEFAULT_IMPL(UIButton)


    void UIButton::setInteractable(bool _enabled) {
        uiInteractable->interactable = _enabled;
        if(!uiInteractable->interactable) {
            SAFE_POINTER(uiImage, setTexture(statesTextures[3]))
        } else {
			onMouseReleased(RDE_MOUSE_BUTTON_LEFT);
        }
    }

    bool UIButton::isInteractable() const {
        return uiInteractable->interactable;
    }


    void UIButton::setColor(const Color& _color) { 
        SAFE_POINTER(uiImage, setColor(_color))
        data.RenderizableInnerData.color = _color;
    }

    Color UIButton::getColor() const {
        return data.RenderizableInnerData.color;
    }


    void UIButton::setLayer(int _layer) {
        SAFE_POINTER(uiImage, setLayer(_layer))
        SAFE_POINTER(textRenderer, setLayer(_layer))
        data.RenderizableInnerData.layer = _layer;
    }

    int UIButton::getLayer() const {
        return data.RenderizableInnerData.layer;
    }


    void UIButton::setTexture(Texture* _texture) {
        SAFE_POINTER(uiImage, setTexture(_texture))
        data.RenderizableInnerData.texture = _texture;
    }

    Texture* UIButton::getTexture() const {
        return data.RenderizableInnerData.texture;
    }


    void UIButton::setOriginOffset(const Vec2F& _offset) {
        SAFE_POINTER(uiImage, setOriginOffset(_offset))
        SAFE_POINTER(textRenderer, setOriginOffset(_offset))
        data.originOffset = _offset;
    }

    Vec2F UIButton::getOriginOffset() const {
        return data.originOffset;
    }


    void UIButton::setShaderID(ShaderID _shaderID) {
        SAFE_POINTER(uiImage, setShaderID(_shaderID))
        data.RenderizableInnerData.shader = _shaderID;
    }

    ShaderID UIButton::getShaderID() const {
        return data.RenderizableInnerData.shader;
    }


    Node* UIButton::getTextNode() {
        return textNode;
    }
}