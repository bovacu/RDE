//
// Created by borja on 10/21/22.
//

#include "core/graph/components/ui/UIInput.h"
#include "core/graph/components/Components.h"
#include "core/Engine.h"

namespace GDE {

    UIInput::UIInput(const NodeID& _nodeID, Scene* _scene, Canvas* _canvas, const UIInputConfig& _config) : UI(_nodeID, _canvas) {
        setConfig(_scene, _config);

        UI::texture = config.inputBackgroundTexture;

        UI::shaderID = defaultShaders[SPRITE_RENDERER_SHADER];
        UI::batchPriority = BatchPriority::SpritePriority;

        UI::interaction->sizeOfInteraction = _config.inputSize;
        UI::interaction->onInnerMouseEntered.bind<&UIInput::onMouseEntered>(this);
        UI::interaction->onInnerMouseExited.bind<&UIInput::onMouseExited>(this);
        UI::interaction->onInnerClicking.bind<&UIInput::onMouseClicked>(this);
        UI::interaction->onInnerClickingReleased.bind<&UIInput::onMouseReleased>(this);
        UI::interaction->onInnerKeyPressed.bind<&UIInput::onKeyPressed>(this);
        UI::interaction->onInnerKeyReleased.bind<&UIInput::onKeyReleased>(this);
        UI::interaction->onInnerUnfocused.bind<&UIInput::onUnfocused>(this);

        nineSliceSprite = _canvas->getGraph()->addComponent<NineSliceSprite>(_nodeID, _scene, _canvas, config.inputBackgroundTexture);
        nineSliceSprite->nineSliceSize = config.inputSize;
        nineSliceSprite->color = config.inputBackgroundColor;
        nineSliceTransform = _canvas->getGraph()->getComponent<Transform>(_nodeID);

        auto _placeholderID = _canvas->getGraph()->createNode("Placeholder", _nodeID);
        placeholderTextRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_placeholderID, _scene, _canvas, config.placeholderText, config.font);
        placeholderTextRenderer->batchPriority = BatchPriority::SpritePriority;
        placeholderTextRenderer->color = config.placeholderTextColor;
        placeholderTextRenderer->pivot = { 0, 0.5f };
        placeholderTextRenderer->enabled = config.showPlaceholderText;
        auto* _placeholderTransform = _canvas->getGraph()->getComponent<Transform>(_placeholderID);
        auto _placeholderPosition = _placeholderTransform->getPosition();
        _placeholderTransform->setPosition(_placeholderPosition.x - config.inputSize.x / 2.f + config.textsOffsetFromLeft.x, _placeholderPosition.y + config.textsOffsetFromLeft.y);

        auto _textID = _canvas->getGraph()->createNode("Text", _nodeID);
        textRenderer = _canvas->getGraph()->addComponent<TextRenderer>(_textID, _scene, _canvas, config.text, config.font);
        textRenderer->batchPriority = BatchPriority::SpritePriority;
        textRenderer->color = config.textColor;
        textRenderer->pivot = { 0, 0.5f };
        textTransform = _canvas->getGraph()->getComponent<Transform>(_textID);
        auto _textPosition = textTransform->getPosition();
        textTransform->setPosition(_textPosition.x - config.inputSize.x / 2.f + config.textsOffsetFromLeft.x, _textPosition.y + config.textsOffsetFromLeft.y);

        auto _caretID = _canvas->getGraph()->createNode("Caret", _nodeID);
        caretSprite = _canvas->getGraph()->addComponent<SpriteRenderer>(_caretID, _scene, _canvas, config.caretTexture);
        caretSprite->batchPriority = BatchPriority::SpritePriority;
        caretSprite->color = config.textColor;
        caretSprite->pivot = { 0, 0.5f };
        caretSprite->enabled = false;
        caretTransform = _canvas->getGraph()->getComponent<Transform>(_caretID);
        auto _caretPosition = caretTransform->getPosition();
        caretTransform->setPosition(_textPosition.x + textRenderer->getSize().x - config.inputSize.x / 2.f + config.textsOffsetFromLeft.x, _caretPosition.y + config.textsOffsetFromLeft.y);

        setConfig(_scene, _config);
    }

    Vec2F UIInput::getSize() const {
        return config.inputSize;
    }

    UIInputConfig UIInput::getConfig() {
        return config;
    }

    void UIInput::setConfig(Scene* _scene, const UIInputConfig& _config) {
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

        if(config.inputBackgroundTexture == nullptr) {
            config.inputBackgroundTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "inputThemeDark");
        }

        if(config.caretTexture == nullptr) {
            config.caretTexture = _scene->engine->manager.textureManager.getSubTexture("assets", "caret");
        }

        if(config.font == nullptr) {
            config.font = _scene->engine->manager.fontManager.getSpecificFont(_scene->engine->manager.fileManager, "MontserratRegular", 40);
        }

        UI::interaction->sizeOfInteraction = config.inputSize;

        if(nineSliceSprite != nullptr) {
            nineSliceSprite->nineSliceSize = config.inputSize;
            nineSliceSprite->color = config.inputBackgroundColor;
        }

        if(textRenderer != nullptr) {
            pointer = 0;
            textRenderer->setText(config.text);
            textRenderer->color = config.textColor;
            textRenderer->setFont(config.font);
        }

        if(placeholderTextRenderer != nullptr) {
            placeholderTextRenderer->setText(config.placeholderText);
            placeholderTextRenderer->color = config.placeholderTextColor;
            placeholderTextRenderer->setFont(config.font);
            placeholderTextRenderer->enabled = config.showPlaceholderText;
        }

        if(caretSprite != nullptr) {
            caretTransform->setScale(caretTransform->getScale().x, config.caretHeight / config.caretTexture->getSize().y);
        }
    }

    void UIInput::setInteractable(bool _disabled) {
        UI::interaction->interactable = _disabled;

        if(textRenderer != nullptr) {
            textRenderer->color = _disabled ? Color::Disabled : config.textColor;
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

        if(_width / 2.f * transform->getScale().x + config.textsOffsetFromLeft.x * 2.f >= getSize().x) {
            textDisplacement = (_width / 2.f * transform->getScale().x + config.textsOffsetFromLeft.x * 2.f) - getSize().x;
        } else {
            textDisplacement = 0;
        }

        auto _origin = Vec2F { nineSliceTransform->getModelMatrixPosition().x - getSize().x / 2.f + config.textsOffsetFromLeft.x, nineSliceTransform->getModelMatrixPosition().y };
        textTransform->setMatrixModelPosition({_origin.x - textDisplacement, _origin.y});

        auto _caretPosition = caretTransform->getPosition();
        caretTransform->setPosition(textTransform->getPosition().x + _width / 2.f * transform->getScale().x, _caretPosition.y);
    }
}