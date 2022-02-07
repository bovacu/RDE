#pragma once

#ifndef GAMEPAD_KEYS_H
#define GAMEPAD_KEYS_H

#include <cstdint>
#include <SDL_gamecontroller.h>

namespace engine {

    /// -----------------------------------------------------------------
    /// |			CODES FOR GAMEPAD BUTTONS, THEY ARE 				|
    /// |			THE SAME AS GLFW'S ONES, BUT NOW ARE				|
    /// |			PROPERTY OF THE ENGINE AND DON'T RELAY				|
    /// |			ON GLFW												|
    /// -----------------------------------------------------------------

    typedef enum class GamePadKeys : uint16_t {
        ButtonA         = SDL_CONTROLLER_BUTTON_A,              // 0
        ButtonB         = SDL_CONTROLLER_BUTTON_B,              // 1
        ButtonX         = SDL_CONTROLLER_BUTTON_X,              // 2
        ButtonY         = SDL_CONTROLLER_BUTTON_Y,              // 3
        LB              = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,   // 9
        RB              = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,  // 10
        Guide           = SDL_CONTROLLER_BUTTON_GUIDE,          // 5
        Start           = SDL_CONTROLLER_BUTTON_START,          // 6
        Select          = SDL_CONTROLLER_BUTTON_BACK,           // 4
        RightJoystick   = SDL_CONTROLLER_BUTTON_RIGHTSTICK,     // 8
        LeftJoystick    = SDL_CONTROLLER_BUTTON_LEFTSTICK,      // 7
        DPadUp          = SDL_CONTROLLER_BUTTON_DPAD_UP,        // 11
        DPadRight       = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,     // 14
        DPadDown        = SDL_CONTROLLER_BUTTON_DPAD_DOWN,      // 12
        DPadLeft        = SDL_CONTROLLER_BUTTON_DPAD_LEFT       // 13
    } GamePad;

    typedef enum class GamePadAxis {
        Left,
        Right,
        LT,
        RT
    } GamePadA;

    static const char* GamePadKeysStr[] = {
            "A", "B", "X", "Y", "Select", "Guide", "Start", "L3", "R3", "LB", "RB",
            "DUp", "DDown", "DLeft", "DRight", "", "LB", "", "RB"
    };
}

//    #define GAMEPAD_BUTTON_0                ::engine::GamePad::Button0          // square
//    #define GAMEPAD_BUTTON_1                ::engine::GamePad::Button1          // X
//    #define GAMEPAD_BUTTON_2                ::engine::GamePad::Button2          // circle
//    #define GAMEPAD_BUTTON_3                ::engine::GamePad::Button3          // triangle
//    #define GAMEPAD_BUTTON_4                ::engine::GamePad::Button4          // R1
//    #define GAMEPAD_BUTTON_5                ::engine::GamePad::Button5          // L1
//    #define GAMEPAD_BUTTON_6                ::engine::GamePad::Button6          // R2
//    #define GAMEPAD_BUTTON_7                ::engine::GamePad::Button7          // L2
//    #define GAMEPAD_BUTTON_8                ::engine::GamePad::Button8          // R3
//    #define GAMEPAD_BUTTON_9                ::engine::GamePad::Button9          // L3
//    #define GAMEPAD_BUTTON_START            ::engine::GamePad::Start            // Start
//    #define GAMEPAD_BUTTON_SELECT           ::engine::GamePad::Select           // Select
//    #define GAMEPAD_BUTTON_RIGHT_JOYSTICK   ::engine::GamePad::RightJoystick    // Right joystick
//    #define GAMEPAD_BUTTON_LEFT_JOYSTICK    ::engine::GamePad::LeftJoystick     // Left joystick
//    #define GAMEPAD_BUTTON_DPAD_UP          ::engine::GamePad::DPadUp
//    #define GAMEPAD_BUTTON_DPAD_RIGHT       ::engine::GamePad::DPadRight
//    #define GAMEPAD_BUTTON_DPAD_DOWN        ::engine::GamePad::DPadDown
//    #define GAMEPAD_BUTTON_DPAD_LEFT        ::engine::GamePad::DPadLeft

#endif // GAMEPAD_KEYS_H
