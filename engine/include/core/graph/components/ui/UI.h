//
// Created by borja on 9/5/22.
//

#ifndef RDE_UI_H
#define RDE_UI_H

#include "entt/entity/entity.hpp"
#include "core/render/elements/IRenderizable.h"
#include "core/systems/uiSystem/Canvas.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/keysAndButtons/ControllerButtons.h"

typedef entt::entity NodeID;

namespace RDE {


    FORWARD_DECLARE_CLASS(KeyEvent, Manager)

    /**
     * @brief Component that every UI element that wants to interact with events on the screen must include.
     *
     */
    class UIInteractable {
        FRIEND_CLASS(Graph, Canvas, UIImage, UIButton, UICheckbox, UIPanel, UIInput, UISlider)

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

            /**
             * @brief Callback triggered when a key has just been pressed.
             *
             */
            MDelegate<void(KeyCode, char)> onInnerKeyPressed;

            /**
             * @brief Callback triggered when a key has just been released.
             *
             */
            MDelegate<void(KeyCode, char)> onInnerKeyReleased;

            /**
             * @brief Callback triggered when an element of the UI has been unfocused.
             *
             */
            MDelegate<void()> onInnerUnfocused;

        public:
            UIInteractable(Node* _node, Manager* _manager, Graph* _graph);

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
            MDelegate<void(KeyCode, char)> onKeyPressed;

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

        private:
            /**
             * @brief Function called inside the Graph system to take the events and handle them inside the UI elements.
             * @param _nodeID Entity that has the UI element
             * @param _eventDispatcher Event dispatcher
             * @param _event Event being handled
             */
            void onEvent(Node* _node, Engine* _engine, EventDispatcher& _eventDispatcher, Event& _event);
    };

    struct CommonUIConfig {
        bool stopFurtherClicks = true;
    };

    /**
     * @brief Component that every UI element (Buttons, UITexts...) should follow to be rendered correctly by the engine.
     */
    class UI : public IRenderizable {
        FRIEND_CLASS(SpriteBatch)

        protected:
            Texture* texture = nullptr;

            /**
             * @brief Amount of pixels from the origin to render the IRenderizable
             */
            Vec2F originOffset = { 0.f, 0.f };

        public:
            UIInteractable* interaction = nullptr;

        public:
            explicit UI(Node* _node, const CommonUIConfig* _config);

            /**
             * @brief Returns if the the Component is interactable, usually the return is just UI::interaction->interactable.
             * @return bool
             */
            virtual bool isInteractable() = 0;

            /**
             * @brief Makes the Component interactable or not. In the inside of each Component it may compute other elements.
             * @param _interactable True/False
             */
            virtual void setInteractable(bool _interactable) = 0;

            /**
             * @brief Returns how far the rendering will be performed from the origin.
             * @return Vec2F
             */
            [[nodiscard]] Vec2F getOriginOffset() const { return originOffset; }

            /**
             * @brief Sets how far the rendering will be performed from the origin. This function will shoot an inner hierarchy update.
             * @param _originOffset Vec2F
             */
            void setOriginOffset(const Vec2F& _originOffset);

            [[nodiscard]] GLuint getTexture() const override { return texture->getGLTexture(); }
            [[nodiscard]] Vec2F getSize() const override;
            [[nodiscard]] FloatRect getRegion() const override { return texture->getRegion(); }

            virtual ~UI() override {  };
    };

}

#endif //RDE_UI_H
