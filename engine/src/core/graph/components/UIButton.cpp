//
// Created by borja on 9/22/22.
//

#include "core/graph/components/UIButton.h"
#include "core/Engine.h"

namespace GDE {

    UIButton::UIButton(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config) : UI(_nodeID, _canvas) {
        setConfig(_scene, _config);

        UI::texture = config.idleTexture;
        _canvas->getGraph()->addComponent<CanvasEventStopper>(_nodeID);

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_nodeID, _scene, _canvas, config.idleTexture);
        nineSliceSprite->interaction = UI::interaction;
        nineSliceSprite->interaction->onInnerMouseEntered.bind<&UIButton::onMouseEntered>(this);
        nineSliceSprite->interaction->onInnerMouseExited.bind<&UIButton::onMouseExited>(this);
        nineSliceSprite->interaction->onInnerClicking.bind<&UIButton::onMouseClicked>(this);
        nineSliceSprite->interaction->onInnerClickingReleased.bind<&UIButton::onMouseReleased>(this);
        nineSliceSprite->nineSliceSize = _config.size;
        nineSliceSprite->color = _config.buttonColor;

        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_nodeID, _scene, _canvas, _config.text, config.font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->color = config.textColor;
    }

    NineSlice& UIButton::getNineSlice() const {
        return nineSliceSprite->getNineSlice();
    }

    Vec2F UIButton::getSize() const {
        return nineSliceSprite->getSize();
    }

    void UIButton::draw(std::vector<OpenGLVertex>& _vertices, std::vector<uint32_t>& _indices, Transform& _transform, const IViewPort& _viewport) {
        if(lastTextOffset != textOffset) {
            textRenderer->transform->translate(textOffset - lastTextOffset);
            lastTextOffset = textOffset;
        }

        if(lastTextScale != textScale) {
            textRenderer->transform->scale(textScale - lastTextScale);
            lastTextScale = textScale;
        }

        if(lastTextRotation != textRotation) {
            textRenderer->transform->rotate(textRotation - lastTextRotation);
            lastTextRotation = textRotation;
        }
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

        if(nineSliceSprite != nullptr) {
            nineSliceSprite->nineSliceSize = config.size;
            nineSliceSprite->color = config.buttonColor;
        }

        if(textRenderer != nullptr) {
            textRenderer->setText(config.text);
            textRenderer->color = config.textColor;
            textRenderer->setFont(config.font);
        }
    }

    void UIButton::onMouseEntered() {
        nineSliceSprite->texture = config.selectedTexture;
    }

    void UIButton::onMouseExited() {
        nineSliceSprite->texture = config.idleTexture;
    }

    void UIButton::onMouseClicked(MouseCode _mouseCode) {
        nineSliceSprite->texture = config.clickedTexture;
    }

    void UIButton::onMouseReleased(MouseCode _mouseCode) {
        if(nineSliceSprite->interaction->mouseInnerStatus == UIInteractable::MouseExited) {
            nineSliceSprite->texture = config.idleTexture;
        } else {
            nineSliceSprite->texture = config.selectedTexture;
        }
    }
}