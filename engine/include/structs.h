#ifndef RDE_STRUCTS_H
#define RDE_STRUCTS_H

#include "core.h"
#include "enums/generic_enums.h"
#include "SDL2/SDL.h"

struct SDL_Window;

struct rde_window_callback {
	bool block_engine_default_implementation = false;
	void (*_callback)(rde_event*) = nullptr;
};

struct rde_window_callbacks {
	rde_window_callback on_window_resize = {0};
	rde_window_callback on_window_focused_by_mouse = {0};
	rde_window_callback on_window_unfocused_by_mouse = {0};
	rde_window_callback on_window_focused_by_keyboard = {0};
	rde_window_callback on_window_unfocused_by_keyboard = {0};
	rde_window_callback on_window_moved = {0};
	rde_window_callback on_window_minimized = {0};
	rde_window_callback on_window_maximized = {0};
	rde_window_callback on_window_closed = {0};
	rde_window_callback on_window_display_changed = {0};
};

struct rde_display_callbacks {
	rde_window_callback on_display_connected = {0};
	rde_window_callback on_display_disconnected = {0};
	rde_window_callback on_display_changed_orientation = {0};
};

struct rde_window {
	SDL_Window* sdl_window;
	SDL_GLContext sdl_gl_context;

	int display_of_window;

	rde_window_callbacks window_callbacks;
	rde_display_callbacks display_callbacks;
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