//
// Created by borja on 10/18/22.
//

#include "core/graph/components/ui/UICheckbox.h"
#include "core/Engine.h"

namespace GDE {

    UICheckbox::UICheckbox(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UICheckboxConfig& _config) : UI(_nodeID, _canvas) {
        setConfig(_scene, _config);

        UI::texture = config.checkboxTickTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->onInnerClickingReleased.bind<&UICheckbox::onMouseReleased>(this);
        UI::interaction->onInnerClicking.bind<&UICheckbox::onMouseClicked>(this);
        UI::interaction->onInnerMouseEntered.bind<&UICheckbox::onMouseEntered>(this);

        auto _textID = _canvas->getGraph()->createNode("Text", _nodeID);
        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_textID, _scene, _canvas, _config.text, config.font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->color = config.textColor;
        textTransform = _canvas->getGraph()->getComponent<Transform>(_textID);

        auto _checkboxBackgroundID = _canvas->getGraph()->createNode("CheckboxBackground", _nodeID);
        checkboxBackgroundSprite = _canvas->getGraph()->addComponent<SpriteRenderer>(_checkboxBackgroundID, _scene, _canvas, config.checkboxBackgroundTexture);
        checkboxBackgroundSprite->color = _config.checkboxColor;
        checkboxBackgroundTransform = _canvas->getGraph()->getComponent<Transform>(_checkboxBackgroundID);

        auto _tickID = _canvas->getGraph()->createNode("CheckboxTick", _checkboxBackgroundID);
        tickSprite = _canvas->getGraph()->addComponent<SpriteRenderer>(_tickID, _scene, _canvas, config.checkboxTickTexture);
        tickSprite->color = config.tickColor;
        tickTransform = _canvas->getGraph()->getComponent<Transform>(_tickID);

        UI::interaction->sizeOfInteraction = Vec2F { checkboxBackgroundSprite->getSize().x + textRenderer->getSize().x,
                                               checkboxBackgroundSprite->getSize().y > textRenderer->getSize().y ? checkboxBackgroundSprite->getSize().y : textRenderer->getSize().y
                                             } + config.checkboxTextOffset;

        setConfig(_scene, config);
    }

    Vec2F UICheckbox::getSize() const {
        return UI::interaction->sizeOfInteraction;
    }

    UICheckboxConfig UICheckbox::getConfig() {
        return config;
    }

    void UICheckbox::setConfig(Scene* _scene, const UICheckboxConfig& _config) {
        config = _config;

        if(config.stopFurtherClicks) {
            if(!UI::canvas->getGraph()->hasComponent<CanvasEventStopper>(ID)) {
                canvas->getGraph()->addComponent<CanvasEventStopper>(ID);
            }
        } else {
            if(UI::canvas->getGraph()->hasComponent<CanvasEventStopper>(ID)) {
                canvas->getGraph()->removeComponent<CanvasEventStopper>(ID);
            }
        }

        if(config.checkboxBackgroundTexture == nullptr) {
            config.checkboxBackgroundTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "checkboxDark");
        }

        if(config.checkboxTickTexture == nullptr) {
            config.checkboxTickTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "checkmark");
        }

        if(config.font == nullptr) {
            config.font = _scene->engine->manager.fontManager.getDefaultFont("MontserratRegular");
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