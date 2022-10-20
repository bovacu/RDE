//
// Created by borja on 9/22/22.
//

#include "core/graph/components/ui/UIButton.h"
#include "core/Engine.h"

namespace GDE {

    UIButton::UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config) : UI(_nodeID, _canvas) {
        setConfig(_scene, _config);

        UI::texture = config.idleTexture;
        _canvas->getGraph()->addComponent<CanvasEventStopper>(_nodeID);

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->sizeOfInteraction = _config.interactableArea;
        UI::interaction->onInnerMouseEntered.bind<&UIButton::onMouseEntered>(this);
        UI::interaction->onInnerMouseExited.bind<&UIButton::onMouseExited>(this);
        UI::interaction->onInnerClicking.bind<&UIButton::onMouseClicked>(this);
        UI::interaction->onInnerClickingReleased.bind<&UIButton::onMouseReleased>(this);

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_nodeID, _scene, _canvas, config.idleTexture);
        nineSliceSprite->nineSliceSize = _config.buttonTextureSize;
        nineSliceSprite->color = _config.buttonColor;

        auto _textID = _canvas->getGraph()->createNode("Text", _nodeID);
        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_textID, _scene, _canvas, _config.text, config.font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->color = config.textColor;
    }

    NineSlice& UIButton::getNineSlice() const {
        return nineSliceSprite->getNineSlice();
    }

    Vec2F UIButton::getSize() const {
        return UI::interaction->sizeOfInteraction;
    }

    UIButtonConfig UIButton::getConfig() {
        return config;
    }

    void UIButton::setConfig(Scene* _scene, const UIButtonConfig& _config) {
        config = _config;

        if(config.idleTexture == nullptr) {
            config.idleTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "buttonDark");
        }

        if(config.selectedTexture == nullptr) {
            config.selectedTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "buttonDarkHighlited");
        }

        if(config.clickedTexture == nullptr) {
            config.clickedTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "buttonDarkPressed");
        }

        if(config.disabledTexture == nullptr) {
            config.disabledTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "buttonDarkLock");
        }

        if(config.font == nullptr) {
            config.font = _scene->engine->manager.fontManager.getDefaultFont("MontserratRegular");
        }

        UI::interaction->sizeOfInteraction = config.interactableArea;

        if(nineSliceSprite != nullptr) {
            nineSliceSprite->nineSliceSize = config.buttonTextureSize;
            nineSliceSprite->color = config.buttonColor;
        }

        if(textRenderer != nullptr) {
            textRenderer->setText(config.text);
            textRenderer->color = config.textColor;
            textRenderer->setFont(config.font);
        }
    }

    void UIButton::onMouseEntered() {
        if(!config.disabled) {
            nineSliceSprite->texture = config.selectedTexture;
        }
    }

    void UIButton::onMouseExited() {
        if(!config.disabled) {
            nineSliceSprite->texture = config.idleTexture;
        }
    }

    void UIButton::onMouseClicked(MouseCode _mouseCode) {
        if(!config.disabled) {
            nineSliceSprite->texture = config.clickedTexture;
        }
    }

    void UIButton::onMouseReleased(MouseCode _mouseCode) {
        if(!config.disabled) {
            if(UI::interaction->mouseInnerStatus == UIInteractable::MouseExited) {
                nineSliceSprite->texture = config.idleTexture;
            } else {
                nineSliceSprite->texture = config.selectedTexture;
            }
        }
    }

    void UIButton::setDisabled(bool _disabled) {
        config.disabled = _disabled;
        if(config.disabled) {
            nineSliceSprite->texture = config.disabledTexture;
        } else {
            onMouseReleased(MouseCode::ButtonLeft);
        }
    }

    bool UIButton::isDisabled() {
        return config.disabled;
    }
}