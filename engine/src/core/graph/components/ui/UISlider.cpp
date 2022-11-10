//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/Components.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"

namespace GDE {

    UISlider::UISlider(Node* _node, Manager* _manager, Graph* _graph, const UISliderConfig& _config) : UI(_node) {
        setConfig(_manager, _config);

        UI::texture = config.backgroundBarTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->onInnerClicking.bind<&UISlider::onMouseClicked>(this);
        UI::interaction->onInnerClickingReleased.bind<&UISlider::onMouseReleased>(this);

        auto _backgroundBarNode = _graph->createNode("Background", _node);
        backgroundBarSprite = _backgroundBarNode->addComponent<UI9Slice>(config.backgroundBarTexture);
        backgroundBarSprite->interaction = UI::interaction;
        backgroundBarSprite->setSize(config.barSize);
        backgroundBarSprite->setColor(config.backgroundBarColor);
        backgroundBarTransform = _backgroundBarNode->getTransform();

        auto _fillBarNode = _graph->createNode("Fill", _node);
        fillBarSprite = _fillBarNode->addComponent<UI9Slice>(config.fillingBarTexture);
        fillBarSprite->setColor(config.fillingBarColor);
        fillBarTransform = _fillBarNode->getTransform();

        auto _handleNode = _graph->createNode("Handle", _node);
        handleSprite = _handleNode->addComponent<UIImage>(config.handleTexture);
        handleSprite->setColor(config.handleColor);
        handleTransform = _handleNode->getTransform();

        setConfig(_manager, _config);
    }

    Vec2F UISlider::getSize() const {
        return ((UITransform*)node->getTransform())->getSize();
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
            config.backgroundBarTexture = _manager->textureManager.getSubTexture("defaultAssets", "fillAndBgrScrollBarHorizontal");
        }

        if(config.fillingBarTexture == nullptr) {
            config.fillingBarTexture = _manager->textureManager.getSubTexture("defaultAssets", "fillAndBgrScrollBarHorizontal");
        }

        if(config.handleTexture == nullptr) {
            config.handleTexture = _manager->textureManager.getSubTexture("defaultAssets", "handle");
        }

        auto _parentSize = ((UITransform*)node->getTransform())->getSize();
        ((UITransform*)node->getTransform())->setSize({config.barSize.x * _parentSize.x, config.barSize.y * _parentSize.y });

        if(backgroundBarSprite != nullptr) {
            backgroundBarSprite->setSize({ 1 , 1 });
            backgroundBarSprite->setColor(config.backgroundBarColor);
        }

        if(handleSprite != nullptr) {
            handleSprite->color = config.handleColor;
            handleTransform->setScale(1.5f * config.barSize.y * _parentSize.y / handleSprite->getSize().x, 1.5f * config.barSize.y * _parentSize.y / handleSprite->getSize().y);
            handleTransform->setPosition(backgroundBarTransform->getPosition().x - (config.barSize.x * _parentSize.x / 2.f) + config.barSize.x * _parentSize.x * config.percentageFilled,
                                         handleTransform->getPosition().y);
        }

        if(fillBarSprite != nullptr) {
            setFilledPercentage(config.percentageFilled);
            fillBarSprite->setColor(config.fillingBarColor);
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
            auto _parentSize = ((UITransform*)node->getTransform()->parentTransform)->getSize();
            auto _size = Vec2F { _parentSize.x * config.barSize.x, _parentSize.y * config.barSize.y };
            Vec2F _limits = { backgroundBarTransform->getModelMatrixPosition().x - _size.x * 0.5f,
                                 backgroundBarTransform->getModelMatrixPosition().x + _size.x * 0.5f };

            auto _posX = clampF(node->manager->inputManager.getMousePosWorldPos().x, _limits.v[0], _limits.v[1]);
            handleTransform->setMatrixModelPosition({_posX, handleTransform->getModelMatrixPosition().y});

            auto _distanceFromLowerPoint = _posX - _limits.v[0];
            setFilledPercentage(_distanceFromLowerPoint / (config.barSize.x * _parentSize.x));
        }

        if(node->manager->inputManager.isMouseJustReleased(MouseCode::ButtonLeft) && mouseDown) {
            mouseDown = false;
        }
    }

    void UISlider::setFilledPercentage(float _percentage) {
        auto _parentSize = ((UITransform*)fillBarTransform->parentTransform->node->getTransform()->parentTransform)->getSize();
        config.percentageFilled = clampF(_percentage, 0.f, 1.f);
        fillBarSprite->setSize({ config.percentageFilled, 1 });
        fillBarTransform->setPosition({handleTransform->getPosition().x - fillBarSprite->getSize().x * _parentSize.x * 0.5f, fillBarTransform->getPosition().y});
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