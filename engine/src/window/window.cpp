#include "window/window.h"
#include "structs.h"
#include "SDL2/SDL.h"
#include <unordered_map>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl32.h>
#elif IS_ANDROID()
#include <GLES3/gl32.h>
#elif IS_IOS()
#include <OpenGLES/ES3/gl.h>
#else
#include "glad/glad.h"
#endif

typedef void (*event_window_callback)(rde_engine*, rde_window*, rde_event*);
std::unordered_map<RDE_EVENT_TYPE_, event_window_callback> events;


#define COMMON_CALLBACK_IMPLEMENTATION(_func_name, _callback_type, _callback_name, _extra_code) 	\
	inline void _func_name(rde_engine* _engine, rde_window* _window, rde_event* _event) {						\
		if(!_window->_callback_type._callback_name.block_engine_default_implementation) {						\
			_extra_code																							\
		}																										\
																												\
		if(_window->_callback_type._callback_name._callback != nullptr) {										\
			_window->_callback_type._callback_name._callback(_event);											\
		}																										\
	}																											


COMMON_CALLBACK_IMPLEMENTATION(window_resize, window_callbacks, on_window_resize, {})
COMMON_CALLBACK_IMPLEMENTATION(window_focused_by_mouse, window_callbacks, on_window_focused_by_mouse, {})
COMMON_CALLBACK_IMPLEMENTATION(window_unfocused_by_mouse, window_callbacks, on_window_unfocused_by_mouse, {})
COMMON_CALLBACK_IMPLEMENTATION(window_focused_by_keyboard, window_callbacks, on_window_focused_by_keyboard, {})
COMMON_CALLBACK_IMPLEMENTATION(window_unfocused_by_keyboard, window_callbacks, on_window_unfocused_by_keyboard, {})
COMMON_CALLBACK_IMPLEMENTATION(window_moved, window_callbacks, on_window_moved, {})
COMMON_CALLBACK_IMPLEMENTATION(window_minimized, window_callbacks, on_window_minimized, {})
COMMON_CALLBACK_IMPLEMENTATION(window_maximized, window_callbacks, on_window_maximized, {})
COMMON_CALLBACK_IMPLEMENTATION(window_closed, window_callbacks, on_window_closed, { _engine->running = false; })
COMMON_CALLBACK_IMPLEMENTATION(window_display_changed, window_callbacks, on_window_moved, { _window->display_of_window = _event->data.window_event_data.display_index; })

rde_window* rde_window_create_windows_window(rde_engine* _engine);

rde_window* rde_window_create_window(rde_engine* _engine) {

	events[RDE_EVENT_TYPE_WINDOW_RESIZED] 			= &window_resize;
	events[RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED] 	= &window_focused_by_mouse;
	events[RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED] 	= &window_unfocused_by_mouse;
	events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_FOCUSED] 	= &window_focused_by_keyboard;
	events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_UNFOCUSED]= &window_unfocused_by_keyboard;
	events[RDE_EVENT_TYPE_WINDOW_MOVED] 			= &window_moved;
	events[RDE_EVENT_TYPE_WINDOW_MINIMIZED] 		= &window_minimized;
	events[RDE_EVENT_TYPE_WINDOW_MAXIMIZED] 		= &window_maximized;
	events[RDE_EVENT_TYPE_WINDOW_CLOSED] 			= &window_closed;
	events[RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED] 	= &window_display_changed;

	return rde_window_create_windows_window(_engine);
}

#if IS_WINDOWS()
rde_window* rde_window_create_windows_window(rde_engine* _engine) {
	rde_window* _window = new rde_window;

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
	_window->sdl_window = SDL_CreateWindow("Title", 0, 0,
	                          1280, 720,
	                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

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

	_window->display_of_window = SDL_GetWindowDisplayIndex(_window->sdl_window);

	return _window;
	//setIcon(properties->projectData.iconPath);

	//refreshDpi();
}
#endif

#if IS_MAC()
rde_window* rde_window_create_mac_window(rde_engine* _engine) {
	
}
#endif

#if IS_LINUX()
rde_window* rde_window_create_linux_window(rde_engine* _engine) {
	
}
#endif

#if IS_ANDROID()
rde_window* rde_window_create_android_window(rde_engine* _engine) {
	
}
#endif

#if IS_IOS()
rde_window* rde_window_create_ios_window(rde_engine* _engine) {
	
}
#endif

#if 0
rde_window* rde_window_create_wasm_window(rde_engine* _engine) {
	
}
#endif

void rde_window_transform_sdl_event_to_rde_event(SDL_Event* _sdl_event, rde_event* _rde_event) {
	switch (_sdl_event->window.event) {

		// ======= WINDOW ========

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

		//  ======================

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
	}
}


void rde_window_consume_event(rde_engine* _engine, rde_window* _window, rde_event* _event) {
	if(events.find(_event->type) != events.end()) {
		events[_event->type](_engine, _window, _event);
	} else {
		std::cout << "Event: " << _event->type << ", not handled" << std::endl;
	}
}