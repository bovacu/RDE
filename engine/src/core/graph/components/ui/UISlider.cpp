//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UISlider.h"

namespace GDE {

    UISlider::UISlider(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UISliderConfig& _config) : UI(_nodeID, _canvas) {
        setConfig(_scene, _config);

        UI::texture = config.backgroundBarTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->sizeOfInteraction = _config.barSize;
        UI::interaction->onInnerClicking.bind<&UISlider::onMouseClicked>(this);
        UI::interaction->onInnerClickingReleased.bind<&UISlider::onMouseReleased>(this);

        auto _backgroundBarID = _canvas->getGraph()->createNode("Background", _nodeID);
        backgroundBarSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_backgroundBarID, _scene, _canvas, config.backgroundBarTexture);
        backgroundBarSprite->nineSliceSize = config.barSize;
        backgroundBarSprite->color = config.backgroundBarColor;
        backgroundBarTransform = _canvas->getGraph()->getComponent<Transform>(_backgroundBarID);

        auto _fillBarID = _canvas->getGraph()->createNode("Fill", _nodeID);
        fillBarSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_fillBarID, _scene, _canvas, config.fillingBarTexture);
        fillBarSprite->nineSliceSize = { config.barSize.x * clampF(config.percentageFilled, 0.f, 1.f), config.barSize.y };
        fillBarSprite->color = config.fillingBarColor;
        fillBarTransform = _canvas->getGraph()->getComponent<Transform>(_fillBarID);
    }

    Vec2F UISlider::getSize() const {
        return UI::getSize();
    }

    UISliderConfig UISlider::getConfig() {
        return UISliderConfig();
    }

    void UISlider::setConfig(Scene* _scene, const UISliderConfig& _config) {
//        config = _config;
//
//        if(config.stopFurtherClicks) {
//            if(!UI::canvas->getGraph()->hasComponent<CanvasEventStopper>(ID)) {
//                canvas->getGraph()->addComponent<CanvasEventStopper>(ID);
//            }
//        } else {
//            if(UI::canvas->getGraph()->hasComponent<CanvasEventStopper>(ID)) {
//                canvas->getGraph()->removeComponent<CanvasEventStopper>(ID);
//            }
//        }
//
//        if(config.inputBackgroundTexture == nullptr) {
//            config.inputBackgroundTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "inputThemeDark");
//        }
//
//        if(config.caretTexture == nullptr) {
//            config.caretTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "caret");
//        }
//
//        if(config.font == nullptr) {
//            config.font = _scene->engine->manager.fontManager.getSpecificFont(_scene->engine->manager.fileManager, "MontserratRegular", 40);
//        }
//
//        UI::interaction->sizeOfInteraction = config.inputSize;
//
//        if(nineSliceSprite != nullptr) {
//            nineSliceSprite->nineSliceSize = config.inputSize;
//            nineSliceSprite->color = config.inputBackgroundColor;
//        }
//
//        if(textRenderer != nullptr) {
//            pointer = 0;
//            textRenderer->setText(config.text);
//            textRenderer->color = config.textColor;
//            textRenderer->setFont(config.font);
//        }
//
//        if(placeholderTextRenderer != nullptr) {
//            placeholderTextRenderer->setText(config.placeholderText);
//            placeholderTextRenderer->color = config.placeholderTextColor;
//            placeholderTextRenderer->setFont(config.font);
//            placeholderTextRenderer->enabled = config.showPlaceholderText;
//        }
//
//        if(caretSprite != nullptr) {
//            caretTransform->setScale(caretTransform->getScale().x, config.caretHeight / config.caretTexture->getSize().y);
//        }
    }

    void UISlider::setInteractable(bool _disabled) {

    }

    bool UISlider::isInteractable() {
        return false;
    }

    void UISlider::setColor(const Color& _color) {
        IRenderizable::setColor(_color);
    }

    Color UISlider::getColor() {
        return IRenderizable::getColor();
    }

    void UISlider::onUpdate(Delta _dt) {
        IRenderizable::onUpdate(_dt);

        if(mouseDown) {

        }
    }

    void UISlider::onMouseClicked(MouseCode _mouseCode) {
        mouseDown = true;
    }

    void UISlider::onMouseReleased(MouseCode _mouseCode) {
        mouseDown = false;
    }

}