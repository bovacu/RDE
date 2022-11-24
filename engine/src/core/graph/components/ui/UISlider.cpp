//
// Created by borja on 10/20/22.
//

#include "core/graph/components/ui/UISlider.h"
#include "core/graph/components/Components.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"

namespace RDE {

    UISlider::UISlider(Node* _node, Manager* _manager, Graph* _graph, const UISliderConfig& _config) : UI(_node, &_config) {
        UI::texture = _config.backgroundBarTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "fillAndBgrScrollBarHorizontal") :
                      _config.backgroundBarTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->onInnerClicking.bind<&UISlider::onMouseClicked>(this);
        UI::interaction->onInnerClickingReleased.bind<&UISlider::onMouseReleased>(this);

        backgroundBarSprite = node->addComponent<UI9Slice>(UI9SliceConfig {
            .size = _config.barSize,
            .texture = _config.backgroundBarTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets","fillAndBgrScrollBarHorizontal") :
                _config.backgroundBarTexture,
            .color = _config.backgroundBarColor,
        });
        backgroundBarSprite->interaction = UI::interaction;
        backgroundBarTransform = (UITransform*)node->getTransform();

        auto _fillBarNode = _graph->createNode("Fill", node);
        fillBarSprite = _fillBarNode->addComponent<UI9Slice>(UI9SliceConfig {
            .size = backgroundBarTransform->getSize(),
            .texture = _config.fillingBarTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "fillAndBgrScrollBarHorizontal") :
                       _config.fillingBarTexture,
            .color = _config.fillingBarColor
        });

        fillBarTransform = (UITransform*)_fillBarNode->getTransform();
        fillBarTransform->setAnchor(Anchor::LEFT);
        fillBarTransform->setStretch(Stretch::HORIZONTAL_STRETCH);

        auto _handleNode = _graph->createNode("Handle", node);
        handleSprite = _handleNode->addComponent<UIImage>(UIImageConfig {
           .texture = _config.handleTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "handle") :
                      _config.handleTexture,
           .color = _config.handleColor
        });
        handleTransform = (UITransform*)_handleNode->getTransform();
        handleTransform->setScale(1.5f * _config.barSize.y / handleSprite->getSize().x, 1.5f * _config.barSize.y / handleSprite->getSize().y);
        handleTransform->setPosition(backgroundBarTransform->getPosition().x - (_config.barSize.x * 0.5f) + _config.barSize.x * _config.percentageFilled,
                                     handleTransform->getPosition().y);
        handleTransform->setAnchor(Anchor::RIGHT);

        setFilledPercentage(_config.percentageFilled);
    }

    void UISlider::setInteractable(bool _interactable) {
        UI::interaction->interactable = _interactable;
    }

    bool UISlider::isInteractable() {
        return UI::interaction->interactable;
    }

    void UISlider::onUpdate(Delta _dt) {
        IRenderizable::onUpdate(_dt);

        if(mouseDown) {
            auto _size = Vec2F { UI::getSize().x, UI::getSize().y };
            Vec2F _limits = { backgroundBarTransform->getModelMatrixPosition().x - _size.x * 0.5f,
                                 backgroundBarTransform->getModelMatrixPosition().x + _size.x * 0.5f };
            auto _posX = Util::Math::clampF(node->manager->inputManager.getMousePosWorldPos().x, _limits.v[0], _limits.v[1]);

            auto _distanceFromLowerPoint = _posX - _limits.v[0];
            setFilledPercentage(_distanceFromLowerPoint / (UI::getSize().x));
        }

        if(node->manager->inputManager.isMouseJustReleased(MouseCode::ButtonLeft) && mouseDown) {
            mouseDown = false;
        }
    }

    void UISlider::setFilledPercentage(float _percentage) {
        percentageFilled = Util::Math::clampF(_percentage, 0.f, 1.f);

        auto _width = ((UITransform*)node->getTransform())->getSize().x;
        auto _leftPos = node->getTransform()->getModelMatrixPosition().x - _width * 0.5f;
        handleTransform->setMatrixModelPosition({ _leftPos + percentageFilled * _width, handleTransform->getModelMatrixPosition().y});

        fillBarSprite->setSize({ UI::getSize().x * percentageFilled, UI::getSize().y });
        fillBarTransform->setPosition({handleTransform->getPosition().x - fillBarSprite->getSize().x * 0.5f, fillBarTransform->getPosition().y});
    }

    float UISlider::getFilledPercentage() const {
        return percentageFilled;
    }

    void UISlider::onMouseClicked(MouseCode _mouseCode) {
        mouseDown = true;
    }

    void UISlider::onMouseReleased(MouseCode _mouseCode) {
        mouseDown = false;
    }

}