

void rde_events_window_create_events();
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_resize, window_callbacks, on_window_resize, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_focused_by_mouse, window_callbacks, on_window_focused_by_mouse, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_unfocused_by_mouse, window_callbacks, on_window_unfocused_by_mouse, {
    memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_focused_by_keyboard, window_callbacks, on_window_focused_by_keyboard, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_unfocused_by_keyboard, window_callbacks, on_window_unfocused_by_keyboard, {
	memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_moved, window_callbacks, on_window_moved, {
    memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_minimized, window_callbacks, on_window_minimized, {
    memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_maximized, window_callbacks, on_window_maximized, {
    memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_closed, window_callbacks, on_window_closed, { 
	rde_window_destroy_window(_window); 
	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_windows; _i++) {
		if(ENGINE.windows[_i].sdl_window != NULL) {
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

void rde_events_key_create_events();
void rde_events_on_key_pressed_event(rde_window* _window, rde_event* _event) {
	if(_window->key_states[_event->data.key_event_data.key] == RDE_INPUT_STATUS_UNINITIALIZED || _window->key_states[_event->data.key_event_data.key] == RDE_INPUT_STATUS_JUST_RELEASED) {
		_window->key_states[_event->data.key_event_data.key] = RDE_INPUT_STATUS_JUST_PRESSED;
	} else if(_window->key_states[_event->data.key_event_data.key] == RDE_INPUT_STATUS_JUST_PRESSED) {
		_window->key_states[_event->data.key_event_data.key] = RDE_INPUT_STATUS_KEEP_PRESSED;
	}
}

void rde_events_on_key_released_event(rde_window* _window, rde_event* _event) {
	_window->key_states[_event->data.key_event_data.key] = RDE_INPUT_STATUS_JUST_RELEASED;
}

void rde_events_mouse_button_create_events();
void rde_events_on_mouse_button_pressed_event(rde_window* _window, rde_event* _event) {
	if(_window->mouse_states[_event->data.mouse_event_data.button] == RDE_INPUT_STATUS_UNINITIALIZED || _window->mouse_states[_event->data.mouse_event_data.button] == RDE_INPUT_STATUS_JUST_RELEASED) {
		_window->mouse_states[_event->data.mouse_event_data.button] = RDE_INPUT_STATUS_JUST_PRESSED;
	} else if(_window->key_states[_event->data.mouse_event_data.button] == RDE_INPUT_STATUS_JUST_PRESSED) {
		_window->mouse_states[_event->data.mouse_event_data.button] = RDE_INPUT_STATUS_KEEP_PRESSED;
	}
}

void rde_events_on_mouse_button_released_event(rde_window* _window, rde_event* _event) {
	_window->mouse_states[_event->data.mouse_event_data.button] = RDE_INPUT_STATUS_JUST_RELEASED;
}

void rde_events_on_mouse_moved_event(rde_window* _window, rde_event* _event) {
	_window->mouse_position = _event->data.mouse_event_data.position;
}


void rde_events_window_create_events() {
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_RESIZED - RDE_WIN_EVENT_INIT] = &window_resize;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED - RDE_WIN_EVENT_INIT] = &window_focused_by_mouse;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED - RDE_WIN_EVENT_INIT] = &window_unfocused_by_mouse;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_FOCUSED - RDE_WIN_EVENT_INIT] = &window_focused_by_keyboard;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_UNFOCUSED - RDE_WIN_EVENT_INIT]	= &window_unfocused_by_keyboard;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MOVED - RDE_WIN_EVENT_INIT] = &window_moved;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MINIMIZED - RDE_WIN_EVENT_INIT] = &window_minimized;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MAXIMIZED - RDE_WIN_EVENT_INIT] = &window_maximized;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_CLOSED - RDE_WIN_EVENT_INIT] = &window_closed;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED - RDE_WIN_EVENT_INIT] = &window_display_changed;
}

void rde_events_display_create_events() {
	ENGINE.display_events[RDE_EVENT_TYPE_DISPLAY_CONNECTED - RDE_DISPLAY_EVENT_INIT] = &display_connected;
	ENGINE.display_events[RDE_EVENT_TYPE_DISPLAY_DISCONNECTED - RDE_DISPLAY_EVENT_INIT] = &display_disconnected;
	ENGINE.display_events[RDE_EVENT_TYPE_DISPLAY_CHANGED_ORIENTATION - RDE_DISPLAY_EVENT_INIT] = &display_changed_orientation;
}

void rde_events_key_create_events() {
	ENGINE.key_events[RDE_EVENT_TYPE_KEY_PRESSED - RDE_KEY_EVENT_INIT] = &rde_events_on_key_pressed_event;
	ENGINE.key_events[RDE_EVENT_TYPE_KEY_RELEASED - RDE_KEY_EVENT_INIT] = &rde_events_on_key_released_event;
	// TODO: implement the other ones:
	//		- RDE_EVENT_TYPE_KEY_TYPED
	//		- RDE_EVENT_TYPE_TEXT_TYPED
}

void rde_events_mouse_button_create_events() {
	ENGINE.mouse_events[RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED - RDE_MOUSE_EVENT_INIT] = &rde_events_on_mouse_button_pressed_event;
	ENGINE.mouse_events[RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED - RDE_MOUSE_EVENT_INIT] = &rde_events_on_mouse_button_released_event;
	ENGINE.mouse_events[RDE_EVENT_TYPE_MOUSE_MOVED - RDE_MOUSE_EVENT_INIT] = &rde_events_on_mouse_moved_event;
}

void rde_sdl_to_rde_helper_transform_window_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	_rde_event->time_stamp = _sdl_event->window.timestamp;
	_rde_event->window_id = _sdl_event->window.windowID;

	switch (_sdl_event->window.event) {
		case SDL_WINDOWEVENT_RESIZED: {
			_rde_event->type = RDE_EVENT_TYPE_WINDOW_RESIZED;
			_rde_event->data.window_event_data.size.x = _sdl_event->window.data1;
			_rde_event->data.window_event_data.size.y = _sdl_event->window.data2;
		} break;

		case SDL_WINDOWEVENT_MOVED:	{
			_rde_event->type = RDE_EVENT_TYPE_WINDOW_RESIZED;
			_rde_event->data.window_event_data.position.x = _sdl_event->window.data1;
			_rde_event->data.window_event_data.position.y = _sdl_event->window.data2;
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

void rde_sdl_to_rde_helper_transform_keyboard_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	_rde_event->time_stamp = _sdl_event->display.timestamp;
	_rde_event->window_id = _sdl_event->window.windowID;

	switch (_sdl_event->key.state) {
		case SDL_PRESSED : {
			_rde_event->type = RDE_EVENT_TYPE_KEY_PRESSED;
		} break;

		case SDL_RELEASED: {
			_rde_event->type = RDE_EVENT_TYPE_KEY_RELEASED;
		} break;
	}

	_rde_event->data.key_event_data.key = (RDE_KEYBOARD_KEY_)_sdl_event->key.keysym.scancode;
}

void rde_sdl_to_rde_helper_transform_mouse_button_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	_rde_event->time_stamp = _sdl_event->display.timestamp;
	_rde_event->window_id = _sdl_event->window.windowID;

	switch(_sdl_event->type) {
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP: {
			switch (_sdl_event->button.state) {
				case SDL_PRESSED : {
					_rde_event->type = RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED;
				} break;

				case SDL_RELEASED: {
					_rde_event->type = RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED;
				} break;
			}

			_rde_event->data.mouse_event_data.button = (RDE_MOUSE_BUTTON_)_sdl_event->button.button;
		} break;

		case SDL_MOUSEMOTION: {
			_rde_event->type = RDE_EVENT_TYPE_MOUSE_MOVED;
			_rde_event->data.mouse_event_data.position = (rde_vec_2I) { _sdl_event->motion.x, _sdl_event->motion.y };
		};
	}
}

rde_event rde_engine_sdl_event_to_rde_event(SDL_Event* _sdl_event) {

	rde_event _event;

	switch(_sdl_event->type) {
		case SDL_WINDOWEVENT:	rde_sdl_to_rde_helper_transform_window_event(_sdl_event, &_event); break;
		
		case SDL_DISPLAYEVENT:  rde_sdl_to_rde_helper_transform_display_event(_sdl_event, &_event); break;
		
		case SDL_KEYDOWN:
		case SDL_KEYUP: rde_sdl_to_rde_helper_transform_keyboard_event(_sdl_event, &_event); break;
		
		case SDL_MOUSEMOTION:
		case SDL_MOUSEWHEEL:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP: rde_sdl_to_rde_helper_transform_mouse_button_event(_sdl_event, &_event); break;
	}

	return _event;
}

// ======================= API ===========================

void rde_events_window_consume_events(rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - RDE_WIN_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_WIN_EVENT_COUNT) {
		ENGINE.window_events[_event_index](_window, _event);
	} else {
		printf("Window Event: %i, not handled \n", _event->type);
	}
}


void rde_events_display_consume_events(rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - RDE_DISPLAY_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_DISPLAY_EVENT_COUNT) {
		ENGINE.display_events[_event_index](_window, _event);
	} else {
		printf("Display Event: %i, not handled \n", _event->type);
	}
}

void rde_events_keyboard_consume_events(rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - RDE_KEY_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_KEY_EVENT_INIT) {
		ENGINE.key_events[_event_index](_window, _event);
	} else {
		printf("Key Event: %i, not handled \n", _event->type);
	}
}

void rde_events_mouse_consume_events(rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - RDE_MOUSE_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_MOUSE_EVENT_INIT) {
		ENGINE.mouse_events[_event_index](_window, _event);
	} else {
		printf("Mouse Event: %i, not handled \n", _event->type);
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

bool rde_events_is_key_just_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key) {
	return _window->key_states[_key] == RDE_INPUT_STATUS_JUST_PRESSED;
}

bool rde_events_is_key_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key) {
	return _window->key_states[_key] == RDE_INPUT_STATUS_KEEP_PRESSED || _window->key_states[_key] == RDE_INPUT_STATUS_JUST_PRESSED;
}

bool rde_events_is_key_just_released(rde_window* _window, RDE_KEYBOARD_KEY_ _key) {
	return _window->key_states[_key] == RDE_INPUT_STATUS_JUST_RELEASED;
}

bool rde_events_is_mouse_button_just_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button) {
	return _window->mouse_states[_button] == RDE_INPUT_STATUS_JUST_PRESSED;
}

bool rde_events_is_mouse_button_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button) {
	return _window->mouse_states[_button] == RDE_INPUT_STATUS_KEEP_PRESSED || _window->mouse_states[_button] == RDE_INPUT_STATUS_JUST_PRESSED;
}

bool rde_events_is_mouse_button_just_released(rde_window* _window, RDE_MOUSE_BUTTON_ _button) {
	return _window->mouse_states[_button] == RDE_INPUT_STATUS_JUST_RELEASED;
}

rde_vec_2I rde_events_mouse_get_position(rde_window* _window) {
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	return (rde_vec_2I) { _window->mouse_position.x - _window_size.x * 0.5f, _window->mouse_position.y - _window_size.y * 0.5f };
}

void rde_events_sync_events(rde_window* _window) {
	for(int _i = 0; _i < RDE_AMOUNT_OF_MOUSE_BUTTONS; _i++) {
		if(_window->mouse_states[_i] == RDE_INPUT_STATUS_JUST_PRESSED) {
			_window->mouse_states[_i] = RDE_INPUT_STATUS_KEEP_PRESSED;
		}

		if(_window->mouse_states[_i] == RDE_INPUT_STATUS_JUST_RELEASED) {
			_window->mouse_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
		}
	}

	for(int _i = 0; _i < RDE_AMOUNT_OF_KEYS; _i++) {
		if(_window->key_states[_i] == RDE_INPUT_STATUS_JUST_PRESSED) {
			_window->key_states[_i] = RDE_INPUT_STATUS_KEEP_PRESSED;
		}

		if(_window->key_states[_i] == RDE_INPUT_STATUS_JUST_RELEASED) {
			_window->key_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
		}
	}
}