//
// Created by borja on 9/5/22.
//

#include "core/Core.h"
#include "core/graph/components/ui/UI.h"
#include "core/graph/components/ui/UIAnchoring.h"
#include "core/systems/eventSystem/MouseEvent.h"
#include "core/systems/eventSystem/KeyEvent.h"
#include "core/systems/eventSystem/ControllerEvent.h"
#include "core/systems/eventSystem/MobileEvent.h"
#include "core/Engine.h"
#include "core/graph/components/Node.h"
#include "core/render/elements/Texture.h"

namespace RDE {

    UIInteractable::UIInteractable(Node* _node, Manager* _manager, Graph* _graph) {}

    static bool trigger(Node* _node, Engine* _engine) {
        auto* _transform = _node->getTransform();
        auto _mousePos = _engine->manager.inputManager.getMousePosWorldPos();
		auto* _anchoring = _node->getComponent<UIAnchoring>();

		return _mousePos.isInside(_transform->getPosition(), Vec2F {(float)_anchoring->getSize().x, (float)_anchoring->getSize().y});
    }

    static void shouldStopEventPropagation(Event& _event, Node* _node, bool _forceStop = false) {
        _event.handled = _node->hasComponent<CanvasEventStopper>() && _node->getComponent<CanvasEventStopper>()->isEnabled() || !_forceStop;
    }

    void UIInteractable::onEvent(Node* _node, Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event) {
        if (!_node->isActive() || !interactable) {
            return;
        }

        if(focused) {
            if(_eventDispatcher.dispatchEvent<TextTypedEvent>() && !onInnerTextTyped.isEmpty()) {
                shouldStopEventPropagation(_event, _node, true);
                auto* _ttEvent = ((TextTypedEvent*)&_event);
                onInnerTextTyped(_ttEvent->text);
            }

            if(_eventDispatcher.dispatchEvent<KeyPressedEvent>() && !onInnerKeyPressed.isEmpty() && !_event.handled) {
                shouldStopEventPropagation(_event, _node);
                auto* _kpe = (KeyPressedEvent*)&_event;
                onInnerKeyPressed(_kpe->getKeyCode());
            }

            if(_eventDispatcher.dispatchEvent<KeyReleasedEvent>() && !onInnerKeyReleased.isEmpty() && !_event.handled) {
                shouldStopEventPropagation(_event, _node);
                auto* _kre = (KeyReleasedEvent*)&_event;
                onInnerKeyReleased(_kre->getKeyCode());
            }

            if(_eventDispatcher.dispatchEvent<KeyPressedEvent>() && !onKeyPressed.isEmpty() && !_event.handled) {
                shouldStopEventPropagation(_event, _node);
                auto* _kpe = (KeyPressedEvent*)&_event;
                onKeyPressed(_kpe->getKeyCode());
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
				if(mouseInnerStatus == RDE_MOUSE_STATUS_EXITED) {
					mouseInnerStatus = RDE_MOUSE_STATUS_ENTERED;
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
			if(mouseInnerStatus == RDE_MOUSE_STATUS_ENTERED) {
                shouldStopEventPropagation(_event, _node);
                if(!onInnerMouseExited.isEmpty()) onInnerMouseExited();
				mouseInnerStatus = RDE_MOUSE_STATUS_EXITED;
            }
        }

        if(_eventDispatcher.dispatchEvent<MouseMovedEvent>() && !_event.handled) {
			if(mouseStatus == RDE_MOUSE_STATUS_ENTERED) {
                shouldStopEventPropagation(_event, _node);
                if(!onMouseExited.isEmpty()) onMouseExited();
				mouseStatus = RDE_MOUSE_STATUS_EXITED;
            }
        }
    }
}