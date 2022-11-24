//
// Created by borja on 10/18/22.
//

#include "core/graph/components/ui/UICheckbox.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/Engine.h"

namespace RDE {

    UICheckbox::UICheckbox(Node* _node, Manager* _manager, Graph* _graph, const UICheckboxConfig& _config) : UI(_node, &_config) {
        UI::texture = _config.checkboxBackgroundTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "checkboxDark") :
                      _config.checkboxBackgroundTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->onInnerClickingReleased.bind<&UICheckbox::onMouseReleased>(this);
        UI::interaction->onInnerClicking.bind<&UICheckbox::onMouseClicked>(this);
        UI::interaction->onInnerMouseEntered.bind<&UICheckbox::onMouseEntered>(this);

        auto _textNode = _graph->createNode("Text", _node);
        textRenderer = _textNode->addComponent<UIText>(UITextConfig {
            .font = _config.font == nullptr ? _manager->fontManager.getDefaultFont("MontserratRegular") :
                    _config.font,
            .text = _config.text,
            .textColor = _config.textColor
        });
        textRenderer->setBatchPriority(BatchPriority::SpritePriority);

        auto _checkboxBackgroundNode = _graph->createNode("CheckboxBackground", _node);
        checkboxBackgroundSprite = _checkboxBackgroundNode->addComponent<UIImage>(UIImageConfig {
            .texture = UI::texture,
            .color = _config.checkboxColor
        });

        auto _tickNode = _graph->createNode("CheckboxTick", _checkboxBackgroundNode);
        tickSprite = _tickNode->addComponent<UIImage>(UIImageConfig {
            .texture = _config.checkboxTickTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "checkmark") :
                       _config.checkboxTickTexture,
            .color = _config.tickColor
        });

        ((UITransform*)node->getTransform())->setSize({ textRenderer->getSize() + checkboxBackgroundSprite->getSize() + _config.checkboxTextOffset });

        auto _size = Vec2F { checkboxBackgroundSprite->getSize().x + textRenderer->getSize().x,
                       checkboxBackgroundSprite->getSize().y > textRenderer->getSize().y ? checkboxBackgroundSprite->getSize().y : textRenderer->getSize().y
        } + Vec2F { _config.checkboxTextOffset.x, _config.checkboxTextOffset.y };
        ((UITransform*)node->getTransform())->setSize(_size);

        checked = _config.checked;
        checkboxTextOffset = _config.checkboxTextOffset;
    }

    void UICheckbox::onMouseReleased(MouseCode _mouseButton) {
        if(!interaction->interactable) return;

        if(UI::interaction->mouseInnerStatus == UIInteractable::MouseEntered) {
            setChecked(!checked);
            tickSprite->setEnabled(checked);
        }
    }

    void UICheckbox::setInteractable(bool _interactable) {
        interaction->interactable = _interactable;

        if(!interaction->interactable) {
            checkboxBackgroundSprite->setColor(Color::Disabled);
            tickSprite->setColor(Color::Disabled);
            textRenderer->setColor(Color::Disabled);
        } else {
            checkboxBackgroundSprite->setColor(checkboxBackgroundSprite->getColor() == Color::Disabled ? Color::White : checkboxBackgroundSprite->getColor());
            tickSprite->setColor(tickSprite->getColor() == Color::Disabled ? Color::White : tickSprite->getColor());
            textRenderer->setColor(textRenderer->getColor() == Color::Disabled ? Color::White : textRenderer->getColor());
        }
    }

    bool UICheckbox::isInteractable() {
        return interaction->interactable;
    }

    void UICheckbox::setChecked(bool _checked) {
        checked = _checked;
        if(!onClickedCheckbox.isEmpty()) onClickedCheckbox(checked);
    }

    bool UICheckbox::isChecked() const {
        return checked;
    }


}