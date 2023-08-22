#ifndef RDE_EVENT_H
#define RDE_EVENT_H

#include "core.h"
#include "enums/generic_enums.h"
#include "enums/keyboard_enums.h"
#include "enums/mouse_enums.h"
#include "enums/controller_enums.h"

struct rde_event_window {
	rde_vec2I position { -1, -1 };
	rde_vec2I size { -1, -1 };

	int display_index = -1;

	bool minimized = false;
	bool maximized = false;
};

struct rde_event_display {
	int orientation = -1;
	int display_index = -1;
};

struct rde_event_key {
	RDE_KEYBOARD_KEY_ key = RDE_KEYBOARD_KEY_NONE;

	char typed_char = '\0';

	std::string typed_text = "";

	int amount_of_times_pressed = -1;

	uint32_t init_time_of_press = 0;
	uint32_t end_time_of_press = 0;
};

struct rde_event_mouse {
	rde_vec2I position { -1, -1 };
	rde_vec2F scrolled { -1.f, -1.f };

	RDE_MOUSE_BUTTON_ button = RDE_MOUSE_BUTTON_NONE;

	int amount_of_times_pressed = -1;

	uint32_t init_time_of_press = 0;
	uint32_t end_time_of_press = 0;
};

struct rde_event_controller {
	rde_vec2F left_joystick { -1.f, -1.f };
	rde_vec2F right_joystick { -1.f, -1.f };
	rde_vec2F back_buttons { -1.f, -1.f };

	RDE_CONTROLLER_BUTTON_ button = RDE_CONTROLLER_BUTTON_NONE;

	uint32_t init_time_of_press = 0;
	uint32_t end_time_of_press = 0;
};

struct rde_event_mobile {
	rde_vec2I init_touch_position { -1, -1 };
	rde_vec2I end_touch_position { -1, -1 };

	float pressure = -1.f;

	int finger_id = -1;

	uint32_t init_time_of_touch = 0;
	uint32_t end_time_of_touch = 0;
};

struct rde_event_data {
	rde_event_window window_event_data;
	rde_event_key key_event_data;
	rde_event_mouse mouse_event_data;
	rde_event_controller controller_event_data;
	rde_event_mobile mobile_event_data;
	rde_event_display display_event_data;
};

struct rde_event {
	
	RDE_EVENT_TYPE_ type;
	RDE_EVENT_CATEGORY_ categories;

	bool handled;
	
	std::string name;

	rde_event_data data;

};

#endif