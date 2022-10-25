//
// Created by borja on 9/5/22.
//

#include "core/graph/components/ui/UI.h"
#include "core/systems/eventSystem/MouseEvent.h"
#include "core/systems/eventSystem/KeyEvent.h"
#include "core/systems/eventSystem/ControllerEvent.h"
#include "core/systems/eventSystem/MobileEvent.h"
#include "core/Engine.h"

namespace GDE {

    UI::UI(Node* _node) : IRenderizable(_node) {
        if(!_node->hasComponent<UIInteractable>()) {
            interaction = _node->addComponent<UIInteractable>();
        }
    }




    UIInteractable::UIInteractable(Node* _node, Manager* _manager, Graph* _graph) {  }

    static bool trigger(Node* _node, Engine* _engine, Canvas* _canvas) {
        auto* _ninePatch = _node->getComponent<UIInteractable>();
        auto* _transform = _node->getTransform();
        auto _mousePos = _engine->manager.inputManager.getMousePosWorldPos();

        return _mousePos.isInside(_transform->getPosition(), Vec2F {(float)_ninePatch->sizeOfInteraction.x, (float)_ninePatch->sizeOfInteraction.y});
    }

    static char getCorrectChar(const KeyEvent* _keyEvent) {
        auto _modKeys = static_cast<const SDL_Keymod>(KMOD_SHIFT | KMOD_ALT | KMOD_CAPS);
        bool _modifications = (SDL_GetModState() & _modKeys) == SDL_GetModState();
        char _chosen = !_modifications ? _keyEvent->getChar() : _keyEvent->getAlternateChar();
        return _chosen;
    }

    void UIInteractable::onEvent(Node* _node, Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event, Canvas* _canvas) {
        if (!_node->hasComponent<Active>() || !interactable) return;

        if(focused) {
            if(_eventDispatcher.dispatchEvent<KeyPressedEvent>() && !onInnerKeyPressed.isEmpty()) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                auto* _kpe = (KeyPressedEvent*)&_event;
                onInnerKeyPressed(_kpe->getKeyCode(), getCorrectChar(_kpe));
            }

            if(_eventDispatcher.dispatchEvent<KeyReleasedEvent>() && !onInnerKeyReleased.isEmpty()) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                auto* _kre = (KeyReleasedEvent*)&_event;
                onInnerKeyReleased(_kre->getKeyCode(), getCorrectChar(_kre));
            }

            if(_eventDispatcher.dispatchEvent<KeyPressedEvent>() && !onKeyPressed.isEmpty()) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                auto* _kpe = (KeyPressedEvent*)&_event;
                onKeyPressed(_kpe->getKeyCode(), getCorrectChar(_kpe));
                return;
            }
        }

        auto _eventInsideElement = trigger(_node, _engine, _canvas);

        if(_eventInsideElement && !_event.handled) {

            if(_eventDispatcher.dispatchEvent<MouseButtonReleasedEvent>() && !onInnerClickingReleased.isEmpty()) {
                _event.handled =_node->hasComponent<CanvasEventStopper>();
                auto* _mre = (MouseButtonReleasedEvent*)&_event;
                onInnerClickingReleased(_mre->getMouseButton());
            }

            if(_eventDispatcher.dispatchEvent<MouseButtonPressedEvent>() && !onInnerClicking.isEmpty()) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                auto* _mre = (MouseButtonPressedEvent*)&_event;
                onInnerClicking(_mre->getMouseButton());
            }

            if(_eventDispatcher.dispatchEvent<MouseMovedEvent>() && !onInnerMouseEntered.isEmpty()) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                if(mouseInnerStatus == MouseStatus::MouseExited) {
                    mouseInnerStatus = MouseStatus::MouseEntered;
                    onInnerMouseEntered();
                }
            }



            if(_eventDispatcher.dispatchEvent<MouseButtonReleasedEvent>() && !onClick.isEmpty()) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                auto* _mre = (MouseButtonReleasedEvent*)&_event;
                onClick(_mre->getMouseButton());
                return;
            }

            if(_eventDispatcher.dispatchEvent<MouseScrolledEvent>() && !onScroll.isEmpty()) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                auto* _mse = (MouseScrolledEvent*)&_event;
                onScroll({_mse->getScrollX(), _mse->getScrollY()});
                return;
            }




            if(_eventDispatcher.dispatchEvent<ControllerButtonUpEvent>() && !onGamepadButtonPressed.isEmpty()) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                auto* _jbue = (ControllerButtonUpEvent*)&_event;
                onGamepadButtonPressed(_jbue->getButton());
                return;
            }

            if(_eventDispatcher.dispatchEvent<MobileTouchUpEvent>() && !onMobileClick.isEmpty()) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
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
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                if(!onInnerMouseExited.isEmpty()) onInnerMouseExited();
                mouseInnerStatus = MouseStatus::MouseExited;
            }
        }

        if(_eventDispatcher.dispatchEvent<MouseMovedEvent>() && !_event.handled) {
            if(mouseStatus == MouseStatus::MouseEntered) {
                _event.handled = _node->hasComponent<CanvasEventStopper>();
                if(!onMouseExited.isEmpty()) onMouseExited();
                mouseStatus = MouseStatus::MouseExited;
            }
        }
    }
}