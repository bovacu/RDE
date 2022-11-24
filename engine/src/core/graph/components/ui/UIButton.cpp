//
// Created by borja on 9/22/22.
//

#include "core/graph/components/ui/UIButton.h"
#include "core/graph/components/ui/UIText.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"

namespace RDE {

    UIButton::UIButton(Node* _node, Scene* _scene, Canvas* _canvas, const UIButtonConfig& _config) :
    UIButton(_node, &_scene->engine->manager, _canvas->getGraph(), _config) {  }

    UIButton::UIButton(Node* _node, Manager* _manager, Graph* _graph, const UIButtonConfig& _config) : UI(_node, &_config) {
        UI::texture = _config.idleTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "buttonDark") :
                      _config.idleTexture;

        statesTextures[0] = UI::texture;
        statesTextures[1] = _config.selectedTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "buttonDarkHighlited") :
                            _config.selectedTexture;
        statesTextures[2] = _config.clickedTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "buttonDarkPressed") :
                            _config.clickedTexture;
        statesTextures[3] = _config.disabledTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "buttonDarkLock") :
                            _config.disabledTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->onInnerMouseEntered.bind<&UIButton::onMouseEntered>(this);
        UI::interaction->onInnerMouseExited.bind<&UIButton::onMouseExited>(this);
        UI::interaction->onInnerClicking.bind<&UIButton::onMouseClicked>(this);
        UI::interaction->onInnerClickingReleased.bind<&UIButton::onMouseReleased>(this);

        ((UITransform*)node->getTransform())->setSize(_config.buttonSize);

        nineSliceSprite = _node->addComponent<UI9Slice>( UI9SliceConfig {
            .size = UI::getSize(),
            .texture = _config.idleTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "buttonDark") :
                       _config.idleTexture,
            .color = _config.buttonColor
        });
        nineSliceSprite->interaction = UI::interaction;

        auto _textNode = _graph->createNode("Text", _node);
        textRenderer = _textNode->addComponent<UIText>(UITextConfig {
            .font = _config.font,
            .text = _config.text,
            .textColor = _config.textColor
        });
        textRenderer->batchPriority = BatchPriority::SpritePriority;
    }

    void UIButton::onMouseEntered() {
        if(!interaction->interactable) return;

        nineSliceSprite->texture = statesTextures[1];
        nineSliceSprite->dirty = true;
    }

    void UIButton::onMouseExited() {
        if(!interaction->interactable) return;

        nineSliceSprite->texture = statesTextures[0];
        nineSliceSprite->dirty = true;
    }

    void UIButton::onMouseClicked(MouseCode _mouseCode) {
        if(!interaction->interactable) return;

        nineSliceSprite->texture = statesTextures[2];
        nineSliceSprite->dirty = true;
    }

    void UIButton::onMouseReleased(MouseCode _mouseCode) {
        if(!interaction->interactable) return;

        if(UI::interaction->mouseInnerStatus == UIInteractable::MouseExited) {
            nineSliceSprite->texture = statesTextures[0];
        } else {
            nineSliceSprite->texture = statesTextures[1];
        }

        nineSliceSprite->dirty = true;
    }

    void UIButton::setInteractable(bool _enabled) {
        interaction->interactable = _enabled;
        if(!interaction->interactable) {
            nineSliceSprite->texture = statesTextures[3];
        } else {
            onMouseReleased(MouseCode::ButtonLeft);
        }

        nineSliceSprite->dirty = true;
    }

    bool UIButton::isInteractable() {
        return interaction->interactable;
    }

    void UIButton::setColor(const Color& _color) {
        if(nineSliceSprite != nullptr) {
            nineSliceSprite->setColor(_color);
        }
        nineSliceSprite->dirty = true;
    }

    Color UIButton::getColor() {
        if(nineSliceSprite != nullptr) {
            return nineSliceSprite->getColor();
        }

        return Color::White;
    }
}