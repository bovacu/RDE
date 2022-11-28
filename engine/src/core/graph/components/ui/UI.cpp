//
// Created by borja on 9/5/22.
//

#include "core/graph/components/ui/UI.h"
#include "core/graph/components/ui/UITransform.h"
#include "core/systems/eventSystem/MouseEvent.h"
#include "core/systems/eventSystem/KeyEvent.h"
#include "core/systems/eventSystem/ControllerEvent.h"
#include "core/systems/eventSystem/MobileEvent.h"
#include "core/Engine.h"

namespace RDE {

    UI::UI(Node* _node, const CommonUIConfig* _config) : IRenderizable(_node) {
        if(!_node->hasComponent<UIInteractable>()) {
            interaction = _node->addComponent<UIInteractable>();
        }

        if(_config->stopFurtherClicks) {
            if(!UI::node->hasComponent<CanvasEventStopper>()) {
                UI::node->addComponent<CanvasEventStopper>();
            }
        } else {
            if (UI::node->hasComponent<CanvasEventStopper>()) {
                UI::node->removeComponent<CanvasEventStopper>();
            }
        }
    }

    void UI::setOriginOffset(const Vec2F& _originOffset) {
        originOffset = _originOffset;
        ((UITransform*)node->getTransform())->setUIDirty();
    }

    Vec2F UI::getSize() const {
        auto* _transform = ((UITransform*)node->getTransform());
        return { _transform->getSize().x * _transform->getScale().x, _transform->getSize().y * _transform->getScale().y };
    }


    UIInteractable::UIInteractable(Node* _node, Manager* _manager, Graph* _graph) {}

    static bool trigger(Node* _node, Engine* _engine) {
        auto* _uiTransform = _node->getComponent<UITransform>();
        auto* _transform = _node->getTransform();
        auto _mousePos = _engine->manager.inputManager.getMousePosWorldPos();

        return _mousePos.isInside(_transform->getPosition(), Vec2F {(float)_uiTransform->getSize().x, (float)_uiTransform->getSize().y});
    }

    static char getCorrectChar(const KeyEvent* _keyEvent) {
        auto _modKeys = static_cast<const SDL_Keymod>(KMOD_SHIFT | KMOD_ALT | KMOD_CAPS);
        bool _modifications = (SDL_GetModState() & _modKeys) == SDL_GetModState();
        char _chosen = !_modifications ? _keyEvent->getChar() : _keyEvent->getAlternateChar();
        return _chosen;
    }

    static void shouldStopEventPropagation(Event& _event, Node* _node) {
        _event.handled = _node->hasComponent<CanvasEventStopper>() && _node->getComponent<CanvasEventStopper>()->isEnabled();
    }

    void UIInteractable::onEvent(Node* _node, Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event) {
        if (!_node->isActive() || !interactable) {
            return;
        }

        if(focused) {
            if(_eventDispatcher.dispatchEvent<KeyPressedEvent>() && !onInnerKeyPressed.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                auto* _kpe = (KeyPressedEvent*)&_event;
                onInnerKeyPressed(_kpe->getKeyCode(), getCorrectChar(_kpe));
            }

            if(_eventDispatcher.dispatchEvent<KeyReleasedEvent>() && !onInnerKeyReleased.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                auto* _kre = (KeyReleasedEvent*)&_event;
                onInnerKeyReleased(_kre->getKeyCode(), getCorrectChar(_kre));
            }

            if(_eventDispatcher.dispatchEvent<KeyPressedEvent>() && !onKeyPressed.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                auto* _kpe = (KeyPressedEvent*)&_event;
                onKeyPressed(_kpe->getKeyCode(), getCorrectChar(_kpe));
                return;
            }
        }

        auto _eventInsideElement = trigger(_node, _engine);

        if(_eventInsideElement && !_event.handled) {

            if(_eventDispatcher.dispatchEvent<MouseButtonReleasedEvent>() && !onInnerClickingReleased.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                auto* _mre = (MouseButtonReleasedEvent*)&_event;
                onInnerClickingReleased(_mre->getMouseButton());
            }

            if(_eventDispatcher.dispatchEvent<MouseButtonPressedEvent>() && !onInnerClicking.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                auto* _mre = (MouseButtonPressedEvent*)&_event;
                onInnerClicking(_mre->getMouseButton());
            }

            if(_eventDispatcher.dispatchEvent<MouseMovedEvent>() && !onInnerMouseEntered.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                if(mouseInnerStatus == MouseStatus::MouseExited) {
                    mouseInnerStatus = MouseStatus::MouseEntered;
                    onInnerMouseEntered();
                }
            }



            if(_eventDispatcher.dispatchEvent<MouseButtonReleasedEvent>() && !onClick.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                auto* _mre = (MouseButtonReleasedEvent*)&_event;
                onClick(_mre->getMouseButton());
                return;
            }

            if(_eventDispatcher.dispatchEvent<MouseScrolledEvent>() && !onScroll.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                auto* _mse = (MouseScrolledEvent*)&_event;
                onScroll({_mse->getScrollX(), _mse->getScrollY()});
                return;
            }




            if(_eventDispatcher.dispatchEvent<ControllerButtonUpEvent>() && !onGamepadButtonPressed.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                auto* _jbue = (ControllerButtonUpEvent*)&_event;
                onGamepadButtonPressed(_jbue->getButton());
                return;
            }

            if(_eventDispatcher.dispatchEvent<MobileTouchUpEvent>() && !onMobileClick.isEmpty()) {
                shouldStopEventPropagation(_event, _node);
                auto* _mtue = (MobileTouchUpEvent*)&_event;
                onMobileClick(_mtue->getFingerID());
                return;
            }

            return;
        }

        if(!_eventInsideElement && _eventDispatcher.dispatchEvent<MouseButtonPressedEvent>()) {
            focused = false;
            if(!onInnerUnfocused.isEmpty()) onInnerUnfocused();
        }

        if(_eventDispatcher.dispatchEvent<MouseMovedEvent>() && !_event.handled) {
            if(mouseInnerStatus == MouseStatus::MouseEntered) {
                shouldStopEventPropagation(_event, _node);
                if(!onInnerMouseExited.isEmpty()) onInnerMouseExited();
                mouseInnerStatus = MouseStatus::MouseExited;
            }
        }

        if(_eventDispatcher.dispatchEvent<MouseMovedEvent>() && !_event.handled) {
            if(mouseStatus == MouseStatus::MouseEntered) {
                shouldStopEventPropagation(_event, _node);
                if(!onMouseExited.isEmpty()) onMouseExited();
                mouseStatus = MouseStatus::MouseExited;
            }
        }
    }
}