#pragma once

#ifndef RDE_MOUSE_KEYS_H
#define RDE_MOUSE_KEYS_H

#include <ostream>
#include "core/platform/PlatformHeaderSDL.h"

namespace RDE {

    /// -----------------------------------------------------------------
    /// |			CODES FOR MOUSE BUTTONS, THEY ARE					|
    /// |			THE SAME AS GLFW'S ONES, BUT NOW ARE				|
    /// |			PROPERTY OF THE ENGINE AND DON'T RELAY				|
    /// |			ON GLFW												|
    /// -----------------------------------------------------------------

    typedef enum class MouseCode : uint16_t {
        // From SDL2
        Button0 = SDL_BUTTON_LEFT,
        Button1 = SDL_BUTTON_MIDDLE,
        Button2 = SDL_BUTTON_RIGHT,
        Button3 = 3,
        Button4 = 4,
        Button5 = 5,
        Button6 = 6,
        Button7 = 7,

        ButtonLast = Button7,
        ButtonLeft = Button0,
        ButtonRight = Button2,
        ButtonMiddle = Button1
    } Mouse;

    inline std::ostream &operator<<(std::ostream &os, MouseCode mouseCode) {
        os << static_cast<int32_t>(mouseCode);
        return os;
    }
}

#endif // RDE_MOUSE_KEYS_H
