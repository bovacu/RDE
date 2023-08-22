#include "engine/sdl_to_engine_helper.h"

void rde_sdl_to_rde_helper_transform_window_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	switch (_sdl_event->window.event) {

		// ======= WINDOW =========

		case SDL_WINDOWEVENT_RESIZED: {
			_rde_event->type = RDE_EVENT_TYPE_WINDOW_RESIZED;
			_rde_event->data.window_event_data.size = { _sdl_event->window.data1, _sdl_event->window.data2 };
		} break;

		case SDL_WINDOWEVENT_MOVED:	{
			_rde_event->type = RDE_EVENT_TYPE_WINDOW_RESIZED;
			_rde_event->data.window_event_data.position = { _sdl_event->window.data1, _sdl_event->window.data2 };
		} break;

		case SDL_WINDOWEVENT_MAXIMIZED: {
			_rde_event->type = RDE_EVENT_TYPE_WINDOW_MAXIMIZED; 			
			_rde_event->data.window_event_data.maximized = true;
			_rde_event->data.window_event_data.minimized = false;
		} break;

		case SDL_WINDOWEVENT_MINIMIZED:	{
			_rde_event->type = RDE_EVENT_TYPE_WINDOW_MAXIMIZED; 			
			_rde_event->data.window_event_data.maximized = false;
			_rde_event->data.window_event_data.minimized = true;
		} break;

		case SDL_WINDOWEVENT_DISPLAY_CHANGED: {
			_rde_event->type = RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED;
			_rde_event->data.window_event_data.display_index = _sdl_event->window.data1;
		} break;

		case SDL_WINDOWEVENT_ENTER:	 			_rde_event->type = RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED; 		break;
		case SDL_WINDOWEVENT_LEAVE:	 			_rde_event->type = RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED; 		break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:		_rde_event->type = RDE_EVENT_TYPE_WINDOW_KEYBOARD_FOCUSED; 		break;
		case SDL_WINDOWEVENT_FOCUS_LOST:		_rde_event->type = RDE_EVENT_TYPE_WINDOW_KEYBOARD_UNFOCUSED; 	break;
		case SDL_WINDOWEVENT_CLOSE:				_rde_event->type = RDE_EVENT_TYPE_WINDOW_CLOSED;				break;

			//  =======================

			// ======= DISPLAY ========

		case SDL_DISPLAYEVENT_CONNECTED: {
			_rde_event->type = RDE_EVENT_TYPE_DISPLAY_CONNECTED;
			_rde_event->data.display_event_data.display_index = _sdl_event->display.display;
		} break;

		case SDL_DISPLAYEVENT_DISCONNECTED: {
			_rde_event->type = RDE_EVENT_TYPE_DISPLAY_DISCONNECTED;
			_rde_event->data.display_event_data.display_index = _sdl_event->display.display;
		} break;

		case SDL_DISPLAYEVENT_ORIENTATION: {
			_rde_event->type = RDE_EVENT_TYPE_DISPLAY_CHANGED_ORIENTATION;
			_rde_event->data.display_event_data.display_index = _sdl_event->display.display;
			_rde_event->data.display_event_data.orientation = _sdl_event->display.data1;
		} break;

		//  =======================
	}
}