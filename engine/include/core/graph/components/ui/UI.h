//
// Created by borja on 9/5/22.
//

#ifndef GDE_UI_H
#define GDE_UI_H

#include "entt/entity/entity.hpp"
#include "core/render/elements/IRenderizable.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/keysAndButtons/ControllerButtons.h"

typedef entt::entity NodeID;

namespace GDE {




    /**
     * @brief Component that every UI element that wants to interact with events on the screen must include.
     *
     */
    class UIInteractable {
        FRIEND_CLASS(Canvas, UIButton, UICheckbox, NineSliceSprite)

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

            MouseStatus mouseInnerStatus = MouseStatus::MouseExited;

            bool interactable = true;

        private:
            /**
             * @brief Callback triggered when the mouse just got pressed.
             *
             */
            MDelegate<void(MouseCode)> onInnerClicking;

            /**
             * @brief Callback triggered when the mouse just got released.
             *
             */
            MDelegate<void(MouseCode)> onInnerClickingReleased;

            /**
             * @brief Callback triggered when the mouse has just entered.
             *
             */
            MDelegate<void()> onInnerMouseEntered;

            /**
             * @brief Callback triggered when the mouse has just exited.
             *
             */
            MDelegate<void()> onInnerMouseExited;

        public:

            /**
             * @brief Callback triggered when the mouse is clicked.
             *
             */
            MDelegate<void(MouseCode)> onClick;

            /**
             * @brief Callback triggered when the mouse has just entered.
             *
             */
            MDelegate<void()> onMouseEntered;

            /**
             * @brief Callback triggered when the mouse has just exited.
             *
             */
            MDelegate<void()> onMouseExited;

            /**
             * @brief Callback triggered when the mouse is scrolled.
             *
             */
            MDelegate<void(Vec2F)> onScroll;

            /**
             * @brief Callback triggered when a key is pressed.
             *
             */
            MDelegate<void(KeyCode)> onKeyPressed;

            /**
             * @brief Callback triggered when a gamepad button is pressed.
             *
             */
            MDelegate<void(ControllerButtons)> onGamepadButtonPressed;

            /**
             * @brief Callback triggered when the mobile screen is touched.
             *
             */
            MDelegate<void(int)> onMobileClick;

            /**
             * @brief Size of the rectangle of interaction. Inside this area, there will be interaction, outside it will be ignored.
             */
            Vec2F sizeOfInteraction;

            explicit UIInteractable(const NodeID& _nodeId);

        private:
            /**
             * @brief Function called inside the Graph system to take the events and handle them inside the UI elements.
             * @param _nodeID Entity that has the UI element
             * @param _eventDispatcher Event dispatcher
             * @param _event Event being handled
             * @param _canvas Canvas that contains the Entity
             */
            void onEvent(const NodeID& _nodeID, Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event, Canvas* _canvas);
    };

    struct CommonUIConfig {
        bool stopFurtherClicks = true;
    };

    /**
     * @brief Component that every UI element (Buttons, UITexts...) should follow to be rendered correctly by the engine.
     */
    class UI : public IRenderizable {
        protected:
            /**
             * @brief Texture that contains the sprite of the rendered UI.
             */
            Texture* texture = nullptr;
            Canvas* canvas = nullptr;
            NodeID ID;

        public:
            FRIEND_CLASS(SpriteBatch)

            /**
            * @see UIInteractable
            */
            UIInteractable* interaction = nullptr;

            UI(const NodeID& _nodeID, Canvas* _canvas) : IRenderizable(_canvas->getGraph()->getComponent<Transform>(_nodeID)) {
                if(!_canvas->getGraph()->hasComponent<UIInteractable>(_nodeID)) {
                    interaction = _canvas->getGraph()->addComponent<UIInteractable>(_nodeID);
                }
                canvas = _canvas;
                ID = _nodeID;
            }

            virtual bool isInteractable() = 0;

            virtual void setInteractable(bool _interactable) = 0;

            /**
             * @see IRenderizable
             */
            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] Vec2F getSize() const override { return texture->getSize(); }

            /**
             * @see IRenderizable
             */
            [[nodiscard]] FloatRect getRegion() const override { return texture->getRegion(); }

            virtual ~UI() override {  };
    };

}

#endif //GDE_UI_H