//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/Components.h"
#include "core/Engine.h"

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

//        auto _handleID = _canvas->getGraph()->createNode("Handle", _nodeID);
//        handleSprite = _canvas->getGraph()->addComponent<SpriteRenderer>(_handleID, _scene, _canvas, config.handleTexture);
//        handleSprite->color = config.fillingBarColor;
//        handleTransform = _canvas->getGraph()->getComponent<Transform>(_handleID);

        setConfig(_scene, _config);
    }

    Vec2F UISlider::getSize() const {
        return UI::interaction->sizeOfInteraction;
    }

    UISliderConfig UISlider::getConfig() {
        return config;
    }

    void UISlider::setConfig(Scene* _scene, const UISliderConfig& _config) {
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

        if(config.backgroundBarTexture == nullptr) {
            config.backgroundBarTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "fillAndBgrScrollBarHorizontal");
        }

        if(config.fillingBarTexture == nullptr) {
            config.fillingBarTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "fillAndBgrScrollBarHorizontal");
        }

        if(config.handleTexture == nullptr) {
            config.handleTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "handle");
        }


        UI::interaction->sizeOfInteraction = config.barSize;

        if(backgroundBarSprite != nullptr) {
            backgroundBarSprite->nineSliceSize = config.barSize;
            backgroundBarSprite->color = config.backgroundBarColor;
        }

        if(fillBarSprite != nullptr) {
            fillBarSprite->nineSliceSize = config.barSize;
            fillBarSprite->color = config.fillingBarColor;
        }

        if(handleSprite != nullptr) {
            handleSprite->color = config.handleColor;
            handleTransform->setScale(1.5f * config.barSize.y / handleSprite->getSize().x, 1.5f * config.barSize.y / handleSprite->getSize().y);
        }
    }

    void UISlider::setInteractable(bool _disabled) {

    }

    bool UISlider::isInteractable() {
        return UI::interaction->interactable;
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