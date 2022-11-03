//
// Created by borja on 10/21/22.
//

#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/Components.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UI9Slice.h"
#include "core/graph/components/ui/UIText.h"

namespace GDE {

    UIInput::UIInput(Node* _node, Manager* _manager, Graph* _graph, const UIInputConfig& _config) : UI(_node, _manager->sceneManager.getDisplayedScene()->getMainCamera()->getViewport()) {
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

        nineSliceSprite = _node->addComponent<UI9Slice>(config.inputBackgroundTexture);
        nineSliceSprite->interaction = UI::interaction;
        nineSliceSprite->setSize(config.inputSize);
        nineSliceSprite->setColor(config.inputBackgroundColor);
        nineSliceTransform = _node->getTransform();

        auto _placeholderNode = _graph->createNode("Placeholder", _node);
        placeholderTextRenderer = _placeholderNode->addComponent<UIText>(config.placeholderText, config.font);
        placeholderTextRenderer->batchPriority = BatchPriority::SpritePriority;
        placeholderTextRenderer->color = config.placeholderTextColor;
        placeholderTextRenderer->enabled = config.showPlaceholderText;
        auto* _placeholderTransform = _placeholderNode->getTransform();
        ((UITransform*)_placeholderTransform)->setPivot({ 0.f, 0.5f });
        auto _placeholderPosition = _placeholderTransform->getPosition();
        _placeholderTransform->setPosition(_placeholderPosition.x - config.inputSize.x * (float)viewport->getDeviceResolution().x / 2.f + config.textsOffsetFromLeft.x * (float)viewport->getDeviceResolution().x,
                                           _placeholderPosition.y + config.textsOffsetFromLeft.y * (float)viewport->getDeviceResolution().y);

        auto _textNode = _graph->createNode("Text", _node);
        textRenderer = _textNode->addComponent<UIText>(config.text, config.font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->setColor(config.textColor);
        textTransform = _textNode->getTransform();
        ((UITransform*)textTransform)->setPivot({ 0.0f, 0.5f });
        auto _textPosition = textTransform->getPosition();
        textTransform->setPosition(_textPosition.x - config.inputSize.x * (float)viewport->getDeviceResolution().x / 2.f + config.textsOffsetFromLeft.x * (float)viewport->getDeviceResolution().x,
                                   _textPosition.y + config.textsOffsetFromLeft.y * (float)viewport->getDeviceResolution().y);

        auto _caretNode = _graph->createNode("Caret", _node);
        caretSprite = _caretNode->addComponent<UIImage>(config.caretTexture);
        caretSprite->batchPriority = BatchPriority::SpritePriority;
        caretSprite->color = config.textColor;
        caretSprite->enabled = false;
        caretTransform = _caretNode->getTransform();
        auto _caretPosition = caretTransform->getPosition();
        auto _res = Vec2F { (float)viewport->getDeviceResolution().x, (float)viewport->getDeviceResolution().y };
        caretTransform->setPosition(_textPosition.x + textRenderer->getSize().x - config.inputSize.x * _res.x / 2.f + config.textsOffsetFromLeft.x * _res.x,
                                    _caretPosition.y + config.textsOffsetFromLeft.y * _res.y);

        setConfig(_manager, _config);
    }

    Vec2F UIInput::getSize() const {
        return config.inputSize;
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
            config.font = _manager->fontManager.getSpecificFont(_manager->fileManager, "MontserratRegular", 40);
        }

        auto _res = Vec2F { (float)viewport->getDeviceResolution().x, (float)viewport->getDeviceResolution().y };
        UI::interaction->sizeOfInteraction = Vec2F { config.inputSize.x * _res.x, config.inputSize.y * _res.y };

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
            caretTransform->setScale(caretTransform->getScale().x, config.caretHeight * _res.y / config.caretTexture->getSize().y);
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

        if(UI::interaction != nullptr && UI::interaction->focused && caretSprite != nullptr) {
            if(blinkingTimer > config.blinkingTimeSeconds) {
                caretSprite->enabled = !caretSprite->enabled;
                blinkingTimer = 0;
            }

            blinkingTimer += _dt;
        }
    }

    void UIInput::onMouseEntered() {
        updatePlaceholder();
    }

    void UIInput::onMouseExited() {
        updatePlaceholder();
    }

    void UIInput::onMouseClicked(MouseCode _mouseCode) {
        #if IS_ANDROID()
        SDL_StartTextInput();
        #elif IS_IOS()
        SDL_StartTextInput();
        #endif

        caretSprite->enabled = true;
        UI::interaction->focused = true;
        updatePlaceholder();
        blinkingTimer = 0;
    }

    void UIInput::onUnfocused() {
        #if IS_ANDROID()
        SDL_StopTextInput();
        #elif IS_IOS()
        SDL_StopTextInput();
        #endif

        caretSprite->enabled = false;
    }

    void UIInput::onMouseReleased(MouseCode _mouseCode) {
        updatePlaceholder();
    }

    void UIInput::onKeyPressed(KeyCode _keyCode, char _char) {
        if(textRenderer == nullptr || !UI::interaction->interactable) return;

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
        if(textRenderer == nullptr || caretSprite == nullptr) return;

        auto* _font = textRenderer->getFont();
        float _width = 0;

        for(auto _i = 0; _i < pointer; _i++) {
            _width += _font->getChars()[textRenderer->getText()[_i]].advance.x + textRenderer->spaceBetweenChars;
        }

        auto _res = Vec2F { (float)viewport->getDeviceResolution().x, (float)viewport->getDeviceResolution().y };
        if(_width / 2.f * UI::node->getTransform()->getScale().x + config.textsOffsetFromLeft.x * _res.x * 2.f >= getSize().x * _res.x) {
            textDisplacement = (_width / 2.f * UI::node->getTransform()->getScale().x + config.textsOffsetFromLeft.x * _res.y * 2.f) - getSize().x * _res.x;
        } else {
            textDisplacement = 0;
        }

        auto _origin = Vec2F { nineSliceTransform->getModelMatrixPosition().x - getSize().x * _res.x / 2.f + config.textsOffsetFromLeft.x * _res.x, nineSliceTransform->getModelMatrixPosition().y };
        textTransform->setMatrixModelPosition({_origin.x - textDisplacement, _origin.y});

        auto _caretPosition = caretTransform->getPosition();
        caretTransform->setPosition(textTransform->getPosition().x + _width / 2.f * UI::node->getTransform()->getScale().x, _caretPosition.y);

        caretTransform->setScale(caretTransform->getScale().x, config.caretHeight * _res.y / config.caretTexture->getSize().y);
    }
}