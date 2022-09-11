//
// Created by borja on 9/5/22.
//

#include "core/graph/components/UI.h"
#include "core/systems/eventSystem/MouseEvent.h"
#include "core/systems/eventSystem/KeyEvent.h"
#include "core/systems/eventSystem/ControllerEvent.h"
#include "core/systems/eventSystem/MobileEvent.h"
#include "core/graph/components/Components.h"

namespace GDE {

    UIInteractable::UIInteractable(const NodeID& _nodeId) {  }

    void UIInteractable::onEvent(const NodeID& _nodeID, EventDispatcher& _eventDispatcher, Event& _event, Canvas* _canvas) {
        if (interactionTrigger == nullptr) return;

        if(interactionTrigger(_nodeID, _canvas)) {
            if(_eventDispatcher.dispatchEvent<MouseButtonReleasedEvent>() && !onClick.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mre = (MouseButtonReleasedEvent*)&_event;
                onClick(_mre->getMouseButton());
                return;
            }

            if(_eventDispatcher.dispatchEvent<MouseMovedEvent>()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                if(mouseStatus == MouseStatus::MouseExited) {
                    mouseStatus = MouseStatus::MouseEntered;
                    if(onMouseEntered.isEmpty()) return;
                    onMouseEntered();
                }
                return;
            }

            if(_eventDispatcher.dispatchEvent<MouseScrolledEvent>() && !onScroll.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mse = (MouseScrolledEvent*)&_event;
                onScroll({_mse->getScrollX(), _mse->getScrollY()});
                return;
            }

            if(_eventDispatcher.dispatchEvent<KeyReleasedEvent>() && !onKeyPressed.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _kre = (KeyReleasedEvent*)&_event;
                onKeyPressed(_kre->getKeyCode());
                return;
            }

            if(_eventDispatcher.dispatchEvent<ControllerButtonUpEvent>() && !onGamepadButtonPressed.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _jbue = (ControllerButtonUpEvent*)&_event;
                onGamepadButtonPressed(_jbue->getButton());
                return;
            }

            if(_eventDispatcher.dispatchEvent<MobileTouchUpEvent>() && !onMobileClick.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mtue = (MobileTouchUpEvent*)&_event;
                onMobileClick(_mtue->getFingerID());
                return;
            }

            return;
        }

        if(_eventDispatcher.dispatchEvent<MouseMovedEvent>()) {
            if(mouseStatus == MouseStatus::MouseEntered) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                mouseStatus = MouseStatus::MouseExited;
                if(onMouseExited.isEmpty()) return;
                onMouseExited();
            }
        }
    }

}