#ifndef RDE_STRUCTS_H
#define RDE_STRUCTS_H

#include "core.h"
#include "enums/generic_enums.h"
#include "SDL2/SDL.h"

struct SDL_Window;

struct rde_window {
	SDL_Window* sdl_window;
	SDL_GLContext sdl_gl_context;

	int display_of_window;
};

struct rde_engine {
	float delta_time;
	float fixed_delta_time;

	float fixed_time_step_accumulator;
	RDE_PLATFORM_TYPE_ platform_type;

	bool running;

	rde_window* engine_windows[10];
};

struct rde_scene {
	
};

#endif