//
// Created by borja on 10/21/22.
//

#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/Components.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UI9Slice.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIMask.h"

namespace RDE {

    UIInput::UIInput(Node* _node, Manager* _manager, Graph* _graph, const UIInputConfig& _config) : UI(_node, &_config) {
        UI::texture = _config.inputBackgroundTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "inputThemeDark") :
                      _config.inputBackgroundTexture;

        ((UITransform*)node->getTransform())->setSize({ _config.inputSize.x, _config.inputSize.y });

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->onInnerMouseEntered.bind<&UIInput::onMouseEntered>(this);
        UI::interaction->onInnerMouseExited.bind<&UIInput::onMouseExited>(this);
        UI::interaction->onInnerClicking.bind<&UIInput::onMouseClicked>(this);
        UI::interaction->onInnerClickingReleased.bind<&UIInput::onMouseReleased>(this);
        UI::interaction->onInnerKeyPressed.bind<&UIInput::onKeyPressed>(this);
        UI::interaction->onInnerKeyReleased.bind<&UIInput::onKeyReleased>(this);
        UI::interaction->onInnerUnfocused.bind<&UIInput::onUnfocused>(this);

        node->addComponent<UIMask>();

        nineSliceSprite = node->addComponent<UI9Slice>(UI9SliceConfig {
            .size = UI::getSize(),
            .texture = UI::texture,
            .color = _config.inputBackgroundColor
        });
        nineSliceTransform = _node->getTransform();

        auto _placeholderNode = _graph->createNode("Placeholder", _node);
        placeholderTextRenderer = _placeholderNode->addComponent<UIText>(UITextConfig {
            .font = _config.font == nullptr ? _manager->fontManager.getSpecificFont("MontserratRegular", 40) :
                    _config.font,
            .text = _config.placeholderText,
            .textColor = _config.placeholderTextColor
        });
        placeholderTextRenderer->batchPriority = BatchPriority::SpritePriority;
        placeholderTextRenderer->setEnabled(_config.showPlaceholderText);
        auto* _placeholderTransform = _placeholderNode->getTransform();
        ((UITransform*)_placeholderTransform)->setAnchor(Anchor::LEFT);
        auto _placeholderPosition = _placeholderTransform->getPosition();
        _placeholderTransform->setPosition(_placeholderPosition.x + placeholderTextRenderer->getSize().x * 0.5f - UI::getSize().x * 0.5f + _config.textsOffsetFromLeft.x,
                                           _placeholderPosition.y + _config.textsOffsetFromLeft.y);

        auto _textNode = _graph->createNode("Text", _node);
        textRenderer = _textNode->addComponent<UIText>(UITextConfig {
                .font = _config.font == nullptr ? _manager->fontManager.getSpecificFont("MontserratRegular", 40) :
                        _config.font,
                .text = _config.text,
                .textColor = _config.textColor
        });
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textTransform = _textNode->getTransform();
        auto _textPosition = textTransform->getPosition();
        ((UITransform*)textTransform)->setAnchor(Anchor::LEFT);
        textRenderer->setOriginOffset({ textRenderer->getTextSize().x * 0.5f, 0.f });
        textTransform->setPosition(_textPosition.x + textRenderer->getSize().x * 0.5f + _config.textsOffsetFromLeft.x,
                                   _textPosition.y + _config.textsOffsetFromLeft.y);

        auto _caretNode = _graph->createNode("Caret", _node);
        auto* _caretTexture = _config.caretTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "caret") :
                              _config.caretTexture;
        caretSprite = _caretNode->addComponent<UIImage>(UIImageConfig {
            .texture = _caretTexture,
            .color = _config.caretColor
        });
        caretSprite->setBatchPriority(BatchPriority::SpritePriority);
        caretSprite->setEnabled(false);
        caretTransform = _caretNode->getTransform();
        ((UITransform*)caretTransform)->setAnchor(Anchor::LEFT);
        auto _caretPosition = caretTransform->getPosition();
        caretTransform->setPosition(_textPosition.x + textRenderer->getSize().x - UI::getSize().x * 0.5f + _config.textsOffsetFromLeft.x,
                                    _caretPosition.y + _config.textsOffsetFromLeft.y);

        blinkingTimeSeconds = _config.blinkingTimeSeconds;
        showPlaceholderText = _config.showPlaceholderText;
        caretHeight = _config.caretHeight;
        caretTexture = _caretTexture;
        textOffsetFromLeft = _config.textsOffsetFromLeft;
    }

    void UIInput::setInteractable(bool _interactable) {
        UI::interaction->interactable = _interactable;

        if(textRenderer != nullptr) {
            textRenderer->color = _interactable ? Color::Disabled : textRenderer->getColor();
        }
    }

    bool UIInput::isInteractable() {
        return UI::interaction->interactable;
    }

    void UIInput::setColor(const Color& _color) {
        nineSliceSprite->color = _color;
    }

    Color UIInput::getColor() {
        return nineSliceSprite->color;
    }

    void UIInput::onUpdate(Delta _dt) {
        IRenderizable::onUpdate(_dt);
        if(!usable()) return;

        if(UI::interaction != nullptr && UI::interaction->focused && caretSprite != nullptr) {
            if(blinkingTimer > blinkingTimeSeconds) {
                caretSprite->setEnabled(!caretSprite->isEnabled());
                blinkingTimer = 0;
            }

            blinkingTimer += _dt;
        }
    }

    void UIInput::onMouseEntered() {
        if(!usable()) return;

        updatePlaceholder();
    }

    void UIInput::onMouseExited() {
        if(!usable()) return;

        updatePlaceholder();
    }

    void UIInput::onMouseClicked(MouseCode _mouseCode) {
        if(!usable()) return;

        #if IS_ANDROID()
        SDL_StartTextInput();
        #elif IS_IOS()
        SDL_StartTextInput();
        #endif

        caretSprite->setEnabled(true);
        UI::interaction->focused = true;
        updatePlaceholder();
        blinkingTimer = 0;
    }

    void UIInput::onUnfocused() {
        if(!usable()) return;

        #if IS_ANDROID()
        SDL_StopTextInput();
        #elif IS_IOS()
        SDL_StopTextInput();
        #endif

        caretSprite->setEnabled(false);
    }

    void UIInput::onMouseReleased(MouseCode _mouseCode) {
        if(!usable()) return;

        updatePlaceholder();
    }

    // TODO: caret is not in the right place when the text goes out of scope on the right
    void UIInput::onKeyPressed(KeyCode _keyCode, char _char) {
        if(!usable()) return;

        if(_keyCode == KeyCode::Enter || _keyCode == KeyCode::Escape) {
            UI::interaction->focused = false;
        } else if(_keyCode == KeyCode::Delete) {
            updateCaret();
        } else if(_keyCode == KeyCode::Backspace) {
            if(pointer - 1 >= 0) {
                auto _text = textRenderer->getText();
                _text = _text.erase(pointer - 1, 1);
                pointer--;
                textRenderer->setText(_text);
            }
            updateCaret();
        } else if(_keyCode == KeyCode::Left) {
            pointer = pointer - 1 > 0 ? pointer - 1 : 0;
            updateCaret();
        } else if(_keyCode == KeyCode::Right) {
            auto _textLength = textRenderer->getText().size();
            pointer = pointer + 1 < _textLength ? pointer + 1 : (int)_textLength;
            updateCaret();
        } else {
            if(_char >= 32 && _char <= 126) {
                auto _text = textRenderer->getText();
                _text.insert(pointer, 1, _char);
                textRenderer->setText(_text);

                if(pointer == _text.size() - 1) {
                    pointer++;
                }

                updateCaret();
            }
        }

        textRenderer->setOriginOffset({ textRenderer->getTextSize().x * 0.5f, 0.f });
        updatePlaceholder();
    }

    void UIInput::onKeyReleased(KeyCode _keyCode, char _char) {

    }

    void UIInput::updatePlaceholder() {
        if(placeholderTextRenderer != nullptr) {
            if(UI::interaction->focused) {
                placeholderTextRenderer->setEnabled(textRenderer->getText().empty());
            } else {
                if(textRenderer != nullptr) {
                    placeholderTextRenderer->setEnabled(textRenderer->getText().empty());
                } else {
                    placeholderTextRenderer->setEnabled(showPlaceholderText);
                }
            }
        }
    }

    void UIInput::updateCaret() {
        if(!usable()) return;

        float _width = textRenderer->getTextSize().x;
        if(_width * 0.5f * UI::node->getTransform()->getScale().x + textOffsetFromLeft.x * 2.f >= getSize().x) {
            textDisplacement = (_width * 0.5f * UI::node->getTransform()->getScale().x + textOffsetFromLeft.x) - getSize().x;
        } else {
            textDisplacement = 0;
        }

        auto _origin = Vec2F { nineSliceTransform->getModelMatrixPosition().x - getSize().x * 0.5f + textOffsetFromLeft.x, nineSliceTransform->getModelMatrixPosition().y };
        textTransform->setMatrixModelPosition({_origin.x - textDisplacement, _origin.y});

        auto _caretPosition = caretTransform->getPosition();
        caretTransform->setPosition(textTransform->getPosition().x + _width * UI::node->getTransform()->getScale().x, _caretPosition.y);

        caretTransform->setScale(caretTransform->getScale().x, (caretHeight * getSize().y) / caretTexture->getSize().y);
    }

    bool UIInput::usable() {
        return textRenderer != nullptr && UI::interaction->interactable && nineSliceSprite->isEnabled();
    }
}