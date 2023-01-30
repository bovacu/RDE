//
// Created by borja on 9/5/22.
//

#ifndef RDE_UI_H
#define RDE_UI_H

#include "entt/entity/entity.hpp"
#include "core/render/elements/IRenderizable.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/keysAndButtons/ControllerButtons.h"

typedef entt::entity NodeID;

namespace RDE {


    class KeyEvent;
    class Manager;
    class Engine;
    class EventDispatcher;
    class Event;

    /**
     * @brief Component that every UI element that wants to interact with events on the screen must include.
     *
     */
    class UIInteractable {
        friend class Scene;
        friend class Graph;
        friend class Canvas;
        friend class UIImage;
        friend class UIButton;
        friend class UICheckbox;
        friend class UIPanel;
        friend class UIInput;
        friend class UISlider;
        friend class UIText;

        private:
            /**
             * @brief Status of the mouse entering and exiting.
             */
            enum MouseStatus {
                MouseEntered,
                MouseExited
            };

            /**
             * @brief Current mouse status.
             *
             */
            MouseStatus mouseStatus = MouseStatus::MouseExited;

            /**
             * @brief Current inner status of the mouse.
             */
            MouseStatus mouseInnerStatus = MouseStatus::MouseExited;

            /**
             * @brief Flag that keeps if the Component is or not interactable.
             */
            bool interactable = true;

            /**
             * @brief Flag that keeps if the Component is focused or not.
             */
            bool focused = false;

        private:
            /**
             * @brief Callback triggered when the mouse just got pressed.
             *
             */
            Delegate<void(MouseCode)> onInnerClicking;

            /**
             * @brief Callback triggered when the mouse just got released.
             *
             */
            Delegate<void(MouseCode)> onInnerClickingReleased;

            /**
             * @brief Callback triggered when the mouse has just entered.
             *
             */
            Delegate<void()> onInnerMouseEntered;

            /**
             * @brief Callback triggered when the mouse has just exited.
             *
             */
            Delegate<void()> onInnerMouseExited;

            /**
             * @brief Callback triggered when a key has just been pressed.
             *
             */
            Delegate<void(KeyCode)> onInnerKeyPressed;

            /**
             * @brief Callback triggered when text editing mode is on and a key is pressed.
             *
             */
            Delegate<void(const std::string&)> onInnerTextTyped;

            /**
             * @brief Callback triggered when a key has just been released.
             *
             */
            Delegate<void(KeyCode)> onInnerKeyReleased;

            /**
             * @brief Callback triggered when an element of the UI has been unfocused.
             *
             */
            Delegate<void()> onInnerUnfocused;

        public:
            UIInteractable(Node* _node, Manager* _manager, Graph* _graph);

            /**
             * @brief Callback triggered when the mouse is clicked.
             *
             */
            Delegate<void(MouseCode)> onClick;

            /**
             * @brief Callback triggered when the mouse has just entered.
             *
             */
            Delegate<void()> onMouseEntered;

            /**
             * @brief Callback triggered when the mouse has just exited.
             *
             */
            Delegate<void()> onMouseExited;

            /**
             * @brief Callback triggered when the mouse is scrolled.
             *
             */
            Delegate<void(Vec2F)> onScroll;

            /**
             * @brief Callback triggered when a key is pressed.
             *
             */
            Delegate<void(KeyCode)> onKeyPressed;

            /**
             * @brief Callback triggered when a gamepad button is pressed.
             *
             */
            Delegate<void(ControllerButtons)> onGamepadButtonPressed;

            /**
             * @brief Callback triggered when the mobile screen is touched.
             *
             */
            Delegate<void(int)> onMobileClick;

        private:
            /**
             * @brief Function called inside the Graph system to take the events and handle them inside the UI elements.
             * @param _nodeID Entity that has the UI element
             * @param _eventDispatcher Event dispatcher
             * @param _event Event being handled
             */
            void onEvent(Node* _node, Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event);
    };
}

#endif //RDE_UI_H
