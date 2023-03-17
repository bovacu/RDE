//
// Created by borja on 10/21/22.
//

#include "core/graph/components/ui/UIInput.h"
#include "core/Engine.h"
#include "core/graph/components/ui/UIAnchoring.h"
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
        data.RenderizableInnerData.renderizableType = RDE_RENDERIZABLE_TYPE_UI_IMAGE;

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

		anchoring = node->getComponent<UIAnchoring>();	
        anchoring->setSize({ _config.inputSize.x, _config.inputSize.y });

        uiInteractable->onInnerMouseEntered.bind<&UIInput::onMouseEntered>(this);
        uiInteractable->onInnerMouseExited.bind<&UIInput::onMouseExited>(this);
        uiInteractable->onInnerClicking.bind<&UIInput::onMouseClicked>(this);
        uiInteractable->onInnerClickingReleased.bind<&UIInput::onMouseReleased>(this);
        uiInteractable->onInnerKeyPressed.bind<&UIInput::onKeyPressed>(this);
        uiInteractable->onInnerTextTyped.bind<&UIInput::onTextTyping>(this);
        uiInteractable->onInnerUnfocused.bind<&UIInput::onUnfocused>(this);

        node->addComponent<UIMask>();

        background = node->addComponent<UIImage>(UIImageConfig {
            .size = getSize(),
            .texture = data.RenderizableInnerData.texture,
            .color = _config.inputBackgroundColor,
            .imageRenderingType = RDE_IMAGE_RENDERING_TYPE_NINE_SLICE
        });
        nineSliceTransform = _node->getTransform();
        data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;
        data.RenderizableInnerData.shader = background->getShaderID();



        auto _placeholderNode = _graph->createNode("Placeholder", _node);
        placeholderTextRenderer = _placeholderNode->addComponent<UIText>(UITextConfig {
            .font = _config.font == nullptr ? _manager->fontManager.getFont("MontserratRegular") :
                    _config.font,
            .text = _config.placeholderText,
            .textColor = _config.placeholderTextColor
        });
        placeholderTextRenderer->data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;
		placeholderTextRenderer->setEnabled(_config.showPlaceholderText);
        auto* _placeholderTransform = _placeholderNode->getTransform();
        _placeholderNode->getComponent<UIAnchoring>()->setAnchor(RDE_UI_ANCHOR_LEFT);
        auto _placeholderPosition = _placeholderTransform->getPosition();
		_placeholderTransform->setPosition(_placeholderPosition.x + _config.textsOffsetFromLeft.x - getSize().x * 0.5f,
                                           _placeholderPosition.y + _config.textsOffsetFromLeft.y);

        
		
		
		auto _textNode = _graph->createNode("Text", _node);
        textRenderer = _textNode->addComponent<UIText>(UITextConfig {
                .font = _config.font == nullptr ? _manager->fontManager.getFont("MontserratRegular") :
                        _config.font,
                .text = _config.text,
                .textColor = _config.textColor
        });
        textRenderer->data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;
        textTransform = _textNode->getTransform();
        auto _textPosition = textTransform->getPosition();
		_textNode->getComponent<UIAnchoring>()->setAnchor(RDE_UI_ANCHOR_LEFT);
        textTransform->setPosition(_textPosition.x + textRenderer->getSize().x * 0.5f + _config.textsOffsetFromLeft.x - getSize().x * 0.5f,
                                   _placeholderTransform->getPosition().y);

        
		
		
		auto _caretNode = _graph->createNode("Caret", _node);
		_caretNode->getTransform()->setScale(2, 1);
        auto* _caretTexture = _config.caretTexture == nullptr ? _manager->textureManager.getSubTexture("defaultAssets", "caret") :
                              _config.caretTexture;
        caretSprite = _caretNode->addComponent<UIImage>(UIImageConfig {
            .texture = _caretTexture,
            .color = _config.caretColor
        });
        caretSprite->data.RenderizableInnerData.batchPriority = RDE_BATCH_PRIORITY_SPRITE;
        caretTransform = _caretNode->getTransform();
		auto _caretPosition = caretTransform->getPosition();
		_caretNode->getComponent<UIAnchoring>()->setAnchor(RDE_UI_ANCHOR_LEFT);
        caretTransform->setPosition(_textPosition.x - getSize().x * 0.5f + _config.textsOffsetFromLeft.x,
                                    _caretPosition.y);
		caretSprite->setEnabled(false);

        
		
		
		blinkingTimeSeconds = _config.blinkingTimeSeconds;
        showPlaceholderText = _config.showPlaceholderText;
        caretHeight = _config.caretHeight;
        caretTexture = _caretTexture;
        textOffsetFromLeft = _config.textsOffsetFromLeft;
		spaceOnTheRightToLimitText = _config.spaceOnTheRightToLimitText;
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

	void UIInput::onMouseClicked(RDE_MOUSE_BUTTON_ _mouseCode) {
        if(!usable()) return;

        SDL_StartTextInput();

		caretSprite->setEnabled(true);
        uiInteractable->focused = true;
        updatePlaceholder();
        updateCaretOther();
        blinkingTimer = 0;
    }

    void UIInput::onUnfocused() {
        if(!usable()) return;

        SDL_StopTextInput();
		caretSprite->setEnabled(false);
        updatePlaceholder();
    }

	void UIInput::onMouseReleased(RDE_MOUSE_BUTTON_ _mouseCode) {
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

		updateTextOnTextKey();
		updateCaretOnTextKey();
    }

	float UIInput::getMaximumAllowdWidth() {
		return textRenderer->getSize().x * node->getTransform()->getScale().x + spaceOnTheRightToLimitText;
	}

    void UIInput::onKeyPressed(RDE_KEYBOARD_KEY_ _keyCode) {
        if(!usable()) return;

		if(_keyCode == RDE_KEYBOARD_KEY_ENTER || _keyCode == RDE_KEYBOARD_KEY_ESCAPE) {
            uiInteractable->focused = false;
			onUnfocused();
		} else if(_keyCode == RDE_KEYBOARD_KEY_BAKCSPACE) {
            if(pointer - 1 >= 0) {
                auto _text = textRenderer->getText();
                _text = _text.erase(pointer - 1, 1);
                pointer--;
                textRenderer->setText(_text);
            }
			updateTextOnTextKey();
            updateCaretOnTextKey();
        }

        updatePlaceholder();
    }

    void UIInput::updatePlaceholder() {
        if(placeholderTextRenderer != nullptr) {
            if(uiInteractable->focused) {
				placeholderTextRenderer->setEnabled(false);
            } else {
                if(textRenderer != nullptr) {
					placeholderTextRenderer->setEnabled(textRenderer->getText().empty());
                } else {
					placeholderTextRenderer->setEnabled(showPlaceholderText);
                }
            }
        }
    }

	void UIInput::updateTextOnLeftKey() {
		
	}

	void UIInput::updateTextOnRightKey() {
		
	}

	void UIInput::updateTextOnBackspaceKey() {
		
	}

	void UIInput::updateTextOnTextKey() {
		auto _chars = textRenderer->getFont()->getChars();
		auto _maxAllowed = getSize().x - spaceOnTheRightToLimitText;
		auto _displacement = 0.f;
		auto _text = textRenderer->getText();

		auto _width = textOffsetFromLeft.x;
		for(auto _i = 0; _i < _text.size(); _i++) {
			_width += _chars[_text[_i]].advance.x * 0.5f;
		}

		if(_width >= _maxAllowed) {
			_displacement = _maxAllowed - _width;
		}

		textRenderer->setOriginOffset({ _displacement, 0.f });
	}

	void UIInput::updateTextOther() {
		auto _maxAllowed = getSize().x - spaceOnTheRightToLimitText;
		auto _displacement = 0.f;
		if(textRenderer->getSize().x > _maxAllowed) {
			_displacement = _maxAllowed - textRenderer->getSize().x;
		}

		textRenderer->setOriginOffset({ _displacement, 0.f });
	}



	void UIInput::updateCaretOnLeftKey() {
		auto _displacement = caretSprite->getOriginOffset();

		auto _font = textRenderer->getFont();
		auto _chars = _font->getChars();
		auto _text = textRenderer->getText();

		if(_displacement.x > 0) {
			auto _delta = Util::Math::clampF(_displacement.x - _chars[_text[pointer]].advance.x * 0.5f, 0, FLT_MAX);
			caretSprite->setOriginOffset({_delta, _displacement.y});
		}
	}

	void UIInput::updateCaretOnRightKey() {
		auto _maxAllowed = getSize().x - spaceOnTheRightToLimitText;
		auto _displacement = caretSprite->getOriginOffset();
		auto _font = textRenderer->getFont();
		auto _chars = _font->getChars();
		auto _text = textRenderer->getText();
			
		if(_displacement.x != _maxAllowed) {
			auto _delta = Util::Math::clampF(_displacement.x + _chars[_text[pointer - 1]].advance.x * 0.5f, 0, _maxAllowed);
			caretSprite->setOriginOffset({_delta, _displacement.y});
		}
	}

	void UIInput::updateCaretOnBackspaceKey() {
		
	}

	void UIInput::updateCaretOnTextKey() {
		auto _chars = textRenderer->getFont()->getChars();
		auto _displacement = 0.f;
		auto _text = textRenderer->getText();
		auto _maxAllowed = getSize().x - spaceOnTheRightToLimitText;
		
		for(auto _i = 0; _i < _text.size(); _i++) {
			_displacement += _chars[_text[_i]].advance.x * 0.5f;
			if(_displacement >= _maxAllowed) {
				_displacement = _maxAllowed;
				break;
			}
		}

		caretTransform->setScale(caretTransform->getScale().x, (caretHeight * getSize().y) / caretTexture->getSize().y);
		caretSprite->setOriginOffset({ _displacement, 0 });
	}

    void UIInput::updateCaretOther() {
        if(caretTransform == nullptr) return;
		auto _chars = textRenderer->getFont()->getChars();
		auto _text = textRenderer->getText();

		auto _displacement = caretSprite->getOriginOffset().x;
		_displacement = textRenderer->getSize().x;
		auto _maxAllowed = getSize().x - spaceOnTheRightToLimitText;
		if(_displacement > _maxAllowed) {
			_displacement = _maxAllowed;
		}

		if(_text.empty()) {
			_displacement = 0.f;
		}

		caretTransform->setScale(caretTransform->getScale().x, (caretHeight * getSize().y) / caretTexture->getSize().y);
		caretSprite->setOriginOffset({ _displacement, 0 });
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