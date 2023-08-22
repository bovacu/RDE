#include "window/window.h"
#include "SDL2/SDL.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl32.h>
#elif IS_ANDROID()
#include <GLES3/gl32.h>
#elif IS_IOS()
#include <OpenGLES/ES3/gl.h>
#else
#include "glad/glad.h"
#endif

#include "private_structs.h"

#define WIN_EVENT_INIT (RDE_EVENT_TYPE_WINDOW_BEGIN + 1)
#define WIN_EVENT_COUNT (RDE_EVENT_TYPE_WINDOW_END - RDE_EVENT_TYPE_WINDOW_BEGIN)

typedef void (*event_window_callback)(rde_engine*, rde_window*, rde_event*);
event_window_callback events[WIN_EVENT_COUNT];


#define COMMON_CALLBACK_IMPLEMENTATION(_func_name, _callback_type, _callback_name, _extra_code) 				\
	inline void _func_name(rde_engine* _engine, rde_window* _window, rde_event* _event) {						\
		UNUSED(_engine);																						\
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
COMMON_CALLBACK_IMPLEMENTATION(window_display_changed, window_callbacks, on_window_moved, { _window->display_info.index = _event->data.window_event_data.display_index; })

rde_window* rde_window_create_windows_window(rde_engine* _engine);

rde_window* rde_window_create_window(rde_engine* _engine) {

	events[RDE_EVENT_TYPE_WINDOW_RESIZED - WIN_EVENT_INIT] 				= &window_resize;
	events[RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED - WIN_EVENT_INIT] 		= &window_focused_by_mouse;
	events[RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED - WIN_EVENT_INIT] 		= &window_unfocused_by_mouse;
	events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_FOCUSED - WIN_EVENT_INIT] 	= &window_focused_by_keyboard;
	events[RDE_EVENT_TYPE_WINDOW_KEYBOARD_UNFOCUSED - WIN_EVENT_INIT]	= &window_unfocused_by_keyboard;
	events[RDE_EVENT_TYPE_WINDOW_MOVED - WIN_EVENT_INIT] 				= &window_moved;
	events[RDE_EVENT_TYPE_WINDOW_MINIMIZED - WIN_EVENT_INIT] 			= &window_minimized;
	events[RDE_EVENT_TYPE_WINDOW_MAXIMIZED - WIN_EVENT_INIT] 			= &window_maximized;
	events[RDE_EVENT_TYPE_WINDOW_CLOSED - WIN_EVENT_INIT] 				= &window_closed;
	events[RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED - WIN_EVENT_INIT] 		= &window_display_changed;

	return rde_window_create_windows_window(_engine);
}

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

void rde_window_consume_event(rde_engine* _engine, rde_window* _window, rde_event* _event) {

	size_t _event_index = _event->type - WIN_EVENT_INIT;

	if(_event_index >= 0 && _event_index < WIN_EVENT_COUNT) {
		events[_event_index](_engine, _window, _event);
	} else {
		std::cout << "Event: " << _event->type << ", not handled" << std::endl;
	}
}