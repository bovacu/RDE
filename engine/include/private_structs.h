#ifndef RDE_PRIVATE_STRUCTS_H
#define RDE_PRIVATE_STRUCTS_H

#include "structs.h"

#define RDE_MAX_NUMBER_OF_WINDOWS 10

struct rde_window {
	SDL_Window* sdl_window = nullptr;
	SDL_GLContext sdl_gl_context;

	rde_display_info display_info;

	rde_window_callbacks window_callbacks;
	rde_display_callbacks display_callbacks;
};

struct rde_engine {
	float delta_time = 0.0f;
	float fixed_delta_time = 0.016f;

	float fixed_time_step_accumulator = 0.0f;
	RDE_PLATFORM_TYPE_ platform_type = RDE_PLATFORM_TYPE_UNSUPPORTED;

	bool running = true;

	rde_window* engine_windows[RDE_MAX_NUMBER_OF_WINDOWS];
};

struct rde_scene {
	
};

#endif