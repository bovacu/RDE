#pragma once

#ifndef RDE_GAMEPAD_KEYS_H
#define RDE_GAMEPAD_KEYS_H

#include <cstdint>
#include "core/platform/PlatformHeaderSDL.h"

namespace RDE {

    /**
     * @brief Codes for the buttons of a controller.
    */
    enum RDE_CONTROLLER_BUTTON_ : uint16_t {
		RDE_CONTROLLER_BUTTON_A            = SDL_CONTROLLER_BUTTON_A,              // 0
		RDE_CONTROLLER_BUTTON_B            = SDL_CONTROLLER_BUTTON_B,              // 1
		RDE_CONTROLLER_BUTTON_X            = SDL_CONTROLLER_BUTTON_X,              // 2
		RDE_CONTROLLER_BUTTON_Y            = SDL_CONTROLLER_BUTTON_Y,              // 3
		RDE_CONTROLLER_BUTTON_LB           = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,   // 9
		RDE_CONTROLLER_BUTTON_RB           = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,  // 10
		RDE_CONTROLLER_BUTTON_GUIDE        = SDL_CONTROLLER_BUTTON_GUIDE,          // 5
		RDE_CONTROLLER_BUTTON_START        = SDL_CONTROLLER_BUTTON_START,          // 6
		RDE_CONTROLLER_BUTTON_SELECT       = SDL_CONTROLLER_BUTTON_BACK,           // 4
		RDE_CONTROLLER_BUTTON_R_JOY   	   = SDL_CONTROLLER_BUTTON_RIGHTSTICK,     // 8
		RDE_CONTROLLER_BUTTON_L_JOY    	   = SDL_CONTROLLER_BUTTON_LEFTSTICK,      // 7
		RDE_CONTROLLER_BUTTON_DPAD_UP      = SDL_CONTROLLER_BUTTON_DPAD_UP,        // 11
		RDE_CONTROLLER_BUTTON_DPAD_RIGHT   = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,     // 14
		RDE_CONTROLLER_BUTTON_DPAD_DOWN    = SDL_CONTROLLER_BUTTON_DPAD_DOWN,      // 12
		RDE_CONTROLLER_BUTTON_DPAD_LEFT    = SDL_CONTROLLER_BUTTON_DPAD_LEFT       // 13
    };

    /**
     * @brief Codes for the Axis/Special buttons of a controller.
    */
    enum RDE_CONTROLLER_AXIS_ {
		RDE_CONTROLLER_AXIS_LEFT,
		RDE_CONTROLLER_AXIS_RIGHT,
		RDE_CONTROLLER_AXIS_LT,
		RDE_CONTROLLER_AXIS_RT
    };

    /**
     * @brief Names of the buttons of a controller.
    */
    static const char* ControllerKeysStr[] = {
            "A", "B", "X", "Y", "Select", "Guide", "Start", "L3", "R3", "LB", "RB",
            "DUp", "DDown", "DLeft", "DRight", "", "LB", "", "RB"
    };
}

#endif // RDE_GAMEPAD_KEYS_H
