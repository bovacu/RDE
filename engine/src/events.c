void rde_inner_events_window_create_events();
void rde_inner_events_display_create_events();
void rde_inner_events_key_create_events();
void rde_inner_events_mouse_button_create_events();
void rde_inner_events_drag_and_drop_create_events();
void rde_inner_events_mobile_create_events();
void rde_inner_event_sdl_to_rde_helper_transform_window_event(SDL_Event* _sdl_event, rde_event* _rde_event);
void rde_inner_event_sdl_to_rde_helper_transform_display_event(SDL_Event* _sdl_event, rde_event* _rde_event);
void rde_inner_event_sdl_to_rde_helper_transform_keyboard_event(SDL_Event* _sdl_event, rde_event* _rde_event);
void rde_inner_event_sdl_to_rde_helper_transform_mouse_button_event(SDL_Event* _sdl_event, rde_event* _rde_event);
void rde_inner_event_sdl_to_rde_helper_transform_drop_event(SDL_Event* _sdl_event, rde_event* _rde_event);
void rde_inner_event_sdl_to_rde_helper_transform_mobile_event(SDL_Event* _sdl_event, rde_event* _rde_event);
rde_event rde_inner_event_sdl_event_to_rde_event(SDL_Event* _sdl_event);

COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_event_window_resize, {
#ifdef RDE_RENDERING_MODULE
	rde_vec_2I _new_window_size = _event->data.window_event_data.size;

	if(ENGINE.antialiasing.samples > 0) {
		rde_inner_rendering_destroy_current_antialiasing_config();
		rde_rendering_set_antialiasing(_window, (RDE_ANTIALIASING_)ENGINE.antialiasing.samples);		
	}

	rde_rendering_render_texture_update(DEFAULT_RENDER_TEXTURE, _new_window_size.x, _new_window_size.y);
	rde_rendering_render_texture_enable(DEFAULT_RENDER_TEXTURE);
#endif
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_focused_by_mouse, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_unfocused_by_mouse, {
    memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_focused_by_keyboard, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_unfocused_by_keyboard, {
	memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_moved, {
    memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_minimized, {
    memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_maximized, {
    memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_closed, { 
	size_t _existing_non_destroyed_windows = 0;
	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_windows; _i++) {
		if(&ENGINE.windows[_i] != _window && ENGINE.windows[_i].sdl_window != NULL) {
			_existing_non_destroyed_windows++;
		}
	}

	if(_existing_non_destroyed_windows > 0) {
		rde_window_destroy_window(_window);
	}

	ENGINE.running = _existing_non_destroyed_windows > 0;
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_display_changed, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_connected, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_disconnected, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_changed_orientation, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(key_pressed, {
	if(_window->key_states[_event->data.key_event_data.key] == RDE_INPUT_STATUS_UNINITIALIZED || _window->key_states[_event->data.key_event_data.key] == RDE_INPUT_STATUS_JUST_RELEASED) {
		_window->key_states[_event->data.key_event_data.key] = RDE_INPUT_STATUS_JUST_PRESSED;
	} else if(_window->key_states[_event->data.key_event_data.key] == RDE_INPUT_STATUS_JUST_PRESSED) {
		_window->key_states[_event->data.key_event_data.key] = RDE_INPUT_STATUS_KEEP_PRESSED;
	}
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(key_released, {
	_window->key_states[_event->data.key_event_data.key] = RDE_INPUT_STATUS_JUST_RELEASED;
})

COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mouse_pressed, {
	if(_window->mouse_states[_event->data.mouse_event_data.button] == RDE_INPUT_STATUS_UNINITIALIZED || _window->mouse_states[_event->data.mouse_event_data.button] == RDE_INPUT_STATUS_JUST_RELEASED) {
		_window->mouse_states[_event->data.mouse_event_data.button] = RDE_INPUT_STATUS_JUST_PRESSED;
	} else if(_window->key_states[_event->data.mouse_event_data.button] == RDE_INPUT_STATUS_JUST_PRESSED) {
		_window->mouse_states[_event->data.mouse_event_data.button] = RDE_INPUT_STATUS_KEEP_PRESSED;
	}
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mouse_released, {
	_window->mouse_states[_event->data.mouse_event_data.button] = RDE_INPUT_STATUS_JUST_RELEASED;
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mouse_moved, {
	_window->mouse_position = _event->data.mouse_event_data.position;
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mouse_scrolled, {
	_window->mouse_scroll = _event->data.mouse_event_data.scrolled;
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(drop_file, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_touch_down, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_touch_up, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_touch_moved, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_multi_touch, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_terminating, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_low_memory, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_will_enter_background, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_did_enter_background, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_will_enter_foreground, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_did_enter_foreground, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_locale_changed, {})

void rde_inner_events_window_create_events() {
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_RESIZED - RDE_WIN_EVENT_INIT] = &rde_inner_event_window_resize;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED - RDE_WIN_EVENT_INIT] = &rde_inner_window_focused_by_mouse;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED - RDE_WIN_EVENT_INIT] = &rde_inner_window_unfocused_by_mouse;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_FOCUSED - RDE_WIN_EVENT_INIT] = &rde_inner_window_focused_by_keyboard;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_UNFOCUSED - RDE_WIN_EVENT_INIT]	= &rde_inner_window_unfocused_by_keyboard;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MOVED - RDE_WIN_EVENT_INIT] = &rde_inner_window_moved;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MINIMIZED - RDE_WIN_EVENT_INIT] = &rde_inner_window_minimized;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_MAXIMIZED - RDE_WIN_EVENT_INIT] = &rde_inner_window_maximized;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_CLOSED - RDE_WIN_EVENT_INIT] = &rde_inner_window_closed;
	ENGINE.window_events[RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED - RDE_WIN_EVENT_INIT] = &window_display_changed;
}

void rde_inner_events_display_create_events() {
	ENGINE.display_events[RDE_EVENT_TYPE_DISPLAY_CONNECTED - RDE_DISPLAY_EVENT_INIT] = &display_connected;
	ENGINE.display_events[RDE_EVENT_TYPE_DISPLAY_DISCONNECTED - RDE_DISPLAY_EVENT_INIT] = &display_disconnected;
	ENGINE.display_events[RDE_EVENT_TYPE_DISPLAY_CHANGED_ORIENTATION - RDE_DISPLAY_EVENT_INIT] = &display_changed_orientation;
}

void rde_inner_events_key_create_events() {
	ENGINE.key_events[RDE_EVENT_TYPE_KEY_PRESSED - RDE_KEY_EVENT_INIT] = &key_pressed;
	ENGINE.key_events[RDE_EVENT_TYPE_KEY_RELEASED - RDE_KEY_EVENT_INIT] = &key_released;
	// TODO: implement the other ones:
	//		- RDE_EVENT_TYPE_KEY_TYPED
	//		- RDE_EVENT_TYPE_TEXT_TYPED
}

void rde_inner_events_mouse_button_create_events() {
	ENGINE.mouse_events[RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED - RDE_MOUSE_EVENT_INIT] = &mouse_pressed;
	ENGINE.mouse_events[RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED - RDE_MOUSE_EVENT_INIT] = &mouse_released;
	ENGINE.mouse_events[RDE_EVENT_TYPE_MOUSE_MOVED - RDE_MOUSE_EVENT_INIT] = &mouse_moved;
	ENGINE.mouse_events[RDE_EVENT_TYPE_MOUSE_SCROLLED - RDE_MOUSE_EVENT_INIT] = &mouse_scrolled;
}

void rde_inner_events_drag_and_drop_create_events() {
	ENGINE.drag_and_drop_events[RDE_EVENT_TYPE_DRAG_AND_DROP_FILE - RDE_DRAG_AND_DROP_EVENT_INIT] = &drop_file;
}

void rde_inner_events_mobile_create_events() {
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN - RDE_MOBILE_EVENT_INIT] = &mobile_touch_down;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_TOUCH_UP - RDE_MOBILE_EVENT_INIT] = &mobile_touch_up;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED - RDE_MOBILE_EVENT_INIT] = &mobile_touch_moved;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_MULTI_TOUCH - RDE_MOBILE_EVENT_INIT] = &mobile_multi_touch;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_TERMINATING - RDE_MOBILE_EVENT_INIT] = &mobile_terminating;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_LOW_MEMORY - RDE_MOBILE_EVENT_INIT] = &mobile_low_memory;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_WILL_ENTER_BACKGROUND - RDE_MOBILE_EVENT_INIT] = &mobile_will_enter_background;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_DID_ENTER_BACKGROUND - RDE_MOBILE_EVENT_INIT] = &mobile_did_enter_background;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_WILL_ENTER_FOREGROUND - RDE_MOBILE_EVENT_INIT] = &mobile_will_enter_foreground;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_DID_ENTER_FOREGROUND - RDE_MOBILE_EVENT_INIT] = &mobile_did_enter_foreground;
	ENGINE.mobile_events[RDE_EVENT_TYPE_MOBILE_LOCALE_CHANGED - RDE_MOBILE_EVENT_INIT] = &mobile_locale_changed;
}

void rde_inner_event_sdl_to_rde_helper_transform_window_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	_rde_event->time_stamp = _sdl_event->window.timestamp;
	_rde_event->window_id = _sdl_event->window.windowID;

	switch (_sdl_event->window.event) {
		case SDL_WINDOWEVENT_RESIZED: {
			_rde_event->type = RDE_EVENT_TYPE_WINDOW_RESIZED;
			_rde_event->data.window_event_data.size.x = _sdl_event->window.data1;
			_rde_event->data.window_event_data.size.y = _sdl_event->window.data2;
		} break;

		case SDL_WINDOWEVENT_MOVED:	{
			_rde_event->type = RDE_EVENT_TYPE_WINDOW_MOVED;
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

void rde_inner_event_sdl_to_rde_helper_transform_display_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
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

void rde_inner_event_sdl_to_rde_helper_transform_keyboard_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	_rde_event->time_stamp = _sdl_event->key.timestamp;
	_rde_event->window_id = _sdl_event->key.windowID;

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

void rde_inner_event_sdl_to_rde_helper_transform_mouse_button_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	switch(_sdl_event->type) {
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP: {
			_rde_event->time_stamp = _sdl_event->button.timestamp;
			_rde_event->window_id = _sdl_event->button.windowID;

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
			_rde_event->time_stamp = _sdl_event->motion.timestamp;
			_rde_event->window_id = _sdl_event->motion.windowID;
			_rde_event->type = RDE_EVENT_TYPE_MOUSE_MOVED;
			_rde_event->data.mouse_event_data.position = (rde_vec_2I) { _sdl_event->motion.x, _sdl_event->motion.y };
		} break;

		case SDL_MOUSEWHEEL: {
			_rde_event->time_stamp = _sdl_event->wheel.timestamp;
			_rde_event->window_id = _sdl_event->wheel.windowID;
			_rde_event->type = RDE_EVENT_TYPE_MOUSE_SCROLLED;
			_rde_event->data.mouse_event_data.scrolled = (rde_vec_2F) { (float)_sdl_event->wheel.x, (float)_sdl_event->wheel.y };
		} break;
	}
}

void rde_inner_event_sdl_to_rde_helper_transform_drop_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	_rde_event->time_stamp = _sdl_event->drop.timestamp;
	_rde_event->window_id = _sdl_event->drop.windowID;

	switch(_sdl_event->type) {
		case SDL_DROPFILE: {
			_rde_event->type = RDE_EVENT_TYPE_DRAG_AND_DROP_FILE;
			_rde_event->data.drag_and_drop_data.file_path = _sdl_event->drop.file;
		}
	}
}

void rde_inner_event_sdl_to_rde_helper_transform_mobile_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	switch(_sdl_event->type) {
		case SDL_FINGERDOWN: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN;
			_rde_event->data.mobile_event_data.init_touch_position = (rde_vec_2I) { _sdl_event->tfinger.x, _sdl_event->tfinger.y };
			_rde_event->data.mobile_event_data.pressure = _sdl_event->tfinger.pressure;
			_rde_event->data.mobile_event_data.finger_id = _sdl_event->tfinger.fingerId;
			_rde_event->time_stamp = _sdl_event->tfinger.timestamp;
			_rde_event->window_id = _sdl_event->tfinger.windowID;
		} break;

		case SDL_FINGERUP:{
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_TOUCH_UP;
			_rde_event->data.mobile_event_data.end_touch_position = (rde_vec_2I) { _sdl_event->tfinger.x, _sdl_event->tfinger.y };
			_rde_event->data.mobile_event_data.pressure = _sdl_event->tfinger.pressure;
			_rde_event->data.mobile_event_data.finger_id = _sdl_event->tfinger.fingerId;
			_rde_event->time_stamp = _sdl_event->tfinger.timestamp;
			_rde_event->window_id = _sdl_event->tfinger.windowID;
		} break;

		case SDL_FINGERMOTION: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED;
			_rde_event->time_stamp = _sdl_event->tfinger.timestamp;
			_rde_event->window_id = _sdl_event->tfinger.windowID;
		} break;

		case SDL_DOLLARGESTURE:{
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_DOLLAR_GESTURE;
			_rde_event->time_stamp = _sdl_event->dgesture.timestamp;
		} break;

		case SDL_DOLLARRECORD:{
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_DOLLAR_RECORD;
			_rde_event->time_stamp = _sdl_event->dgesture.timestamp;
		} break;

		case SDL_MULTIGESTURE: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_MULTI_TOUCH;
			_rde_event->time_stamp = _sdl_event->mgesture.timestamp;
		} break;

		case SDL_APP_TERMINATING: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_TERMINATING;
		} break;

		case SDL_APP_LOWMEMORY: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_LOW_MEMORY;
		} break;

		case SDL_APP_WILLENTERBACKGROUND: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_WILL_ENTER_BACKGROUND;
		} break;

		case SDL_APP_DIDENTERBACKGROUND: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_DID_ENTER_BACKGROUND;
		} break;

		case SDL_APP_WILLENTERFOREGROUND: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_WILL_ENTER_FOREGROUND;
		} break;

		case SDL_APP_DIDENTERFOREGROUND: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_DID_ENTER_FOREGROUND;
		} break;

		case SDL_LOCALECHANGED: {
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_LOCALE_CHANGED;
		} break;

	}
}

rde_event rde_inner_event_sdl_event_to_rde_event(SDL_Event* _sdl_event) {

	rde_event _event = rde_struct_create_event();

	switch(_sdl_event->type) {
		case SDL_WINDOWEVENT:	rde_inner_event_sdl_to_rde_helper_transform_window_event(_sdl_event, &_event); break;
		
		case SDL_DISPLAYEVENT:  rde_inner_event_sdl_to_rde_helper_transform_display_event(_sdl_event, &_event); break;
		
		case SDL_KEYDOWN:
		case SDL_KEYUP: rde_inner_event_sdl_to_rde_helper_transform_keyboard_event(_sdl_event, &_event); break;
		
		case SDL_MOUSEMOTION:
		case SDL_MOUSEWHEEL:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP: rde_inner_event_sdl_to_rde_helper_transform_mouse_button_event(_sdl_event, &_event); break;

		case SDL_DROPFILE: rde_inner_event_sdl_to_rde_helper_transform_drop_event(_sdl_event, &_event); break;
	
		case SDL_FINGERDOWN:
		case SDL_FINGERUP:
		case SDL_FINGERMOTION: 
		case SDL_DOLLARGESTURE:
		case SDL_DOLLARRECORD:
		case SDL_MULTIGESTURE: 
		case SDL_APP_TERMINATING:
		case SDL_APP_LOWMEMORY:
		case SDL_APP_WILLENTERBACKGROUND:
		case SDL_APP_DIDENTERBACKGROUND:
		case SDL_APP_WILLENTERFOREGROUND:
		case SDL_APP_DIDENTERFOREGROUND:
		case SDL_LOCALECHANGED: rde_inner_event_sdl_to_rde_helper_transform_mobile_event(_sdl_event, &_event); break;
	}

	return _event;
}




// ==============================================================================
// =									API										=
// ==============================================================================

void rde_events_window_consume_events(rde_event* _event, rde_window* _window) {
	size_t _event_index = _event->type - RDE_WIN_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_WIN_EVENT_COUNT) {
		ENGINE.window_events[_event_index](_event, _window);
	} else {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_ERROR_EVENT_NOT_HANDLED, "Window", _event->type);
	}
}


void rde_events_display_consume_events(rde_event* _event, rde_window* _window) {
	size_t _event_index = _event->type - RDE_DISPLAY_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_DISPLAY_EVENT_COUNT) {
		ENGINE.display_events[_event_index](_event, _window);
	} else {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_ERROR_EVENT_NOT_HANDLED, "Display", _event->type);
	}
}

void rde_events_keyboard_consume_events(rde_event* _event, rde_window* _window) {
	size_t _event_index = _event->type - RDE_KEY_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_KEY_EVENT_INIT) {
		ENGINE.key_events[_event_index](_event, _window);
	} else {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_ERROR_EVENT_NOT_HANDLED, "Key", _event->type);
	}
}

void rde_events_mouse_consume_events(rde_event* _event, rde_window* _window) {
	size_t _event_index = _event->type - RDE_MOUSE_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_MOUSE_EVENT_INIT) {
		ENGINE.mouse_events[_event_index](_event, _window);
	} else {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_ERROR_EVENT_NOT_HANDLED, "Mouse", _event->type);
	}
}

void rde_events_drag_and_drop_consume_events(rde_event* _event, rde_window* _window) {
	size_t _event_index = _event->type - RDE_DRAG_AND_DROP_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_DRAG_AND_DROP_EVENT_INIT) {
		ENGINE.mobile_events[_event_index](_event, _window);
	} else {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_ERROR_EVENT_NOT_HANDLED, "Drag N Drop", _event->type);
	}
}

void rde_events_mobile_consume_events(rde_event* _event, rde_window* _window) {
	size_t _event_index = _event->type - RDE_MOBILE_EVENT_INIT;

	if(_event_index >= 0 && _event_index < RDE_MOBILE_EVENT_INIT) {
		ENGINE.mobile_events[_event_index](_event, _window);
	} else {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_ERROR_EVENT_NOT_HANDLED, "Mobile", _event->type);
	}
}

#if IS_MOBILE()
int rde_events_mobile_consume_events_callback_wrapper(void* _user_data, SDL_Event* _event) {
	static bool _terminated = false;

	switch(_event->type) {
		case SDL_APP_TERMINATING: {
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Terminating");
			_terminated = true;
			rde_engine_destroy_engine();
		} break;

		case SDL_APP_LOWMEMORY: {
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Low Memory");
		} break;

		case SDL_APP_WILLENTERBACKGROUND: {
			if(_terminated) {
				return 1;
			}
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Will Enter Background");
			ENGINE.pause_loop = true;
		} break;

		case SDL_APP_DIDENTERBACKGROUND: {
			if(_terminated) {
				return 1;
			}
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Did Enter Background");
			ENGINE.pause_loop = true;
		} break;

		case SDL_APP_WILLENTERFOREGROUND: {
			if(_terminated) {
				return 1;
			}
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Will Enter Foreground");
			ENGINE.pause_loop = false;
		} break;

		case SDL_APP_DIDENTERFOREGROUND: {
			if(_terminated) {
				return 1;
			}
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Did Enter Foreground");
			ENGINE.pause_loop = false;
		} break;

		case SDL_LOCALECHANGED: {
			if(_terminated) {
				return 1;
			}
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Locale changed");
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

rde_vec_2F rde_events_mouse_get_scrolled(rde_window* _window) {
	rde_vec_2F _scrolled = _window->mouse_scroll;
	_window->mouse_scroll = (rde_vec_2F) { 0.f, 0.f };
	return _scrolled;
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