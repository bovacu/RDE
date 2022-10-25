//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/Components.h"
#include "core/Engine.h"

namespace GDE {

    UISlider::UISlider(Node* _node, Manager* _manager, Graph* _graph, const UISliderConfig& _config) : UI(_node) {
        setConfig(_manager, _config);

        UI::texture = config.backgroundBarTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->sizeOfInteraction = _config.barSize;
        UI::interaction->onInnerClicking.bind<&UISlider::onMouseClicked>(this);
        UI::interaction->onInnerClickingReleased.bind<&UISlider::onMouseReleased>(this);

        auto _backgroundBarNode = _graph->createNode("Background", _node);
        backgroundBarSprite = _backgroundBarNode->addComponent<NineSliceSprite>(config.backgroundBarTexture);
        backgroundBarSprite->nineSliceSize = config.barSize;
        backgroundBarSprite->color = config.backgroundBarColor;
        backgroundBarTransform = _backgroundBarNode->getTransform();

        auto _fillBarNode = _graph->createNode("Fill", _node);
        fillBarSprite = _fillBarNode->addComponent<NineSliceSprite>(config.fillingBarTexture);
        fillBarSprite->nineSliceSize = { config.barSize.x * clampF(config.percentageFilled, 0.f, 1.f), config.barSize.y };
        fillBarSprite->color = config.fillingBarColor;
        fillBarTransform = _fillBarNode->getTransform();

//        auto _handleID = _canvas->getGraph()->createNode("Handle", _nodeID);
//        handleSprite = _canvas->getGraph()->addComponent<SpriteRenderer>(_handleID, _scene, _canvas, config.handleTexture);
//        handleSprite->color = config.fillingBarColor;
//        handleTransform = _canvas->getGraph()->getComponent<Transform>(_handleID);

        setConfig(_manager, _config);
    }

    Vec2F UISlider::getSize() const {
        return UI::interaction->sizeOfInteraction;
    }

    UISliderConfig UISlider::getConfig() {
        return config;
    }

    void UISlider::setConfig(Manager* _manager, const UISliderConfig& _config) {
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

        if(config.backgroundBarTexture == nullptr) {
            config.backgroundBarTexture = _manager->textureManager.getSubTexture("assets", "fillAndBgrScrollBarHorizontal");
        }

        if(config.fillingBarTexture == nullptr) {
            config.fillingBarTexture = _manager->textureManager.getSubTexture("assets", "fillAndBgrScrollBarHorizontal");
        }

        if(config.handleTexture == nullptr) {
            config.handleTexture = _manager->textureManager.getSubTexture("assets", "handle");
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