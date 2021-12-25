#pragma once

#ifndef GAMEPAD_KEYS_H
#define GAMEPAD_KEYS_H

#include <cstdint>

namespace engine {

    /// -----------------------------------------------------------------
    /// |			CODES FOR GAMEPAD BUTTONS, THEY ARE 				|
    /// |			THE SAME AS GLFW'S ONES, BUT NOW ARE				|
    /// |			PROPERTY OF THE ENGINE AND DON'T RELAY				|
    /// |			ON GLFW												|
    /// -----------------------------------------------------------------

    typedef enum class GamePadKeys : uint16_t {
        Button0         = 0,
        Button1         = 1,
        Button2         = 2,
        Button3         = 3,
        Button4         = 4,
        Button5         = 5,
        Button6         = 6,
        Button7         = 7,
        Button8         = 8,
        Button9         = 9,
        Start           = 10,
        Select          = 11,
        RightJoystick   = 12,
        LeftJoystick    = 13,
        DPadUp          = 14,
        DPadRight       = 15,
        DPadDown        = 16,
        DPadLeft        = 17
    } GamePad;
}

    #define GAMEPAD_BUTTON_0                ::engine::GamePad::Button0          // square
    #define GAMEPAD_BUTTON_1                ::engine::GamePad::Button1          // X
    #define GAMEPAD_BUTTON_2                ::engine::GamePad::Button2          // circle
    #define GAMEPAD_BUTTON_3                ::engine::GamePad::Button3          // triangle
    #define GAMEPAD_BUTTON_4                ::engine::GamePad::Button4          // R1
    #define GAMEPAD_BUTTON_5                ::engine::GamePad::Button5          // L1
    #define GAMEPAD_BUTTON_6                ::engine::GamePad::Button6          // R2
    #define GAMEPAD_BUTTON_7                ::engine::GamePad::Button7          // L2
    #define GAMEPAD_BUTTON_8                ::engine::GamePad::Button8          // R3
    #define GAMEPAD_BUTTON_9                ::engine::GamePad::Button9          // L3
    #define GAMEPAD_BUTTON_START            ::engine::GamePad::Start            // Start
    #define GAMEPAD_BUTTON_SELECT           ::engine::GamePad::Select           // Select
    #define GAMEPAD_BUTTON_RIGHT_JOYSTICK   ::engine::GamePad::RightJoystick    // Right joystick
    #define GAMEPAD_BUTTON_LEFT_JOYSTICK    ::engine::GamePad::LeftJoystick     // Left joystick
    #define GAMEPAD_BUTTON_DPAD_UP          ::engine::GamePad::DPadUp
    #define GAMEPAD_BUTTON_DPAD_RIGHT       ::engine::GamePad::DPadRight
    #define GAMEPAD_BUTTON_DPAD_DOWN        ::engine::GamePad::DPadDown
    #define GAMEPAD_BUTTON_DPAD_LEFT        ::engine::GamePad::DPadLeft

#endif // GAMEPAD_KEYS_H
