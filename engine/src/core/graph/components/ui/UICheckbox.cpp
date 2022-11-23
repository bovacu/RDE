//
// Created by borja on 10/18/22.
//

#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/Engine.h"

namespace RDE {

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
        textRenderer->setBatchPriority(BatchPriority::SpritePriority);
        textRenderer->setColor(config.textColor);
        textTransform = (UITransform*)_textNode->getTransform();

        auto _checkboxBackgroundNode = _graph->createNode("CheckboxBackground", _node);
        checkboxBackgroundSprite = _checkboxBackgroundNode->addComponent<UIImage>(config.checkboxBackgroundTexture);
        checkboxBackgroundSprite->setColor(_config.checkboxColor);
        checkboxBackgroundTransform = (UITransform*)_checkboxBackgroundNode->getTransform();

        auto _tickNode = _graph->createNode("CheckboxTick", _checkboxBackgroundNode);
        tickSprite = _tickNode->addComponent<UIImage>(config.checkboxTickTexture);
        tickSprite->setColor(config.tickColor);
        tickTransform = (UITransform*)_tickNode->getTransform();

        auto _size = Vec2F { checkboxBackgroundSprite->getSize().x + textRenderer->getSize().x,
                       checkboxBackgroundSprite->getSize().y > textRenderer->getSize().y ? checkboxBackgroundSprite->getSize().y : textRenderer->getSize().y
        } + Vec2F { config.checkboxTextOffset.x, config.checkboxTextOffset.y };
        ((UITransform*)node->getTransform())->setSize(_size);

        setConfig(_manager, config);
    }

    Vec2F UICheckbox::getSize() const {
        return ((UITransform*)node->getTransform())->getSize();
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
            config.checkboxBackgroundTexture = _manager->textureManager.getSubTexture("defaultAssets", "checkboxDark");
        }

        if(config.checkboxTickTexture == nullptr) {
            config.checkboxTickTexture = _manager->textureManager.getSubTexture("defaultAssets", "checkmark");
        }

        if(config.font == nullptr) {
            config.font = _manager->fontManager.getDefaultFont("MontserratRegular");
        }

        config.checkboxTextOffset = _config.checkboxTextOffset;
        config.checked = _config.checked;

        Vec2F textRendererSize;

        if(textRenderer != nullptr) {
            textRenderer->setText(config.text);
            textRenderer->setColor(config.textColor);
            textRenderer->setFont(config.font);
            textRendererSize = textRenderer->getSize();
        }

        Vec2F checkboxSize;
        if(checkboxBackgroundSprite != nullptr) {
            checkboxSize = checkboxBackgroundSprite->getSize();
        }

        auto _offset = Vec2F { config.checkboxTextOffset.x, config.checkboxTextOffset.y};
        ((UITransform*)node->getTransform())->setSize(Vec2F { checkboxSize.x + textRendererSize.x, checkboxSize.y > textRendererSize.y ? checkboxSize.y : textRendererSize.y } + _offset);

        if(tickSprite != nullptr) {
            tickSprite->setEnabled(config.checked);
            Util::Log::info(UI::getSize());
        }

        if(checkboxBackgroundTransform != nullptr) {
            auto _checkboxBackgroundPos = checkboxBackgroundTransform->getPosition();
            auto _size = ((UITransform*)node->getTransform())->getSize();
            checkboxBackgroundTransform->setPosition(_checkboxBackgroundPos.x - _size.x / 2.f + checkboxBackgroundSprite->getSize().x / 2.f, _checkboxBackgroundPos.y);
        }

        if(textTransform != nullptr) {
            auto _textPosition = textTransform->getPosition();
            auto _checkboxBackgroundPos = checkboxBackgroundTransform->getPosition();
            textTransform->setPosition(_textPosition.x + _checkboxBackgroundPos.x + checkboxBackgroundSprite->getSize().x / 2.f + textRenderer->getSize().x / 2.f + _offset.x, _textPosition.y + _offset.y);
        }
    }

    void UICheckbox::onMouseReleased(MouseCode _mouseButton) {
        if(!interaction->interactable) return;

        config.checked = !config.checked;
        if(UI::interaction->mouseInnerStatus == UIInteractable::MouseEntered) {
            tickSprite->setEnabled(config.checked);
        }
    }

    void UICheckbox::setInteractable(bool _interactable) {
        interaction->interactable = _interactable;

        if(!interaction->interactable) {
            checkboxBackgroundSprite->setColor(Color::Disabled);
            tickSprite->setColor(Color::Disabled);
            textRenderer->setColor(Color::Disabled);
        } else {
            checkboxBackgroundSprite->setColor(checkboxBackgroundSprite->getColor() == Color::Disabled ? Color::White : checkboxBackgroundSprite->getColor());
            tickSprite->setColor(tickSprite->getColor() == Color::Disabled ? Color::White : tickSprite->getColor());
            textRenderer->setColor(textRenderer->getColor() == Color::Disabled ? Color::White : textRenderer->getColor());
        }
    }

    bool UICheckbox::isInteractable() {
        return interaction->interactable;
    }


}