//
// Created by borja on 10/18/22.
//

#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UIAnchoring.h"
#include "core/Engine.h"
#include "core/render/elements/Batch.h"
#include "core/render/elements/IRenderizable.h"
#include "core/graph/components/Node.h"

namespace RDE {

    UICheckbox::UICheckbox(Node* _node, Manager* _manager, Graph* _graph, const UICheckboxConfig& _config) {
        data.RenderizableInnerData.texture = _config.checkboxBackgroundTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "checkboxDark") :
                      _config.checkboxBackgroundTexture;
        data.RenderizableInnerData.color = _config.checkboxColor;

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

        uiInteractable->onInnerClickingReleased.bind<&UICheckbox::onMouseReleased>(this);
        uiInteractable->onInnerClicking.bind<&UICheckbox::onMouseClicked>(this);
        uiInteractable->onInnerMouseEntered.bind<&UICheckbox::onMouseEntered>(this);

        textNode = _graph->createNode("Text", _node);
        textRenderer = textNode->addComponent<UIText>(UITextConfig {
            .font = _config.font == nullptr ? _manager->fontManager.getFont("MontserratRegular") :
                    _config.font,
            .text = _config.text,
            .textColor = _config.textColor
        });
		textRenderer->data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;

        auto _checkboxBackgroundNode = _graph->createNode("CheckboxBackground", _node);
        checkboxBackgroundSprite = _checkboxBackgroundNode->addComponent<UIImage>(UIImageConfig {
            .texture = data.RenderizableInnerData.texture,
            .color = _config.checkboxColor
        });
        data.RenderizableInnerData.shader = checkboxBackgroundSprite->getShaderID();
        data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;

        node->getComponent<UIAnchoring>()->setSize({ textRenderer->getSize().x + checkboxBackgroundSprite->getSize().x + _config.checkboxTextOffset.x,
                                                        std::max(checkboxBackgroundSprite->getSize().y, textRenderer->getSize().y * 0.5f)});

        _checkboxBackgroundNode->getComponent<UIAnchoring>()->setAnchor(RDE_UI_ANCHOR_LEFT);
        _checkboxBackgroundNode->getTransform()->setMatrixModelPosition({
            _checkboxBackgroundNode->getTransform()->getModelMatrixPosition().x - getSize().x * 0.5f + checkboxBackgroundSprite->getSize().x * 0.5f,
            _checkboxBackgroundNode->getTransform()->getModelMatrixPosition().y
        });

        tickNode = _graph->createNode("CheckboxTick", _checkboxBackgroundNode);
        tickSprite = tickNode->addComponent<UIImage>(UIImageConfig {
            .texture = _config.checkboxTickTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "checkmark") :
                       _config.checkboxTickTexture,
            .color = _config.tickColor
        });
        tickSprite->setEnabled(_config.checked);


		textNode->getComponent<UIAnchoring>()->setAnchor(RDE_UI_ANCHOR_LEFT);
        textNode->getTransform()->setMatrixModelPosition({
            textNode->getTransform()->getModelMatrixPosition().x - getSize().x * 0.5f +
            checkboxBackgroundSprite->getSize().x + textRenderer->getTextSize().x * 0.5f + _config.checkboxTextOffset.x,
            textNode->getTransform()->getModelMatrixPosition().y
        });

        checked = _config.checked;
        checkboxTextOffset = _config.checkboxTextOffset;
    }


    SIZE_METHODS_DEFAULT_IMPL(UICheckbox)


		void UICheckbox::onMouseReleased(RDE_MOUSE_BUTTON_ _mouseButton) {
        if(!uiInteractable->interactable) return;

		if(uiInteractable->mouseInnerStatus == RDE_MOUSE_STATUS_ENTERED) {
            setChecked(!checked);
            tickSprite->setEnabled(checked);
        }
    }

    void UICheckbox::setInteractable(bool _interactable) {
        uiInteractable->interactable = _interactable;

        if(!uiInteractable->interactable) {
            checkboxBackgroundSprite->setColor(Color::Disabled);
            tickSprite->setColor(Color::Disabled);
            textRenderer->setColor(Color::Disabled);
        } else {
            checkboxBackgroundSprite->setColor(checkboxBackgroundSprite->getColor() == Color::Disabled ? Color::White : checkboxBackgroundSprite->getColor());
            tickSprite->setColor(tickSprite->getColor() == Color::Disabled ? Color::White : tickSprite->getColor());
            textRenderer->setColor(textRenderer->getColor() == Color::Disabled ? Color::White : textRenderer->getColor());
        }
    }

    bool UICheckbox::isInteractable() const {
        return uiInteractable->interactable;
    }

    void UICheckbox::setChecked(bool _checked) {
        checked = _checked;
        if(!onClickedCheckbox.isEmpty()) onClickedCheckbox(checked);
    }

    bool UICheckbox::isChecked() const {
        return checked;
    }


    void UICheckbox::setColor(const Color& _color) {
        SAFE_POINTER(checkboxBackgroundSprite, setColor(_color))
        data.RenderizableInnerData.color = _color;
    }

    Color UICheckbox::getColor() const {
        return data.RenderizableInnerData.color;
    }


    void UICheckbox::setLayer(int _layer) {
        SAFE_POINTER(checkboxBackgroundSprite, setLayer(_layer))
        SAFE_POINTER(tickSprite, setLayer(_layer))
        SAFE_POINTER(textRenderer, setLayer(_layer))
        data.RenderizableInnerData.layer = _layer;
    }

    int UICheckbox::getLayer() const {
        return data.RenderizableInnerData.layer;
    }


    void UICheckbox::setTexture(Texture* _texture) {
        SAFE_POINTER(checkboxBackgroundSprite, setTexture(_texture))
        data.RenderizableInnerData.texture = _texture;
    }

    Texture* UICheckbox::getTexture() const {
        return data.RenderizableInnerData.texture;
    }


    void UICheckbox::setOriginOffset(const Vec2F& _offset) {
        SAFE_POINTER(checkboxBackgroundSprite, setOriginOffset(_offset))
        SAFE_POINTER(tickSprite, setOriginOffset(_offset))
        SAFE_POINTER(textRenderer, setOriginOffset(_offset))
        data.originOffset = _offset;
    }

    Vec2F UICheckbox::getOriginOffset() const {
        return data.originOffset;
    }


    void UICheckbox::setShaderID(ShaderID _shaderID) {
        SAFE_POINTER(checkboxBackgroundSprite, setShaderID(_shaderID))
        data.RenderizableInnerData.shader = _shaderID;
    }

    ShaderID UICheckbox::getShaderID() const {
        return data.RenderizableInnerData.shader;
    }


    Node* UICheckbox::getTickNode() {
        return tickNode;
    }

    Node* UICheckbox::getTextNode() {
        return textNode;
    }

}