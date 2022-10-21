//
// Created by borja on 9/5/22.
//

#include "core/graph/components/ui/UI.h"
#include "core/systems/eventSystem/MouseEvent.h"
#include "core/systems/eventSystem/KeyEvent.h"
#include "core/systems/eventSystem/ControllerEvent.h"
#include "core/systems/eventSystem/MobileEvent.h"
#include "core/graph/components/Components.h"
#include "core/graph/components/NineSliceSprite.h"
#include "core/Engine.h"

namespace GDE {

    UIInteractable::UIInteractable(const NodeID& _nodeId) {  }

    static bool trigger(const NodeID& _nodeID, Engine* _engine, Canvas* _canvas) {
        auto* _ninePatch = _canvas->getGraph()->getComponent<UIInteractable>(_nodeID);
        auto* _transform = _canvas->getGraph()->getComponent<Transform>(_nodeID);
        auto _mousePos = _engine->manager.inputManager.getMousePosWorldPos();

        return _mousePos.isInside(_transform->getPosition(), Vec2F {(float)_ninePatch->sizeOfInteraction.x, (float)_ninePatch->sizeOfInteraction.y});
    }

    void UIInteractable::onEvent(const NodeID& _nodeID, Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event, Canvas* _canvas) {
        if (!_canvas->getGraph()->hasComponent<Active>(_nodeID) || !interactable) return;

        if(trigger(_nodeID, _engine, _canvas) && !_event.handled) {

            if(_eventDispatcher.dispatchEvent<MouseButtonReleasedEvent>() && !onInnerClickingReleased.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mre = (MouseButtonReleasedEvent*)&_event;
                onInnerClickingReleased(_mre->getMouseButton());
            }

            if(_eventDispatcher.dispatchEvent<MouseButtonPressedEvent>() && !onInnerClicking.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mre = (MouseButtonPressedEvent*)&_event;
                onInnerClicking(_mre->getMouseButton());
            }

            if(_eventDispatcher.dispatchEvent<MouseMovedEvent>()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                if(mouseInnerStatus == MouseStatus::MouseExited) {
                    mouseInnerStatus = MouseStatus::MouseEntered;
                    if(onInnerMouseEntered.isEmpty()) return;
                    onInnerMouseEntered();
                }
            }




            if(_eventDispatcher.dispatchEvent<MouseButtonReleasedEvent>() && !onClick.isEmpty()) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                auto* _mre = (MouseButtonReleasedEvent*)&_event;
                onClick(_mre->getMouseButton());
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



        if(_eventDispatcher.dispatchEvent<MouseMovedEvent>() || _event.handled) {
            if(mouseInnerStatus == MouseStatus::MouseEntered) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                mouseInnerStatus = MouseStatus::MouseExited;
                if(onInnerMouseExited.isEmpty()) return;
                onInnerMouseExited();
            }
        }

        if(_eventDispatcher.dispatchEvent<MouseMovedEvent>() || _event.handled) {
            if(mouseStatus == MouseStatus::MouseEntered) {
                _event.handled = _canvas->getGraph()->hasComponent<CanvasEventStopper>(_nodeID);
                mouseStatus = MouseStatus::MouseExited;
                if(onMouseExited.isEmpty()) return;
                onMouseExited();
            }
        }
    }

}