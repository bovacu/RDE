#include <iostream>

#include "rde.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl32.h>
#elif IS_ANDROID()
#include <GLES3/gl32.h>
#elif IS_IOS()
#include <OpenGLES/ES3/gl.h>
#else
#include "glad/glad.h"
#endif


void rde_events_window_create_events();
#define WIN_EVENT_INIT (RDE_EVENT_TYPE_WINDOW_BEGIN + 1)
#define WIN_EVENT_COUNT (RDE_EVENT_TYPE_WINDOW_END - RDE_EVENT_TYPE_WINDOW_BEGIN)
rde_event_func_outer window_events[WIN_EVENT_COUNT];
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_resize, window_callbacks, on_window_resize, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_focused_by_mouse, window_callbacks, on_window_focused_by_mouse, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_unfocused_by_mouse, window_callbacks, on_window_unfocused_by_mouse, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_focused_by_keyboard, window_callbacks, on_window_focused_by_keyboard, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_unfocused_by_keyboard, window_callbacks, on_window_unfocused_by_keyboard, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_moved, window_callbacks, on_window_moved, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_minimized, window_callbacks, on_window_minimized, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_maximized, window_callbacks, on_window_maximized, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_closed, window_callbacks, on_window_closed, { _engine->running = false; })
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_display_changed, window_callbacks, on_window_moved, { _window->display_info.index = _event->data.window_event_data.display_index; })

void rde_events_display_create_events();
#define DISPLAY_EVENT_INIT (RDE_EVENT_TYPE_DISPLAY_BEGIN + 1)
#define DISPLAY_EVENT_COUNT (RDE_EVENT_TYPE_DISPLAY_END - RDE_EVENT_TYPE_DISPLAY_BEGIN)
rde_event_func_outer display_events[DISPLAY_EVENT_COUNT];
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_connected, display_callbacks, on_display_connected, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_disconnected, display_callbacks, on_display_disconnected, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_changed_orientation, display_callbacks, on_display_changed_orientation, {})


void rde_engine_on_event(rde_engine* _engine);
void rde_engine_on_update(rde_engine* _engine, float _dt);
void rde_engine_on_fixed_update(rde_engine* _engine, float _fixed_dt);
void rde_engine_on_late_update(rde_engine* _engine, float _dt);
void rde_engine_on_render(rde_engine* _engine, float _dt);
void rde_engine_sync_events(rde_engine* _engine);




/// ============================ MATH =======================================




/// ============================ LOGGING ====================================




/// ============================ UTIL =======================================

void rde_sdl_to_rde_helper_transform_window_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	_rde_event->time_stamp = _sdl_event->window.timestamp;

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


/// ============================ ENGINE =====================================

rde_engine* rde_engine_create_engine(int _argc, char** _argv) {
	rde_engine* _engine = new rde_engine {  };
	rde_window* _default_window = rde_window_create_window(_engine);

	memset(_engine->engine_windows, 0, RDE_MAX_NUMBER_OF_WINDOWS);
	_engine->engine_windows[0] = _default_window;

	rde_events_window_create_events();
	rde_events_display_create_events();

	rde_engine_entry_point(_argc, _argv, _engine, _default_window);

	return _engine;
}

RDE_PLATFORM_TYPE_ rde_engine_get_platform(rde_engine* _engine) {
	return _engine->platform_type;
}

float rde_engine_get_fixed_delta(rde_engine* _engine) {
	return _engine->fixed_delta_time;
}

void rde_engine_set_fixed_delta(rde_engine* _engine, float _delta_time) {
	UNUSED(_engine)
		UNUSED(_delta_time)
		UNIMPLEMENTED("Not implemented")
}

void rde_engine_on_run(rde_engine* _engine) {

#if IS_MOBILE()
	SDL_SetEventFilter(rde_mobile_consume_events, _engine);
#endif

	while(_engine->running) {
		Uint64 _start = SDL_GetPerformanceCounter();
		_engine->fixed_time_step_accumulator += _engine->delta_time;

		rde_engine_on_event(_engine);

		rde_engine_on_update(_engine, _engine->delta_time);
		rde_engine_user_on_update(_engine->delta_time);

		while (_engine->fixed_time_step_accumulator >= _engine->fixed_delta_time) {
			_engine->fixed_time_step_accumulator -= _engine->fixed_delta_time;
			rde_engine_on_fixed_update(_engine, _engine->fixed_delta_time);
			rde_engine_user_on_fixed_update(_engine->fixed_delta_time);
		}

		rde_engine_on_late_update(_engine, _engine->delta_time);
		rde_engine_user_on_late_upadte(_engine->delta_time);

		rde_engine_on_render(_engine, _engine->delta_time);
		rde_engine_user_on_render(_engine->delta_time);

		rde_engine_sync_events(_engine);

		Uint64 _end = SDL_GetPerformanceCounter();
		float _elapsedMS = (float)(_end - _start) / (float)SDL_GetPerformanceFrequency();
		_engine->delta_time = _elapsedMS;
	}

	rde_engine_destroy_engine(_engine);
}

void rde_engine_init_imgui_layer(rde_engine* _engine) {
	UNUSED(_engine)
	UNIMPLEMENTED("Not implemented")
}

void rde_engine_end_imgui_layer(rde_engine* _engine) {
	UNUSED(_engine)
	UNIMPLEMENTED("Not implemented")
}

rde_window* rde_engine_get_window(rde_engine* _engine, int _index) {
	return _engine->engine_windows[_index];
}


bool rde_engine_is_running(rde_engine* _engine) {
	return _engine->running;
}

void rde_engine_set_running(rde_engine* _engine, bool _running) {
	_engine->running = _running;
}

rde_vec_2I rde_engine_get_display_size(rde_engine* _engine) {
	UNUSED(_engine)
	SDL_DisplayMode _displayMode;
	SDL_GetCurrentDisplayMode(0, &_displayMode);
	return { _displayMode.w, _displayMode.h };
}

void rde_engine_destroy_engine(rde_engine* _engine) {
	SDL_GL_DeleteContext(_engine->engine_windows[0]->sdl_gl_context);
	SDL_DestroyWindow(_engine->engine_windows[0]->sdl_window);
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
}

rde_event rde_engine_sdl_event_to_rde_event(SDL_Event* _sdl_event) {

	rde_event _event;

	switch(_sdl_event->type) {
		case SDL_WINDOWEVENT:	rde_sdl_to_rde_helper_transform_window_event(_sdl_event, &_event); break;
		case SDL_DISPLAYEVENT:  rde_sdl_to_rde_helper_transform_display_event(_sdl_event, &_event); break;
	}

	return _event;
}

void rde_engine_on_event(rde_engine* _engine) {
	SDL_Event _event;
        
	SDL_PumpEvents();
	
	// TODO: this now handles only one possible window, to manage multiple window events
	// a window must now its SDL window id and each event gives you which windowID the event
	// has happened on.

	while (SDL_PollEvent(&_event)) {
		rde_event _rde_event = rde_engine_sdl_event_to_rde_event(&_event);

		switch(_event.type) {
			case SDL_WINDOWEVENT:	rde_events_window_consume_events(_engine, _engine->engine_windows[0], &_rde_event); break;
			case SDL_DISPLAYEVENT: 	rde_events_display_consume_events(_engine, _engine->engine_windows[0], &_rde_event); break;
		}
	}
}

void rde_engine_on_update(rde_engine* _engine, float _dt) {
	UNUSED(_engine)
	UNUSED(_dt)
}

void rde_engine_on_fixed_update(rde_engine* _engine, float _fixed_dt) {
	UNUSED(_engine)
	UNUSED(_fixed_dt)
}

void rde_engine_on_late_update(rde_engine* _engine, float _dt) {
	UNUSED(_engine)
		UNUSED(_dt)
}

void rde_engine_on_render(rde_engine* _engine, float _dt) {
	UNUSED(_engine)
	UNUSED(_dt)
	
	if (!_engine->use_rde_batching_system) {
		return;
	}
}

void rde_engine_sync_events(rde_engine* _engine) {
	UNUSED(_engine)
}

rde_display_info* rde_engine_get_available_displays() {
	UNIMPLEMENTED("Not implemented");
	return nullptr;
}

void rde_engine_switch_window_display(rde_window* _window, size_t _new_display) {
	UNUSED(_window)
	UNUSED(_new_display)
	UNIMPLEMENTED("Not implemented");
}


/// ============================ WINDOW =====================================

#if IS_WINDOWS()
rde_window* rde_window_create_windows_window(rde_engine* _engine) {
	UNUSED(_engine);

	rde_window* _window = new rde_window {  };

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		
	#ifdef RENDER_DOC_COMPATIBILITY
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	#endif

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	_window->sdl_window = SDL_CreateWindow("Title", 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(_window->sdl_window == nullptr) {
		std::cout << "SDL window creation failed: " << SDL_GetError() << std::endl;
		exit(-1);
	}
	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);

	if(_window->sdl_gl_context == nullptr) {
		std::cout << "OpenGL context couldn't initialize -> " << SDL_GetError() << std::endl;
		exit(-1);
	}

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	std::cout << "GLAD and SDL2 loaded successfully" << std::endl;

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	_window->display_info.index = SDL_GetWindowDisplayIndex(_window->sdl_window);

	return _window;
	//setIcon(properties->projectData.iconPath);

	//refreshDpi();
}
#endif

#if IS_MAC()
rde_window* rde_window_create_mac_window(rde_engine* _engine) {
	UNIMPLEMENTED("Mac window creation is not implemented yet");
}
#endif

#if IS_LINUX()
rde_window* rde_window_create_linux_window(rde_engine* _engine) {
	UNIMPLEMENTED("Mac linux creation is not implemented yet");
}
#endif

#if IS_ANDROID()
rde_window* rde_window_create_android_window(rde_engine* _engine) {
	UNIMPLEMENTED("Mac android creation is not implemented yet");
}
#endif

#if IS_IOS()
rde_window* rde_window_create_ios_window(rde_engine* _engine) {
	UNIMPLEMENTED("Mac ios creation is not implemented yet");
}
#endif

#if 0
rde_window* rde_window_create_wasm_window(rde_engine* _engine) {
	UNIMPLEMENTED("Mac WASM creation is not implemented yet");
}
#endif

rde_window* rde_window_create_window(rde_engine* _engine) {
	// TODO: create window depending on platform
	return rde_window_create_windows_window(_engine);
}

void rde_window_set_callbacks(rde_window* _window, rde_window_callbacks _callbacks) {
	UNUSED(_window)
	UNUSED(_callbacks)
}

//RDE_FUNC_ND Vec2I			rde_window_get_window_size(rde_window* _window);
//RDE_FUNC	void			rde_window_set_window_size(rde_window* _window, const Vec2I& _size);
//
//RDE_FUNC_ND Vec2I			rde_window_get_position(rde_window* _window);
//RDE_FUNC_ND void			rde_window_set_position(rde_window* _window, const Vec2I& _position);
//
//RDE_FUNC_ND std::string 	rde_window_get_title(rde_window* _window);
//RDE_FUNC	void			rde_window_set_title(rde_window* _window, const std::string& _name);
//
//RDE_FUNC_ND bool			rde_window_is_fullscreen(rde_window* _window);
//RDE_FUNC 	void			rde_window_set_fullscreen(rde_window* _window, bool _fullscreen);
//
//RDE_FUNC_ND bool			rde_window_is_vsync_active(rde_window* _window);
//RDE_FUNC 	void			rde_window_set_vsync_active(rde_window* _window, bool _vsync);
//
//RDE_FUNC_ND SDL_Window* 	rde_window_get_native_sdl_window(rde_window* _window);
//RDE_FUNC_ND SDL_GLContext* 	rde_window_get_native_sdl_gl_window(rde_window* _window);
//
//RDE_FUNC	void			rde_window_set_icon(const std::string& _path_to_icon);
//
//RDE_FUNC_ND bool			rde_window_is_minimize(rde_window* _window);
//RDE_FUNC 	void			rde_window_minimize(rde_window* _window, bool _minimize);
//
//RDE_FUNC_ND bool			rde_window_is_maximized(rde_window* _window);
//RDE_FUNC 	void			rde_window_maximized(rde_window* _window, bool _maximized);
//
//RDE_FUNC_ND bool			rde_window_is_focused(rde_window* _window);
//
//RDE_FUNC_ND bool			rde_window_is_mouse_out_of_window_allowed(rde_window* _window);
//RDE_FUNC 	void			rde_window_allow_mouse_out_of_window(rde_window* _window, bool _allow_mouse_out_of_window);
//
//RDE_FUNC	void			rde_window_refresh_dpi(rde_window* _window);
//
//RDE_FUNC	void 			rde_window_destroy_window(Engine* _engine, rde_window _window);

/// ============================ EVENTS =====================================

void rde_events_window_create_events() {
	window_events[RDE_EVENT_TYPE_WINDOW_RESIZED - WIN_EVENT_INIT] 				= &window_resize;
	window_events[RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED - WIN_EVENT_INIT] 		= &window_focused_by_mouse;
	window_events[RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED - WIN_EVENT_INIT] 		= &window_unfocused_by_mouse;
	window_events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_FOCUSED - WIN_EVENT_INIT] 		= &window_focused_by_keyboard;
	window_events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_UNFOCUSED - WIN_EVENT_INIT]	= &window_unfocused_by_keyboard;
	window_events[RDE_EVENT_TYPE_WINDOW_MOVED - WIN_EVENT_INIT] 				= &window_moved;
	window_events[RDE_EVENT_TYPE_WINDOW_MINIMIZED - WIN_EVENT_INIT] 			= &window_minimized;
	window_events[RDE_EVENT_TYPE_WINDOW_MAXIMIZED - WIN_EVENT_INIT] 			= &window_maximized;
	window_events[RDE_EVENT_TYPE_WINDOW_CLOSED - WIN_EVENT_INIT] 				= &window_closed;
	window_events[RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED - WIN_EVENT_INIT] 		= &window_display_changed;
}

void rde_events_window_consume_events(rde_engine* _engine, rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - WIN_EVENT_INIT;

	if(_event_index >= 0 && _event_index < WIN_EVENT_COUNT) {
		window_events[_event_index](_engine, _window, _event);
	} else {
		std::cout << "Window Event: " << _event->type << ", not handled" << std::endl;
	}
}

void rde_events_display_create_events() {
	display_events[RDE_EVENT_TYPE_DISPLAY_CONNECTED - DISPLAY_EVENT_INIT] 			= &display_connected;
	display_events[RDE_EVENT_TYPE_DISPLAY_DISCONNECTED - DISPLAY_EVENT_INIT] 		= &display_disconnected;
	display_events[RDE_EVENT_TYPE_DISPLAY_CHANGED_ORIENTATION - DISPLAY_EVENT_INIT] = &display_changed_orientation;
}

void rde_events_display_consume_events(rde_engine* _engine, rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - DISPLAY_EVENT_INIT;

	if(_event_index >= 0 && _event_index < DISPLAY_EVENT_COUNT) {
		display_events[_event_index](_engine, _window, _event);
	} else {
		std::cout << "Display Event: " << _event->type << ", not handled" << std::endl;
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

/// ============================ RENDERING ==================================




/// ============================ AUDIO ======================================




/// ============================ PHYSICS ====================================




/// ============================ FILE SYSTEM ================================