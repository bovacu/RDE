#pragma once

#ifndef RDE_MOUSE_KEYS_H
#define RDE_MOUSE_KEYS_H

#include <ostream>
#include "SDL2/SDL_mouse.h"

namespace RDE {

	/**
	 * @brief Codes for the buttons of te mouse.
	*/
	enum RDE_MOUSE_BUTTON_ : uint16_t {
		// From SDL2
		RDE_MOUSE_BUTTON_0 = SDL_BUTTON_LEFT,
		RDE_MOUSE_BUTTON_1 = SDL_BUTTON_MIDDLE,
		RDE_MOUSE_BUTTON_2 = SDL_BUTTON_RIGHT,
		RDE_MOUSE_BUTTON_3 = 3,
		RDE_MOUSE_BUTTON_4 = 4,
		RDE_MOUSE_BUTTON_5 = 5,
		RDE_MOUSE_BUTTON_6 = 6,
		RDE_MOUSE_BUTTON_7 = 7,

		RDE_MOUSE_BUTTON_LAST = RDE_MOUSE_BUTTON_7,
		RDE_MOUSE_BUTTON_LEFT = RDE_MOUSE_BUTTON_0,
		RDE_MOUSE_BUTTON_RIGHT = RDE_MOUSE_BUTTON_2,
		RDE_MOUSE_BUTTON_MIDDLE = RDE_MOUSE_BUTTON_1
	};

	inline std::ostream &operator<<(std::ostream &os, RDE_MOUSE_BUTTON_ mouseCode) {
        os << static_cast<int32_t>(mouseCode);
        return os;
    }
}

#endif // RDE_MOUSE_KEYS_H
