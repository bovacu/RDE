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

    UIInput::UIInput(Node* _node, Manager* _manager, Graph* _graph, const UIInputConfig& _config) : UI(_node) {
        setConfig(_manager, _config);

        UI::texture = config.inputBackgroundTexture;

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

        nineSliceSprite = node->addComponent<UI9Slice>(config.inputBackgroundTexture);
        nineSliceSprite->interaction = UI::interaction;
        nineSliceSprite->setSize(config.inputSize);
        nineSliceSprite->setColor(config.inputBackgroundColor);
        nineSliceTransform = _node->getTransform();

        auto _placeholderNode = _graph->createNode("Placeholder", _node);
        placeholderTextRenderer = _placeholderNode->addComponent<UIText>(config.placeholderText, config.font);
        placeholderTextRenderer->batchPriority = BatchPriority::SpritePriority;
        placeholderTextRenderer->color = config.placeholderTextColor;
        placeholderTextRenderer->node->setEnabled(config.showPlaceholderText ? EnabledStates::DS_RENDER : INVERSE_ENABLED_STATE(EnabledStates::DS_RENDER));
        placeholderTextRenderer->setOriginOffset({placeholderTextRenderer->getSize().x * 0.5f, 0});
        auto* _placeholderTransform = _placeholderNode->getTransform();
        ((UITransform*)_placeholderTransform)->setAnchor(Anchor::LEFT);

        auto _placeholderPosition = _placeholderTransform->getPosition();

        _placeholderTransform->setPosition(_placeholderPosition.x - config.inputSize.x * 0.5f + config.textsOffsetFromLeft.x,
                                           _placeholderPosition.y + config.textsOffsetFromLeft.y);

        auto _textNode = _graph->createNode("Text", _node);
        textRenderer = _textNode->addComponent<UIText>(config.text, config.font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->setColor(config.textColor);
        textTransform = _textNode->getTransform();

        textRenderer->setOriginOffset({ textRenderer->getSize().x * 0.5f, 0.f });
        auto _textPosition = textTransform->getPosition();
        textTransform->setPosition(_textPosition.x - config.inputSize.x * 0.5f + config.textsOffsetFromLeft.x,
                                   _textPosition.y + config.textsOffsetFromLeft.y);

        auto _caretNode = _graph->createNode("Caret", _node);
        caretSprite = _caretNode->addComponent<UIImage>(config.caretTexture);
        caretSprite->setBatchPriority(BatchPriority::SpritePriority);
        caretSprite->setColor(config.textColor);
        caretSprite->node->setEnabled(INVERSE_ENABLED_STATE(EnabledStates::DS_RENDER));
        caretTransform = _caretNode->getTransform();
        ((UITransform*)caretTransform)->setAnchor(Anchor::LEFT);
        auto _caretPosition = caretTransform->getPosition();
        caretTransform->setPosition(_textPosition.x + textRenderer->getSize().x - config.inputSize.x * 0.5f + config.textsOffsetFromLeft.x,
                                    _caretPosition.y + config.textsOffsetFromLeft.y);

        setConfig(_manager, _config);
    }

    Vec2F UIInput::getSize() const {
        return ((UITransform*)node->getTransform())->getSize();
    }

    UIInputConfig UIInput::getConfig() {
        return config;
    }

    void UIInput::setConfig(Manager* _manager, const UIInputConfig& _config) {
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

        if(config.inputBackgroundTexture == nullptr) {
            config.inputBackgroundTexture = _manager->textureManager.getSubTexture("defaultAssets", "inputThemeDark");
        }

        if(config.caretTexture == nullptr) {
            config.caretTexture = _manager->textureManager.getSubTexture("defaultAssets", "caret");
        }

        if(config.font == nullptr) {
            config.font = _manager->fontManager.getSpecificFont("MontserratRegular", 40);
        }

        ((UITransform*)node->getTransform())->setSize({ config.inputSize.x, config.inputSize.y });

        if(nineSliceSprite != nullptr) {
            nineSliceSprite->setSize(config.inputSize);
            nineSliceSprite->setColor(config.inputBackgroundColor);
        }

        if(textRenderer != nullptr) {
            pointer = 0;
            textRenderer->setText(config.text);
            textRenderer->setColor(config.textColor);
            textRenderer->setFont(config.font);
        }

        if(placeholderTextRenderer != nullptr) {
            placeholderTextRenderer->setText(config.placeholderText);
            placeholderTextRenderer->setColor(config.placeholderTextColor);
            placeholderTextRenderer->setFont(config.font);
            placeholderTextRenderer->enabled = config.showPlaceholderText;
        }

        if(caretSprite != nullptr) {
            caretTransform->setScale(caretTransform->getScale().x, (config.caretHeight * getSize().y) / config.caretTexture->getSize().y);
        }
    }

    void UIInput::setInteractable(bool _interactable) {
        UI::interaction->interactable = _interactable;

        if(textRenderer != nullptr) {
            textRenderer->color = _interactable ? Color::Disabled : config.textColor;
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
            if(blinkingTimer > config.blinkingTimeSeconds) {
                caretSprite->node->setEnabled(
                        (uint8_t) (caretSprite->node->isEnabledStateOn(EnabledStates::DS_RENDER) ? INVERSE_ENABLED_STATE(EnabledStates::DS_RENDER) : EnabledStates::DS_RENDER));
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

        caretSprite->node->setEnabled(EnabledStates::DS_RENDER);
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

        caretSprite->node->setEnabled(INVERSE_ENABLED_STATE(EnabledStates::DS_RENDER));
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

        textRenderer->setOriginOffset({ textRenderer->getSize().x * 0.5f, 0.f });
        updatePlaceholder();
    }

    void UIInput::onKeyReleased(KeyCode _keyCode, char _char) {

    }

    void UIInput::updatePlaceholder() {
        if(placeholderTextRenderer != nullptr) {
            if(UI::interaction->focused) {
                placeholderTextRenderer->enabled = textRenderer->getText().empty();
            } else {
                if(textRenderer != nullptr) {
                    placeholderTextRenderer->enabled = textRenderer->getText().empty();
                } else {
                    placeholderTextRenderer->enabled = config.showPlaceholderText;
                }
            }
        }
    }

    void UIInput::updateCaret() {
        if(!usable()) return;

        auto* _font = textRenderer->getFont();
        float _width = 0;

        for(auto _i = 0; _i < pointer; _i++) {
            _width += _font->getChars()[textRenderer->getText()[_i]].advance.x + textRenderer->spaceBetweenChars;
        }

        if(_width * 0.5f * UI::node->getTransform()->getScale().x + config.textsOffsetFromLeft.x * 2.f >= getSize().x) {
            textDisplacement = (_width * 0.5f * UI::node->getTransform()->getScale().x + config.textsOffsetFromLeft.x) - getSize().x;
        } else {
            textDisplacement = 0;
        }

        auto _origin = Vec2F { nineSliceTransform->getModelMatrixPosition().x - getSize().x * 0.5f + config.textsOffsetFromLeft.x, nineSliceTransform->getModelMatrixPosition().y };
        textTransform->setMatrixModelPosition({_origin.x - textDisplacement, _origin.y});

        auto _caretPosition = caretTransform->getPosition();
        caretTransform->setPosition(textTransform->getPosition().x + _width * 0.5f * UI::node->getTransform()->getScale().x, _caretPosition.y);

        caretTransform->setScale(caretTransform->getScale().x, (config.caretHeight * getSize().y) / config.caretTexture->getSize().y);
    }

    bool UIInput::usable() {
        return textRenderer != nullptr && UI::interaction->interactable && node->isEnabledStateOn(EnabledStates::DS_UPDATE);
    }
}