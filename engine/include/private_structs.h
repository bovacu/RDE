#ifndef RDE_PRIVATE_STRUCTS_H
#define RDE_PRIVATE_STRUCTS_H

/**
 * ==================================================
 * =				READ PLEASE						=
 * ==================================================
 * 
 * This header file should not be included in the projects developed by RDE,
 * there are some data inside the structs that should not be modify by hand by 
 * the end-user, because when being modified, some systems or other structs have to
 * know there's been a change. This header file is included on the source files that
 * make the correct changes and notify every element that needs to be notified.
 * 
 * In general, I do not recommend to add this file as an #include, because every
 * of information that those structs contains, can be accessed from methods given
 * by RDE's API.
 * 
 * That being said, if you know what you are doing, then feel free to add this
 * anywhere you want, modify whatever you want and handle yourself everything, 
 * but have in mind that unexpected behaviours may happen.
 */


#include "structs.h"
#include "SDL2/SDL_Video.h"

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