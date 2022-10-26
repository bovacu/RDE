//
// Created by borja on 10/18/22.
//

#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/Engine.h"

namespace GDE {

    UICheckbox::UICheckbox(Node* _node, Manager* _manager, Graph* _graph, const UICheckboxConfig& _config) : UI(_node) {
        setConfig(_manager, _config);

        UI::texture = config.checkboxTickTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->onInnerClickingReleased.bind<&UICheckbox::onMouseReleased>(this);
        UI::interaction->onInnerClicking.bind<&UICheckbox::onMouseClicked>(this);
        UI::interaction->onInnerMouseEntered.bind<&UICheckbox::onMouseEntered>(this);

        auto _textNode = _graph->createNode("Text", _node);
        textRenderer = _textNode->addComponent<UIText>(_config.text, config.font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->color = config.textColor;
        textTransform = _textNode->getTransform();

        auto _checkboxBackgroundNode = _graph->createNode("CheckboxBackground", _node);
        checkboxBackgroundSprite = _checkboxBackgroundNode->addComponent<UIImage>(config.checkboxBackgroundTexture);
        checkboxBackgroundSprite->color = _config.checkboxColor;
        checkboxBackgroundTransform = _checkboxBackgroundNode->getTransform();

        auto _tickNode = _graph->createNode("CheckboxTick", _checkboxBackgroundNode);
        tickSprite = _tickNode->addComponent<UIImage>(config.checkboxTickTexture);
        tickSprite->color = config.tickColor;
        tickTransform = _tickNode->getTransform();

        UI::interaction->sizeOfInteraction = Vec2F { checkboxBackgroundSprite->getSize().x + textRenderer->getSize().x,
                                               checkboxBackgroundSprite->getSize().y > textRenderer->getSize().y ? checkboxBackgroundSprite->getSize().y : textRenderer->getSize().y
                                             } + config.checkboxTextOffset;

        setConfig(_manager, config);
    }

    Vec2F UICheckbox::getSize() const {
        return UI::interaction->sizeOfInteraction;
    }

    UICheckboxConfig UICheckbox::getConfig() {
        return config;
    }

    void UICheckbox::setConfig(Manager* _manager, const UICheckboxConfig& _config) {
        config = _config;

        if(config.stopFurtherClicks) {
            if(!UI::node->hasComponent<CanvasEventStopper>()) {
                UI::node->addComponent<CanvasEventStopper>();
            }
        } else {
            if(UI::node->hasComponent<CanvasEventStopper>()) {
                UI::node->removeComponent<CanvasEventStopper>();
            }
        }

        if(config.checkboxBackgroundTexture == nullptr) {
            config.checkboxBackgroundTexture = _manager->textureManager.getSubTexture("assets", "checkboxDark");
        }

        if(config.checkboxTickTexture == nullptr) {
            config.checkboxTickTexture = _manager->textureManager.getSubTexture("assets", "checkmark");
        }

        if(config.font == nullptr) {
            config.font = _manager->fontManager.getDefaultFont("MontserratRegular");
        }

        config.checkboxTextOffset = _config.checkboxTextOffset;
        config.checked = _config.checked;

        UI::interaction->sizeOfInteraction = { 0.0f, 0.0f };
        Vec2F textRendererSize;

        if(textRenderer != nullptr) {
            textRenderer->setText(config.text);
            textRenderer->color = config.textColor;
            textRenderer->setFont(config.font);
            textRendererSize = textRenderer->getSize();
        }

        Vec2F checkboxSize;
        if(checkboxBackgroundSprite != nullptr) {
            checkboxSize = checkboxBackgroundSprite->getSize();
        }

        UI::interaction->sizeOfInteraction = Vec2F { checkboxSize.x + textRendererSize.x, checkboxSize.y > textRendererSize.y ? checkboxSize.y : textRendererSize.y } + config.checkboxTextOffset;

        if(tickSprite != nullptr) {
            tickSprite->enabled = config.checked;
        }

        if(checkboxBackgroundTransform != nullptr) {
            auto _checkboxBackgroundPos = checkboxBackgroundTransform->getPosition();
            checkboxBackgroundTransform->setPosition(_checkboxBackgroundPos.x - UI::interaction->sizeOfInteraction.x / 2.f + checkboxBackgroundSprite->getSize().x / 2.f, _checkboxBackgroundPos.y);
        }

        if(textTransform != nullptr) {
            auto _textPosition = textTransform->getPosition();
            auto _checkboxBackgroundPos = checkboxBackgroundTransform->getPosition();
            textTransform->setPosition(_textPosition.x + _checkboxBackgroundPos.x + checkboxBackgroundSprite->getSize().x / 2.f + textRenderer->getSize().x / 2.f + config.checkboxTextOffset.x, _textPosition.y + config.checkboxTextOffset.y);
        }
    }

    void UICheckbox::onMouseReleased(MouseCode _mouseButton) {
        if(!interaction->interactable) return;

        checked = !checked;
        if(UI::interaction->mouseInnerStatus == UIInteractable::MouseEntered) {
            tickSprite->enabled = checked;
        }
    }

    void UICheckbox::setInteractable(bool _interactable) {
        interaction->interactable = _interactable;

        if(!interaction->interactable) {
            checkboxBackgroundSprite->color = Color::Disabled;
            tickSprite->color = Color::Disabled;
            textRenderer->color = Color::Disabled;
        } else {
            checkboxBackgroundSprite->color = checkboxBackgroundSprite->color == Color::Disabled ? Color::White : checkboxBackgroundSprite->color;
            tickSprite->color = tickSprite->color == Color::Disabled ? Color::White : tickSprite->color;
            textRenderer->color = textRenderer->color == Color::Disabled ? Color::White : textRenderer->color;
        }
    }

    bool UICheckbox::isInteractable() {
        return interaction->interactable;
    }


}