
void rde_events_window_create_events();
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_resize, window_callbacks, on_window_resize, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_focused_by_mouse, window_callbacks, on_window_focused_by_mouse, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_unfocused_by_mouse, window_callbacks, on_window_unfocused_by_mouse, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_focused_by_keyboard, window_callbacks, on_window_focused_by_keyboard, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_unfocused_by_keyboard, window_callbacks, on_window_unfocused_by_keyboard, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_moved, window_callbacks, on_window_moved, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_minimized, window_callbacks, on_window_minimized, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_maximized, window_callbacks, on_window_maximized, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_closed, window_callbacks, on_window_closed, { 
	rde_window_destroy_window(_window); 
	for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
		if(_window->sdl_window != nullptr) {
			return;
		}
	}

	ENGINE.running = false;
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_display_changed, window_callbacks, on_window_moved, {})

void rde_events_display_create_events();
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_connected, display_callbacks, on_display_connected, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_disconnected, display_callbacks, on_display_disconnected, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_changed_orientation, display_callbacks, on_display_changed_orientation, {})

void rde_events_window_create_events() {
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_RESIZED - WIN_EVENT_INIT] 				= &window_resize;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED - WIN_EVENT_INIT] 		= &window_focused_by_mouse;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED - WIN_EVENT_INIT] 		= &window_unfocused_by_mouse;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_FOCUSED - WIN_EVENT_INIT] 		= &window_focused_by_keyboard;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_UNFOCUSED - WIN_EVENT_INIT]	= &window_unfocused_by_keyboard;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MOVED - WIN_EVENT_INIT] 				= &window_moved;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MINIMIZED - WIN_EVENT_INIT] 			= &window_minimized;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MAXIMIZED - WIN_EVENT_INIT] 			= &window_maximized;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_CLOSED - WIN_EVENT_INIT] 				= &window_closed;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED - WIN_EVENT_INIT] 		= &window_display_changed;
}

void rde_events_display_create_events() {
	ENGINE.display_events[RDE_EVENT_TYPE_DISPLAY_CONNECTED - DISPLAY_EVENT_INIT] 			= &display_connected;
	ENGINE.display_events[RDE_EVENT_TYPE_DISPLAY_DISCONNECTED - DISPLAY_EVENT_INIT] 		= &display_disconnected;
	ENGINE.display_events[RDE_EVENT_TYPE_DISPLAY_CHANGED_ORIENTATION - DISPLAY_EVENT_INIT] = &display_changed_orientation;
}

void rde_sdl_to_rde_helper_transform_window_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	_rde_event->time_stamp = _sdl_event->window.timestamp;
	_rde_event->window_id = _sdl_event->window.windowID;

	switch (_sdl_event->window.event) {
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
	}
}

void rde_sdl_to_rde_helper_transform_display_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	_rde_event->time_stamp = _sdl_event->display.timestamp;

	switch (_sdl_event->window.event) {
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
	}
}

rde_event rde_engine_sdl_event_to_rde_event(SDL_Event* _sdl_event) {

	rde_event _event;

	switch(_sdl_event->type) {
		case SDL_WINDOWEVENT:	rde_sdl_to_rde_helper_transform_window_event(_sdl_event, &_event); break;
		case SDL_DISPLAYEVENT:  rde_sdl_to_rde_helper_transform_display_event(_sdl_event, &_event); break;
	}

	return _event;
}



// ======================= API ===========================

void rde_events_window_consume_events(rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - WIN_EVENT_INIT;

	if(_event_index >= 0 && _event_index < WIN_EVENT_COUNT) {
		ENGINE.window_events[_event_index](_window, _event);
	} else {
		printf("Window Event: %i, not handled \n", _event->type);
	}
}


void rde_events_display_consume_events(rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - DISPLAY_EVENT_INIT;

	if(_event_index >= 0 && _event_index < DISPLAY_EVENT_COUNT) {
		ENGINE.display_events[_event_index](_window, _event);
	} else {
		printf("Display Event: %i, not handled \n", _event->type);
	}
}

#if IS_MOBILE()
int rde_events_mobile_consume_events(void* _user_data, SDL_Event* _event) {
	switch(_event->type) {
		case SDL_APP_TERMINATING: {
			// TODO: terminate app and clean everything
		} break;

		case SDL_APP_LOWMEMORY: {
			// TODO: not sure what to do in this case
		} break;

		case SDL_APP_WILLENTERBACKGROUND: {
			// TODO: stop everything, main loop, music, nothing can run or the app may crash unexpectedly
		} break;

		case SDL_APP_DIDENTERBACKGROUND: {
			// TODO: stop everything, main loop, music, nothing can run or the app may crash unexpectedly
		} break;

		case SDL_APP_WILLENTERFOREGROUND: {
			// TODO: restart everything stoped on SDL_APP_DIDENTERBACKGROUND and SDL_APP_WILLENTERBACKGROUND
		} break;

		case SDL_APP_DIDENTERFOREGROUND: {
			// TODO: restart everything stoped on SDL_APP_DIDENTERBACKGROUND and SDL_APP_WILLENTERBACKGROUND
		} break;

		case SDL_LOCALECHANGED: {
			// TODO: not sure what to do in this case
		} break;
	}

	// returning 0 means the event gets out of the queue, and 1 means it is pre-processed and re-added to the queue.
	return 1;
}
#endif