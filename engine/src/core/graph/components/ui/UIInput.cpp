//
// Created by borja on 10/21/22.
//

#include "core/graph/components/ui/UIInput.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/graph/components/ui/UIText.h"
#include "core/graph/components/ui/UIMask.h"
#include "core/graph/components/ui/UIImage.h"
#include "core/render/elements/Batch.h"
#include "core/render/elements/IRenderizable.h"
#include "core/util/Functions.h"
#include <SDL_keyboard.h>

namespace RDE {

    UIInput::UIInput(Node* _node, Manager* _manager, Graph* _graph, const UIInputConfig& _config) {
        data.RenderizableInnerData.texture = _config.inputBackgroundTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "inputThemeDark") : _config.inputBackgroundTexture;
        data.RenderizableInnerData.color = _config.inputBackgroundColor;
        data.RenderizableInnerData.renderizableType = RenderizableType::RT_UI_IMAGE;

        node = _node;

        if(!_node->hasComponent<UIInteractable>()) {
            uiInteractable = _node->addComponent<UIInteractable>();
        }
                                                                                                           
        if(_config.stopFurtherClicks) {
            if(!node->hasComponent<CanvasEventStopper>()) {
                node->addComponent<CanvasEventStopper>();
            }
        } else {
            if (node->hasComponent<CanvasEventStopper>()) {
                node->removeComponent<CanvasEventStopper>();
            }
        }

        ((UITransform*)node->getTransform())->setSize({ _config.inputSize.x, _config.inputSize.y });

        uiInteractable->onInnerMouseEntered.bind<&UIInput::onMouseEntered>(this);
        uiInteractable->onInnerMouseExited.bind<&UIInput::onMouseExited>(this);
        uiInteractable->onInnerClicking.bind<&UIInput::onMouseClicked>(this);
        uiInteractable->onInnerClickingReleased.bind<&UIInput::onMouseReleased>(this);
        uiInteractable->onInnerKeyPressed.bind<&UIInput::onKeyPressed>(this);
        uiInteractable->onInnerKeyReleased.bind<&UIInput::onKeyReleased>(this);
        uiInteractable->onInnerTextTyped.bind<&UIInput::onTextTyping>(this);
        uiInteractable->onInnerUnfocused.bind<&UIInput::onUnfocused>(this);

        node->addComponent<UIMask>();

        background = node->addComponent<UIImage>(UIImageConfig {
            .size = getSize(),
            .texture = data.RenderizableInnerData.texture,
            .color = _config.inputBackgroundColor,
            .imageRenderingType = ImageRenderingType::NINE_SLICE
        });
        nineSliceTransform = _node->getTransform();
        data.RenderizableInnerData.batchPriority = BatchPriority::SpritePriority;
        data.RenderizableInnerData.shader = background->getShaderID();

        auto _placeholderNode = _graph->createNode("Placeholder", _node);
        placeholderTextRenderer = _placeholderNode->addComponent<UIText>(UITextConfig {
            .font = _config.font == nullptr ? _manager->fontManager.getSpecificFont("MontserratRegular", 40) :
                    _config.font,
            .text = _config.placeholderText,
            .textColor = _config.placeholderTextColor
        });
        placeholderTextRenderer->data.RenderizableInnerData.batchPriority = BatchPriority::SpritePriority;
        placeholderTextRenderer->setEnabled(_config.showPlaceholderText);
        auto* _placeholderTransform = _placeholderNode->getTransform();
        ((UITransform*)_placeholderTransform)->setAnchor(Anchor::LEFT);
        auto _placeholderPosition = _placeholderTransform->getPosition();
        _placeholderTransform->setPosition(_placeholderPosition.x + placeholderTextRenderer->getSize().x * 0.5f - getSize().x * 0.5f + _config.textsOffsetFromLeft.x,
                                           _placeholderPosition.y + _config.textsOffsetFromLeft.y);

        auto _textNode = _graph->createNode("Text", _node);
        textRenderer = _textNode->addComponent<UIText>(UITextConfig {
                .font = _config.font == nullptr ? _manager->fontManager.getSpecificFont("MontserratRegular", 40) :
                        _config.font,
                .text = _config.text,
                .textColor = _config.textColor
        });
        textRenderer->data.RenderizableInnerData.batchPriority = BatchPriority::SpritePriority;
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
        caretSprite->data.RenderizableInnerData.batchPriority = BatchPriority::SpritePriority;
        caretSprite->setEnabled(false);
        caretTransform = _caretNode->getTransform();
        ((UITransform*)caretTransform)->setAnchor(Anchor::LEFT);
        auto _caretPosition = caretTransform->getPosition();
        caretTransform->setPosition(_textPosition.x + textRenderer->getSize().x - getSize().x * 0.5f + _config.textsOffsetFromLeft.x,
                                    _caretPosition.y + _config.textsOffsetFromLeft.y);

        blinkingTimeSeconds = _config.blinkingTimeSeconds;
        showPlaceholderText = _config.showPlaceholderText;
        caretHeight = _config.caretHeight;
        caretTexture = _caretTexture;
        textOffsetFromLeft = _config.textsOffsetFromLeft;
    }


    ENABLED_DEFAULT_IMPL(UIInput)
    SIZE_METHODS_DEFAULT_IMPL(UIInput)


    void UIInput::setInteractable(bool _interactable) {
        uiInteractable->interactable = _interactable;
        SAFE_POINTER(textRenderer, setColor(_interactable ? Color::Disabled : textRenderer->getColor()))
    }

    bool UIInput::isInteractable() const {
        return uiInteractable->interactable;
    }

    void UIInput::setColor(const Color& _color) {
        SAFE_POINTER(background, setColor(_color))
        data.RenderizableInnerData.color = _color;
    }

    Color UIInput::getColor() const {
        return data.RenderizableInnerData.color;
    }


    void UIInput::setLayer(int _layer) {
        SAFE_POINTER(background, setLayer(_layer))
        SAFE_POINTER(caretSprite, setLayer(_layer))
        SAFE_POINTER(textRenderer, setLayer(_layer))
        SAFE_POINTER(placeholderTextRenderer, setLayer(_layer))
        data.RenderizableInnerData.layer = _layer;
    }

    int UIInput::getLayer() const {
        return data.RenderizableInnerData.layer;
    }


    void UIInput::setTexture(Texture* _texture) {
        SAFE_POINTER(background, setTexture(_texture));
        data.RenderizableInnerData.texture = _texture;
    }

    Texture* UIInput::getTexture() const {
        return data.RenderizableInnerData.texture;
    }


    void UIInput::setOriginOffset(const Vec2F& _offset) {
        SAFE_POINTER(background, setOriginOffset(_offset))
        SAFE_POINTER(caretSprite, setOriginOffset(_offset))
        SAFE_POINTER(textRenderer, setOriginOffset(_offset))
        SAFE_POINTER(placeholderTextRenderer, setOriginOffset(_offset))
        data.originOffset = _offset;
    }

    Vec2F UIInput::getOriginOffset() const {
        return data.originOffset;
    }


    void UIInput::setShaderID(ShaderID _shaderID) {
        SAFE_POINTER(background, setShaderID(_shaderID))
        data.RenderizableInnerData.shader = _shaderID;
    }

    ShaderID UIInput::getShaderID() const {
        return data.RenderizableInnerData.shader;
    }


    void UIInput::onUpdate(Delta _dt) {
        if(!usable()) return;

        if(uiInteractable != nullptr && uiInteractable->focused && caretSprite != nullptr) {
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

        SDL_StartTextInput();

        caretSprite->setEnabled(true);
        uiInteractable->focused = true;
        updatePlaceholder();
        blinkingTimer = 0;
    }

    void UIInput::onUnfocused() {
        if(!usable()) return;

        SDL_StopTextInput();
        caretSprite->setEnabled(false);
    }

    void UIInput::onMouseReleased(MouseCode _mouseCode) {
        if(!usable()) return;

        updatePlaceholder();
    }

    void UIInput::onTextTyping(const std::string& _text) {
        auto _innerText = textRenderer->getText();
        _innerText.insert(pointer, _text);
        textRenderer->setText(_innerText);

        if(pointer == _innerText.size() - 1) {
            pointer++;
        }

        updateCaret();

    }

    // TODO: caret is not in the right place when the text goes out of scope on the right
    void UIInput::onKeyPressed(KeyCode _keyCode) {
        if(!usable()) return;

        if(_keyCode == KeyCode::Enter || _keyCode == KeyCode::Escape) {
            uiInteractable->focused = false;
        } else if(_keyCode == KeyCode::Left) {
            pointer = pointer - 1 > 0 ? pointer - 1 : 0;
            updateCaret();
        } else if(_keyCode == KeyCode::Right) {
            auto _textLength = textRenderer->getText().size();
            pointer = pointer + 1 < _textLength ? pointer + 1 : (int)_textLength;
            updateCaret();
        }  else if(_keyCode == KeyCode::Delete) {
            updateCaret();
        } else if(_keyCode == KeyCode::Backspace) {
            if(pointer - 1 >= 0) {
                auto _text = textRenderer->getText();
                _text = _text.erase(pointer - 1, 1);
                pointer--;
                textRenderer->setText(_text);
            }
            updateCaret();
        }


        textRenderer->setOriginOffset({ textRenderer->getTextSize().x * 0.5f, 0.f });
        updatePlaceholder();
    }

    void UIInput::onKeyReleased(KeyCode _keyCode) {

    }

    void UIInput::updatePlaceholder() {
        if(placeholderTextRenderer != nullptr) {
            if(uiInteractable->focused) {
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
        if(_width * 0.5f * node->getTransform()->getScale().x + textOffsetFromLeft.x * 2.f >= getSize().x) {
            textDisplacement = (_width * 0.5f * node->getTransform()->getScale().x + textOffsetFromLeft.x) - getSize().x;
        } else {
            textDisplacement = 0;
        }

        auto _origin = Vec2F { nineSliceTransform->getModelMatrixPosition().x - getSize().x * 0.5f + textOffsetFromLeft.x, nineSliceTransform->getModelMatrixPosition().y };
        textTransform->setMatrixModelPosition({_origin.x - textDisplacement, _origin.y});

        if(caretTransform == nullptr) return;

        auto _caretPosition = caretTransform->getPosition();
        caretTransform->setPosition(textTransform->getPosition().x + _width * node->getTransform()->getScale().x, _caretPosition.y);

        caretTransform->setScale(caretTransform->getScale().x, (caretHeight * getSize().y) / caretTexture->getSize().y);
    }

    bool UIInput::usable() {
        return textRenderer != nullptr && uiInteractable->interactable && background->isEnabled();
    }

    Node* UIInput::getTextNode() {
        return textNode;
    }

    Node* UIInput::getTextPlaceholderNode() {
        return placeholderNode;
    }

    Node* UIInput::getCaretNode() {
        return caretNode;
    }
}