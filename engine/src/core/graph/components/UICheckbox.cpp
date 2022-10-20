//
// Created by borja on 10/18/22.
//

#include "core/graph/components/UICheckbox.h"
#include "core/Engine.h"

namespace GDE {

    UICheckbox::UICheckbox(const NodeID& nodeId, Canvas* canvas, const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UICheckboxConfig& _config) : UI(nodeId, canvas) {
        setConfig(_scene, _config);

        UI::texture = config.checkboxTickTexture;
        _canvas->getGraph()->addComponent<CanvasEventStopper>(_nodeID);

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_nodeID, _scene, _canvas, config.checkboxBackgroundTexture);
        nineSliceSprite->interaction = UI::interaction;
        nineSliceSprite->interaction->onInnerClickingReleased.bind<&UICheckbox::onMouseReleased>(this);

        if(config.checkboxSize.x > 0) {
            nineSliceSprite->nineSliceSize.x = _config.checkboxSize.x;
        }
        if(config.checkboxSize.y > 0) {
            nineSliceSprite->nineSliceSize.y = _config.checkboxSize.y;
        }
        nineSliceSprite->color = _config.checkboxColor;

        spriteRenderer = _canvas->getGraph()->addComponent<SpriteRenderer>(_nodeID, _scene, _canvas, config.checkboxTickTexture);
        spriteRenderer->color = config.tickColor;

        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_nodeID, _scene, _canvas, _config.text, config.font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->color = config.textColor;
    }

    NineSlice& UICheckbox::getNineSlice() const {
        return nineSliceSprite->getNineSlice();
    }

    Vec2F UICheckbox::getSize() const {
        return { nineSliceSprite->getSize().x + textRenderer->getSize().x, nineSliceSprite->getSize().y > textRenderer->getSize().y ?nineSliceSprite->getSize().y : textRenderer->getSize().y };
    }

    UICheckboxConfig UICheckbox::getConfig() {
        return config;
    }

    void UICheckbox::setConfig(Scene* _scene, const UICheckboxConfig& _config) {
        if(config.checkboxBackgroundTexture == nullptr) {
            config.checkboxBackgroundTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "checkboxDark");
        }

        if(config.checkboxTickTexture == nullptr) {
            config.checkboxTickTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "checkmark64x64");
        }

        if(nineSliceSprite != nullptr) {
            if(config.checkboxSize.x > 0) {
                nineSliceSprite->nineSliceSize.x = _config.checkboxSize.x;
            }
            if(config.checkboxSize.y > 0) {
                nineSliceSprite->nineSliceSize.y = _config.checkboxSize.y;
            }
        }

        if(textRenderer != nullptr) {
            textRenderer->setText(config.text);
            textRenderer->color = config.textColor;
            textRenderer->setFont(config.font);
        }
    }

    void UICheckbox::onMouseReleased(MouseCode _mouseButton) {

    }


}