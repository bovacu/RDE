//
// Created by borja on 10/21/22.
//

#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/Components.h"
#include "core/Engine.h"

namespace GDE {

    UIInput::UIInput(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UIInputConfig& _config) : UI(_nodeID, _canvas) {
        setConfig(_scene, _config);

        UI::texture = config.inputBackgroundTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->sizeOfInteraction = _config.inputSize;
        UI::interaction->onInnerMouseEntered.bind<&UIInput::onMouseEntered>(this);
        UI::interaction->onInnerMouseExited.bind<&UIInput::onMouseExited>(this);
        UI::interaction->onInnerClicking.bind<&UIInput::onMouseClicked>(this);
        UI::interaction->onInnerClickingReleased.bind<&UIInput::onMouseReleased>(this);

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_nodeID, _scene, _canvas, config.inputBackgroundTexture);
        nineSliceSprite->nineSliceSize = config.inputSize;
        nineSliceSprite->color = config.inputBackgroundColor;

        auto _placeholderID = _canvas->getGraph()->createNode("Placeholder", _nodeID);
        placeholderTextRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_placeholderID, _scene, _canvas, config.placeholderText, config.font);
        placeholderTextRenderer->batchPriority = BatchPriority::SpritePriority;
        placeholderTextRenderer->color = config.placeholderTextColor;
        placeholderTextRenderer->pivot = { 0, 0.5f };
        placeholderTextRenderer->enabled = config.showPlaceholderText;
        auto* _placeholderTransform = _canvas->getGraph()->getComponent<Transform>(_placeholderID);
        auto _placeholderPosition = _placeholderTransform->getPosition();
        _placeholderTransform->setPosition(_placeholderPosition.x - config.inputSize.x / 2.f + config.textsOffsetFromLeft.x, _placeholderPosition.y + config.textsOffsetFromLeft.y);

        auto _textID = _canvas->getGraph()->createNode("Text", _nodeID);
        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_textID, _scene, _canvas, config.text, config.font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->color = config.textColor;
        textRenderer->pivot = { 0, 0.5f };
        auto* _textTransform = _canvas->getGraph()->getComponent<Transform>(_textID);
        auto _textPosition = _textTransform->getPosition();
        _textTransform->setPosition(_textPosition.x - config.inputSize.x / 2.f + config.textsOffsetFromLeft.x, _textPosition.y + config.textsOffsetFromLeft.y);
    }

    Vec2F UIInput::getSize() const {
        return config.inputSize;
    }

    UIInputConfig UIInput::getConfig() {
        return config;
    }

    void UIInput::setConfig(Scene* _scene, const UIInputConfig& _config) {
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

        if(config.inputBackgroundTexture == nullptr) {
            config.inputBackgroundTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "inputThemeDark");
        }

        if(config.font == nullptr) {
            config.font = _scene->engine->manager.fontManager.getSpecificFont(_scene->engine->manager.fileManager, "MontserratRegular", 40);
        }

        UI::interaction->sizeOfInteraction = config.inputSize;

        if(nineSliceSprite != nullptr) {
            nineSliceSprite->nineSliceSize = config.inputSize;
            nineSliceSprite->color = config.inputBackgroundColor;
        }

        if(textRenderer != nullptr) {
            textRenderer->setText(config.text);
            textRenderer->color = config.textColor;
            textRenderer->setFont(config.font);
        }

        if(placeholderTextRenderer != nullptr) {
            placeholderTextRenderer->setText(config.placeholderText);
            placeholderTextRenderer->color = config.placeholderTextColor;
            placeholderTextRenderer->setFont(config.font);
            placeholderTextRenderer->enabled = config.showPlaceholderText;
        }
    }

    void UIInput::setInteractable(bool _disabled) {

    }

    bool UIInput::isInteractable() {
        return false;
    }

    void UIInput::setColor(const Color& _color) {
        nineSliceSprite->color = _color;
    }

    Color UIInput::getColor() {
        return nineSliceSprite->color;
    }

    void UIInput::onMouseEntered() {

    }

    void UIInput::onMouseExited() {

    }

    void UIInput::onMouseClicked(MouseCode _mouseCode) {

    }

    void UIInput::onMouseReleased(MouseCode _mouseCode) {

    }
}