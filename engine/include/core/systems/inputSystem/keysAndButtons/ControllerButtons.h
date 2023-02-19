#pragma once

#ifndef RDE_GAMEPAD_KEYS_H
#define RDE_GAMEPAD_KEYS_H

#include <cstdint>
#include "core/platform/PlatformHeaderSDL.h"

namespace RDE {

    /// -----------------------------------------------------------------
    /// |			CODES FOR GAMEPAD BUTTONS, THEY ARE 				|
    /// |			THE SAME AS GLFW'S ONES, BUT NOW ARE				|
    /// |			PROPERTY OF THE ENGINE AND DON'T RELAY				|
    /// |			ON GLFW												|
    /// -----------------------------------------------------------------

    typedef enum class ControllerButtons : uint16_t {
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

    typedef enum class ControllerAxis {
        Left,
        Right,
        LT,
        RT
    } GamePadA;

    static const char* ControllerKeysStr[] = {
            "A", "B", "X", "Y", "Select", "Guide", "Start", "L3", "R3", "LB", "RB",
            "DUp", "DDown", "DLeft", "DRight", "", "LB", "", "RB"
    };
}

#endif // RDE_GAMEPAD_KEYS_H
