#ifndef RDE_STRUCTS_H
#define RDE_STRUCTS_H

#include "core.h"
#include "enums/generic_enums.h"
#include "SDL2/SDL_Video.h"

/**
 * Structs defined as 'typedef struct X X' do have fields data inside, but it is data
 * that should not be modified by hand, but using the functions that the library offers.
 * 
 * An incorrect manual modification of some of those values can create unexpected behaviour.
 *
 * But in case you now what you are doing and want access to everything just use:
 * #include "private_structs.h"
*/
typedef struct rde_engine rde_engine;
typedef struct rde_window rde_window;
typedef struct rde_scene rde_scene;

struct rde_event;

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

struct rde_display_info {
	/**
	* This value is read-only, if modified by hand not only won't take effect
	* but also may produced unexpected behaviour (nothing good man).
	*/
	int index = -1;
	
	// TODO: bounds
};

#endif