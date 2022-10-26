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
        fillBarSprite->setSize({ config.barSize.x * clampF(config.percentageFilled, 0.f, 1.f), config.barSize.y });
        fillBarSprite->color = config.fillingBarColor;
        fillBarTransform = _fillBarNode->getTransform();

        auto _handleNode = _graph->createNode("Handle", _node);
        handleSprite = _handleNode->addComponent<UIImage>(config.handleTexture);
        handleSprite->color = config.fillingBarColor;
        handleTransform = _handleNode->getTransform();
        handleTransform->setPosition(backgroundBarTransform->getPosition().x - backgroundBarSprite->getSize().x / 2.f + backgroundBarSprite->getSize().x * config.percentageFilled,
                                     handleTransform->getPosition().y);

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
            backgroundBarSprite->setSize(config.barSize);
            backgroundBarSprite->color = config.backgroundBarColor;
        }

        if(fillBarSprite != nullptr) {
            setFilledPercentage(config.percentageFilled);
            fillBarSprite->color = config.fillingBarColor;
        }

        if(handleSprite != nullptr) {
            handleSprite->color = config.handleColor;
            handleTransform->setScale(1.5f * config.barSize.y / handleSprite->getSize().x, 1.5f * config.barSize.y / handleSprite->getSize().y);
            handleTransform->setPosition(backgroundBarTransform->getPosition().x - backgroundBarSprite->getSize().x / 2.f + backgroundBarSprite->getSize().x * config.percentageFilled,
                                         handleTransform->getPosition().y);
        }
    }

    void UISlider::setInteractable(bool _interactable) {
        UI::interaction->interactable = _interactable;
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
            Vec2F _limits = { backgroundBarTransform->getModelMatrixPosition().x - backgroundBarSprite->getSize().x / 2.f,
                                 backgroundBarTransform->getModelMatrixPosition().x + backgroundBarSprite->getSize().x / 2.f };

            auto _posX = clampF(node->manager->inputManager.getMousePosWorldPos().x, _limits.v[0], _limits.v[1]);
            handleTransform->setMatrixModelPosition({_posX, handleTransform->getModelMatrixPosition().y});

            auto _distanceFromLowerPoint = _posX - _limits.v[0];
            setFilledPercentage(_distanceFromLowerPoint / backgroundBarSprite->getSize().x);
        }

        if(node->manager->inputManager.isMouseJustReleased(MouseCode::ButtonLeft) && mouseDown) {
            mouseDown = false;
        }
    }

    void UISlider::setFilledPercentage(float _percentage) {
        config.percentageFilled = clampF(_percentage, 0.f, 1.f);
        fillBarSprite->setSize({ config.barSize.x * clampF(config.percentageFilled, 0.f, 1.f), config.barSize.y });
        fillBarTransform->setPosition({handleTransform->getPosition().x - fillBarSprite->getSize().x / 2.f, fillBarTransform->getPosition().y});
    }

    float UISlider::getFilledPercentage() const {
        return config.percentageFilled;
    }

    void UISlider::onMouseClicked(MouseCode _mouseCode) {
        mouseDown = true;
    }

    void UISlider::onMouseReleased(MouseCode _mouseCode) {
        mouseDown = false;
    }

}