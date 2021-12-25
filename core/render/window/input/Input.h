#pragma once

#ifndef ENGINE2_0_INPUT_H
#define ENGINE2_0_INPUT_H

#include "KeyboardKeys.h"
#include "MouseKeys.h"
#include "GamePadKeys.h"

#include "core/util/Util.h"

namespace engine {

    class Input {

        public:
            static std::unordered_map<KeyCode,   bool>  pressedKeyboardKeys;
            static std::unordered_map<MouseCode, bool>  pressedMouseButtons;
            static std::unordered_map<KeyCode,   bool>  pressedGamepadKeys;

        public:
            /// Checks if a key has just been pressed, only the first pressed, if continuous pressing it doesn't detect.
            /// @param _keyCode The code of the key.
            /// @return true if it is just pressed.
            static bool isKeyJustPressed(KeyCode _keyCode);

            /// Checks if a key is being pressed.
            /// @param _keyCode The code of the key.
            /// @return true if it is being pressed.
            static bool isKeyPressed(KeyCode _keyCode);

            /// Checks if a key has been released.
            /// @param _keyCode The code of the key.
            /// @return true if it is been released.
            static bool isKeyReleased(KeyCode _keyCode);

            /// Checks if a mouse button has just been pressed, only the first pressed, if continuous pressing it doesn't detect.
            /// @param _mouseButton The code of the button.
            /// @return true if it is just pressed.
            static bool isMouseJustPressed(MouseCode _mouseButton);

            /// Checks if a mouse button is being pressed.
            /// @param _mouseButton The code of the button.
            /// @return true if it is being pressed.
            static bool isMousePressed(MouseCode _mouseButton);

            /// Checks if a mouse button has been released.
            /// @param _mouseButton The code of the button.
            /// @return true if it is been released.
            static bool isMouseReleased(MouseCode _mouseButton);

            /// Returns the position of the mouse on the screen.
            /// @return Vec2i with [x,y] of the mouse.
            static Vec2I getMousePosition();

            /// Returns getMousePosition().x
            /// @return getMousePosition().x
            static int getMouseX();

            /// Returns getMousePosition().y
            /// @return getMousePosition().y
            static int getMouseY();
    };
}

#endif //ENGINE2_0_INPUT_H
