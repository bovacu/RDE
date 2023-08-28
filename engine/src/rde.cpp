#include <iostream>

#include "rde.h"
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

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PNM
#define STBI_NO_TGA
#define STBI_NO_PNM
#define STBI_NO_HDR
#define STBI_MAX_DIMENSIONS (1 << 13)
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#if IS_WINDOWS()
#define STBIW_WINDOWS_UTF8
#endif
#include "stb/stb_image_write.h"

#include "glm/glm.hpp"

#ifdef RDE_AUDIO_MODULE
#include "SDL2/SDL_mixer.h"
#endif

// TODO: Not to forget
// 		- Set stbi_convert_iphone_png_to_rgb(1) and stbi_set_unpremultiply_on_load(1) for iOS, as 
//		  the format is BGRA instead of RGBA (problem solved by first method) and the second fixes
//		  an error that the first method can generate on some images.
//
//		- 2D rendering:
//			- Camera system
//			- Texture rendering
//			- Debug and non debug geometry rendering
//			- Spritebatch
//			- Text
//
//		- Basic 3D:
//			- Camera system
//			- 3D mathematical operations
//			- Renderer
//			- Mesh creation and loading
//			- Model loading
//			- Texturing and materials
//			- Instancing
//			- Lighting
//			- Model animations
//			- Text
//
//		- Other:
//			- Render Textures
//			- Particles
//
//		- TOOL: command line atlas packing tool for textures.
//				- https://dl.gi.de/server/api/core/bitstreams/f63b9b2f-8c00-4324-b758-22b7d36cb49e/content
//				- https://www.david-colson.com/2020/03/10/exploring-rect-packing.html
//
//		- TOOL: command line font atlas creator.
//
//		- TOOL: command line project creation, compilation and export.

struct rde_engine {
	bool instantiated = false;

	float delta_time = 0.0f;
	float fixed_delta_time = 0.016f;

	float fixed_time_step_accumulator = 0.0f;
	RDE_PLATFORM_TYPE_ platform_type = RDE_PLATFORM_TYPE_UNSUPPORTED;

	bool running = true;
	bool use_rde_2d_physics_system = true;

	rde_display_callbacks display_callbacks;
	rde_window_callbacks window_callbacks;
};

static rde_engine ENGINE;


// This inner structure defined here complements rde_window. In fact, this is not defined
// in the header file so the header file is 100% free of any external dependencies, which
// means no anoying linking problems.

struct rde_inner_window_info {
	SDL_Window* sdl_window = nullptr;
	SDL_GLContext sdl_gl_context;
	rde_window* self_pointer = nullptr;
};

rde_inner_window_info inner_window_info_array[RDE_MAX_NUMBER_OF_WINDOWS];


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
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_closed, window_callbacks, on_window_closed, { 
	rde_window_destroy_window(_window); 
	for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
		if(inner_window_info_array[_i].self_pointer != nullptr) {
			return;
		}
	}

	ENGINE.running = false;
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(window_display_changed, window_callbacks, on_window_moved, {})

void rde_events_display_create_events();
#define DISPLAY_EVENT_INIT (RDE_EVENT_TYPE_DISPLAY_BEGIN + 1)
#define DISPLAY_EVENT_COUNT (RDE_EVENT_TYPE_DISPLAY_END - RDE_EVENT_TYPE_DISPLAY_BEGIN)
rde_event_func_outer display_events[DISPLAY_EVENT_COUNT];
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_connected, display_callbacks, on_display_connected, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_disconnected, display_callbacks, on_display_disconnected, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(display_changed_orientation, display_callbacks, on_display_changed_orientation, {})



void rde_engine_on_event();
void rde_engine_on_update(float _dt);
void rde_engine_on_fixed_update(float _fixed_dt);
void rde_engine_on_late_update(float _dt);
void rde_engine_on_render(float _dt);
void rde_engine_sync_events();

static rde_engine_user_side_loop_func	rde_engine_user_on_update = nullptr;
static rde_engine_user_side_loop_func	rde_engine_user_on_fixed_update = nullptr;
static rde_engine_user_side_loop_func	rde_engine_user_on_late_update = nullptr;
static rde_engine_user_side_loop_func	rde_engine_user_on_render = nullptr;

/// ============================ MATH =======================================




/// ============================ LOGGING ====================================




/// ============================ UTIL =======================================

void rde_util_check_opengl_error(const char* _message) {
	GLenum _err;
	while((_err = glGetError()) != GL_NO_ERROR){
		switch(_err) {
			case GL_NO_ERROR:
				std::cout << "GL_ERROR " << "GL_NO_ERROR: No error has been recorded. The value of this symbolic constant is guaranteed to be 0. " << " | " << _message << " -> " << _err << std::endl;
				break;
			case GL_INVALID_ENUM:
				std::cout << "GL_ERROR " << "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.  " << " | " << _message << " -> " << _err << std::endl;
				break;
			case GL_INVALID_VALUE:
				std::cout << "GL_ERROR " << "GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.  " << " | " << _message << " -> " << _err << std::endl;
				break;
			case GL_INVALID_OPERATION:
				std::cout << "GL_ERROR " << "GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.  " << " | " << _message << " -> " << _err << std::endl;
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				std::cout << "GL_ERROR " << "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete." << " | " << _message << " -> " << _err << std::endl;
				break;
			case GL_OUT_OF_MEMORY:
				std::cout << "GL_ERROR " << "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. . " << " | " << _message << " -> " << _err << std::endl;
				break;
			case GL_STACK_UNDERFLOW:
				std::cout << "GL_ERROR " << "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow. " << " | " << _message << " -> " << _err << std::endl;
				break;
			case GL_STACK_OVERFLOW:
				std::cout << "GL_ERROR " << "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow. " << " | " << _message << " -> " << _err << std::endl;
				break;
			default:
				std::cout << "GL_ERROR " << "No description. " << " | " << _message << " -> " << _err << std::endl;
				break;
		}
	}
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


/// ============================ ENGINE =====================================

void rde_rendering_set_rendering_configuration() {
#if !IS_MOBILE()
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << ", Vendor: " << glGetString(GL_VENDOR) << ", GPU: " << glGetString(GL_RENDERER) << std::endl;
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if !IS_MOBILE() 
	#ifndef __EMSCRIPTEN__
		glEnable(GL_PROGRAM_POINT_SIZE);
		rde_util_check_opengl_error("Invalid Point size");
		glEnable(GL_LINE_SMOOTH);
		rde_util_check_opengl_error("Invalid Line Smooth");
		glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
		rde_util_check_opengl_error("Invalid Line Smooth Hint -> GL_NICEST");
	#endif
		
	#if !IS_MAC() && !IS_LINUX()
		#ifndef __EMSCRIPTEN__
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			rde_util_check_opengl_error("Invalid Point Smooth Hint -> GL_NICEST");
		#endif
	#endif

#endif
}

rde_window* rde_engine_create_engine(int _argc, char** _argv) {
	UNUSED(_argc)
	UNUSED(_argv)
	
	if(ENGINE.instantiated) {
		assert(false && "[ERROR]: Only one engine can be created");
		exit(-1);
	}

	ENGINE.instantiated = true;
	rde_window* _default_window = rde_window_create_window();

	rde_events_window_create_events();
	rde_events_display_create_events();

	rde_rendering_set_rendering_configuration();

	return _default_window;
}

void rde_setup_initial_info(const rde_end_user_mandatory_callbacks _end_user_callbacks) {
	rde_engine_user_on_update = _end_user_callbacks.on_update;
	rde_engine_user_on_fixed_update = _end_user_callbacks.on_fixed_update;
	rde_engine_user_on_late_update = _end_user_callbacks.on_late_update;
	rde_engine_user_on_render = _end_user_callbacks.on_render;
}

RDE_PLATFORM_TYPE_ rde_engine_get_platform() {
	return ENGINE.platform_type;
}

float rde_engine_get_fixed_delta() {
	return ENGINE.fixed_delta_time;
}

void rde_engine_set_fixed_delta(float _delta_time) {
	UNUSED(_delta_time)
	UNIMPLEMENTED("Not implemented")
}

void rde_engine_on_run() {

	assert(rde_engine_user_on_update != nullptr && "User-End callback 'rde_engine_user_on_update' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");
	assert(rde_engine_user_on_fixed_update != nullptr && "User-End callback 'rde_engine_user_on_fixed_update' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");
	assert(rde_engine_user_on_late_update != nullptr && "User-End callback 'rde_engine_user_on_late_upadte' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");
	assert(rde_engine_user_on_render != nullptr && "User-End callback 'rde_engine_user_on_render' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");

	#if IS_MOBILE()
	SDL_SetEventFilter(rde_mobile_consume_events);
	#endif

	while(ENGINE.running) {
		Uint64 _start = SDL_GetPerformanceCounter();
		ENGINE.fixed_time_step_accumulator += ENGINE.delta_time;

		rde_engine_on_event();

		rde_engine_on_update(ENGINE.delta_time);
		rde_engine_user_on_update(ENGINE.delta_time);

		while (ENGINE.fixed_time_step_accumulator >= ENGINE.fixed_delta_time) {
			ENGINE.fixed_time_step_accumulator -= ENGINE.fixed_delta_time;
			rde_engine_on_fixed_update(ENGINE.fixed_delta_time);
			rde_engine_user_on_fixed_update(ENGINE.fixed_delta_time);
		}

		rde_engine_on_late_update(ENGINE.delta_time);
		rde_engine_user_on_late_update(ENGINE.delta_time);

		rde_engine_on_render(ENGINE.delta_time);
		rde_engine_user_on_render(ENGINE.delta_time);

		rde_engine_sync_events();

		Uint64 _end = SDL_GetPerformanceCounter();
		float _elapsedMS = (float)(_end - _start) / (float)SDL_GetPerformanceFrequency();
		ENGINE.delta_time = _elapsedMS;
	}

	rde_engine_destroy_engine();
}

void rde_engine_init_imgui_layer() {
	UNIMPLEMENTED("Not implemented")
}

void rde_engine_end_imgui_layer() {
	UNIMPLEMENTED("Not implemented")
}

bool rde_engine_is_running() {
	return ENGINE.running;
}

void rde_engine_set_running(bool _running) {
	ENGINE.running = _running;
}

rde_vec_2I rde_engine_get_display_size() {
	
	SDL_DisplayMode _displayMode;
	SDL_GetCurrentDisplayMode(0, &_displayMode);
	return { _displayMode.w, _displayMode.h };
}

void rde_engine_use_rde_2d_physics_system(bool _use) {
	ENGINE.use_rde_2d_physics_system = _use;
}

bool rde_engine_is_vsync_active() {
	return SDL_GL_GetSwapInterval() == 1;
}

void rde_engine_set_vsync_active(bool _vsync) {
	SDL_GL_SetSwapInterval(_vsync ? 1 : 0);
}

void rde_engine_destroy_engine() {
	for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
		if(inner_window_info_array[_i].self_pointer == nullptr) {
			continue;
		}

		rde_window_destroy_window(inner_window_info_array[_i].self_pointer);
	}

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

void rde_engine_on_event() {
	;

	SDL_Event _event;
        
	SDL_PumpEvents();

	while (SDL_PollEvent(&_event)) {
		rde_event _rde_event = rde_engine_sdl_event_to_rde_event(&_event);
			
		for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
			rde_inner_window_info* _window_info = &inner_window_info_array[_i];

			if(_window_info->self_pointer == nullptr) {
				continue;
			}

			// TODO: Use the if statement on the following event types:
			//			- SDL_KeyboardEvent
			//			- SDL_TextEditingEvent
			//			- SDL_TextInputEvent
			//			- SDL_MouseMotionEvent
			//			- SDL_MouseButtonEvent
			//			- SDL_MouseWheelEvent
			//			- SDL_TouchFingerEvent
			//			- SDL_DropEvent
			//
			//		Also, but without the if, still need to implement:
			//			- SDL_ControllerAxisEvent
			//			- SDL_ControllerButtonEvent
			//			- SDL_ControllerDeviceEvent
			//			- SDL_AudioDeviceEvent
			switch(_event.type) {
				case SDL_WINDOWEVENT:	{
					if(SDL_GetWindowID(_window_info->sdl_window) != _rde_event.window_id) {
						continue;
					}
					rde_events_window_consume_events(_window_info->self_pointer, &_rde_event);
				} break;
				case SDL_DISPLAYEVENT: 	rde_events_display_consume_events(_window_info->self_pointer, &_rde_event); break;
			}
		}
	}
}

void rde_engine_on_update(float _dt) {
	UNUSED(_dt)
}

void rde_engine_on_fixed_update(float _fixed_dt) {
	UNUSED(_fixed_dt)
}

void rde_engine_on_late_update(float _dt) {
	UNUSED(_dt)
}

void rde_engine_on_render(float _dt) {
	UNUSED(_dt)
}

void rde_engine_sync_events() {
	
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

rde_window* rde_engine_get_focused_window() {
	SDL_Window* _window = SDL_GetMouseFocus();

	for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
		rde_inner_window_info* _info = &inner_window_info_array[_i];
		
		if(_info->sdl_window == _window) {
			return _info->self_pointer;
		}
	}

	return nullptr;
}

/// ============================ WINDOW =====================================

#if IS_WINDOWS()
rde_window* rde_window_create_windows_window(size_t _free_window_index) {
	rde_window* _window = new rde_window {  };
	_window->id = _free_window_index;

	rde_inner_window_info* _window_info = &inner_window_info_array[_free_window_index];
	_window_info->self_pointer = _window;

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

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);
	_window_info->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(_window_info->sdl_window == nullptr) {
		std::cout << "SDL window creation failed: " << SDL_GetError() << std::endl;
		exit(-1);
	}
	_window_info->sdl_gl_context = SDL_GL_CreateContext(_window_info->sdl_window);

	if(_window_info->sdl_gl_context == nullptr) {
		std::cout << "OpenGL context couldn't initialize -> " << SDL_GetError() << std::endl;
		exit(-1);
	}

	SDL_GL_MakeCurrent(_window_info->sdl_window, _window_info->sdl_gl_context);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	std::cout << "GLAD and SDL2 loaded successfully" << std::endl;

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window_info->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window_info->sdl_window, SDL_TRUE);

	return _window;
	//setIcon(properties->projectData.iconPath);

	//refreshDpi();
}
#endif

#if IS_MAC()
rde_window* rde_window_create_mac_window() {
	UNIMPLEMENTED("Mac window creation is not implemented yet");
}
#endif

#if IS_LINUX()
rde_window* rde_window_create_linux_window(size_t _free_window_index) {
	rde_window* _window = new rde_window {  };
	_window->id = _free_window_index;

	rde_inner_window_info* _window_info = &inner_window_info_array[_free_window_index];
	_window_info->self_pointer = _window;

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

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);
	_window_info->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(_window_info->sdl_window == nullptr) {
		std::cout << "SDL window creation failed: " << SDL_GetError() << std::endl;
		exit(-1);
	}
	_window_info->sdl_gl_context = SDL_GL_CreateContext(_window_info->sdl_window);

	if(_window_info->sdl_gl_context == nullptr) {
		std::cout << "OpenGL context couldn't initialize -> " << SDL_GetError() << std::endl;
		exit(-1);
	}

	SDL_GL_MakeCurrent(_window_info->sdl_window, _window_info->sdl_gl_context);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	std::cout << "GLAD and SDL2 loaded successfully" << std::endl;

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window_info->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window_info->sdl_window, SDL_TRUE);

	return _window;
	//setIcon(properties->projectData.iconPath);

	//refreshDpi();
}
#endif

#if IS_ANDROID()
rde_window* rde_window_create_android_window(size_t _free_window_index) {
	UNIMPLEMENTED("Mac android creation is not implemented yet");
}
#endif

#if IS_IOS()
rde_window* rde_window_create_ios_window(size_t _free_window_index) {
	UNIMPLEMENTED("Mac ios creation is not implemented yet");
}
#endif

#if 0
rde_window* rde_window_create_wasm_window(size_t _free_window_index) {
	UNIMPLEMENTED("Mac WASM creation is not implemented yet");
}
#endif

rde_window* rde_window_create_window() {

	size_t _free_window_index = 0;

	for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
		if(inner_window_info_array[_i].self_pointer != nullptr) {

			if(_i == RDE_MAX_NUMBER_OF_WINDOWS - 1) {
				assert(false && "[ERROR]: Tried to create a new window but the limit of simultaneous windows has been reached.");
			}

			continue;
		}

		_free_window_index = _i;
		break;
	}

	// TODO: create window depending on platform
#if IS_WINDOWS()
	return rde_window_create_windows_window(_free_window_index);
#elif IS_LINUX()
	return rde_window_create_linux_window(_free_window_index);
#else
	assert(false && "[Error]: Unsupported or unimplemented platform");
#endif
}

void rde_window_set_callbacks(rde_window* _window, rde_window_callbacks _callbacks) {
	UNUSED(_window)
	UNUSED(_callbacks)
}

rde_vec_2I rde_window_get_window_size(rde_window* _window) {
	rde_vec_2I _size {  };
	SDL_GetWindowSize(inner_window_info_array[_window->id].sdl_window, &_size.x, &_size.y);
	return _size;
}
void rde_window_set_window_size(rde_window* _window, const rde_vec_2I _size) {
	SDL_SetWindowSize(inner_window_info_array[_window->id].sdl_window, _size.x, _size.y);
}

rde_vec_2I rde_window_get_position(rde_window* _window) {
	rde_vec_2I _position = {  };
	SDL_GetWindowPosition(inner_window_info_array[_window->id].sdl_window, &_position.x, &_position.y);
	return _position;
}

void rde_window_set_position(rde_window* _window, const rde_vec_2I _position) {
	SDL_SetWindowPosition(inner_window_info_array[_window->id].sdl_window, _position.x, _position.y);
}

const char* rde_window_get_title(rde_window* _window) {
	return SDL_GetWindowTitle(inner_window_info_array[_window->id].sdl_window);
}

void rde_window_set_title(rde_window* _window, const char* _title) {
	SDL_SetWindowTitle(inner_window_info_array[_window->id].sdl_window, _title);
}

//
//RDE_FUNC_ND bool			rde_window_is_fullscreen(rde_window* _window);
//RDE_FUNC 	void			rde_window_set_fullscreen(rde_window* _window, bool _fullscreen);
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
void rde_window_destroy_window(rde_window* _window) {
	rde_inner_window_info* _info = &inner_window_info_array[_window->id];

	SDL_GL_DeleteContext(_info->sdl_gl_context);
	SDL_DestroyWindow(_info->sdl_window);

	_info->sdl_window = nullptr;
	_info->sdl_gl_context = {};
	delete _info->self_pointer;
	_info->self_pointer = nullptr;
}


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

void rde_events_window_consume_events(rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - WIN_EVENT_INIT;

	if(_event_index >= 0 && _event_index < WIN_EVENT_COUNT) {
		window_events[_event_index](_window, _event);
	} else {
		std::cout << "Window Event: " << _event->type << ", not handled" << std::endl;
	}
}

void rde_events_display_create_events() {
	display_events[RDE_EVENT_TYPE_DISPLAY_CONNECTED - DISPLAY_EVENT_INIT] 			= &display_connected;
	display_events[RDE_EVENT_TYPE_DISPLAY_DISCONNECTED - DISPLAY_EVENT_INIT] 		= &display_disconnected;
	display_events[RDE_EVENT_TYPE_DISPLAY_CHANGED_ORIENTATION - DISPLAY_EVENT_INIT] = &display_changed_orientation;
}

void rde_events_display_consume_events(rde_window* _window, rde_event* _event) {
	size_t _event_index = _event->type - DISPLAY_EVENT_INIT;

	if(_event_index >= 0 && _event_index < DISPLAY_EVENT_COUNT) {
		display_events[_event_index](_window, _event);
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


rde_texture* rde_rendering_load_texture(const char* _file_path) {
	UNUSED(_file_path);
	UNIMPLEMENTED("rde_rendering_load_texture")
	return nullptr;
}

void rde_rendering_unload_texture(rde_texture* _texture) {
	UNUSED(_texture);
	UNIMPLEMENTED("rde_rendering_unload_texture")
}

rde_texture* rde_rendering_load_atlas(const char* _file_path) {
	UNUSED(_file_path);
	UNIMPLEMENTED("rde_rendering_load_atlas")
	return nullptr;
}

void rde_rendering_unload_atlas(rde_atlas* _atlas) {
	UNUSED(_atlas);
	UNIMPLEMENTED("rde_rendering_unload_atlas")
}

rde_texture* rde_rendering_create_cpu_texture(const rde_vec_2UI _texture_size) {
	UNUSED(_texture_size);
	UNIMPLEMENTED("rde_rendering_create_cpu_texture")
	return nullptr;
}

void rde_rendering_destroy_cpu_texture(rde_cpu_texture* _cpu_texture) {
	UNUSED(_cpu_texture);
	UNIMPLEMENTED("rde_rendering_destroy_cpu_texture")
}

void rde_rendering_upload_cpu_texture_to_gpu(rde_cpu_texture* _cpu_texture) {
	UNUSED(_cpu_texture);
	UNIMPLEMENTED("rde_rendering_upload_cpu_texture_to_gpu")
}

rde_font* rde_rendering_load_font(const char* _file_path) {
	UNUSED(_file_path);
	UNIMPLEMENTED("rde_rendering_load_font")
	return nullptr;
}

void rde_rendering_unload_font(rde_font* _font) {
	UNUSED(_font);
	UNIMPLEMENTED("rde_rendering_unload_font")
}

rde_texture* rde_rendering_get_atlas_sub_texture(const char* _texture_name){
	UNUSED(_texture_name);
	UNIMPLEMENTED("rde_rendering_get_atlas_sub_texture")
	return nullptr;
}



void rde_rendering_start_drawing_2d(rde_camera* _camera) {
	UNUSED(_camera);
	UNIMPLEMENTED("rde_rendering_start_drawing_2d")
}

void rde_rendering_start_drawing_3d(rde_camera* _camera) {
	UNUSED(_camera);
	UNIMPLEMENTED("rde_rendering_start_drawing_3d")
}

void rde_rendering_draw_point_2d(const rde_vec_2F _position, const rde_color _color) {
	UNUSED(_position);
	UNUSED(_color);
	UNIMPLEMENTED("rde_rendering_draw_point_2d")
}

void rde_rendering_draw_point_3d(const rde_vec_3F _position, const rde_color _color) {
	UNUSED(_position);
	UNUSED(_color);
	UNIMPLEMENTED("rde_rendering_draw_point_3d")
}

void rde_rendering_draw_line_2d(const rde_vec_2F _init, const rde_vec_2F _end, const rde_color _color) {
	UNUSED(_init);
	UNUSED(_end);
	UNUSED(_color);
	UNIMPLEMENTED("rde_rendering_draw_line_2d")
}

void rde_rendering_draw_triangle_2d(const rde_vec_2F _vertex_a, const rde_vec_2F _vertex_b, const rde_vec_2F _vertex_c, const rde_color _color) {
	UNUSED(_vertex_a);
	UNUSED(_vertex_b);
	UNUSED(_vertex_c);
	UNUSED(_color);
	UNIMPLEMENTED("rde_rendering_draw_triangle_2d")
}

void rde_rendering_draw_rectangle_2d(const rde_vec_2F _bottom_left, const rde_vec_2F _top_right, const rde_color _color) {
	UNUSED(_bottom_left);
	UNUSED(_top_right);
	UNUSED(_color);
	UNIMPLEMENTED("rde_rendering_draw_rectangle_2d")
}

void rde_rendering_draw_circle_2d(const rde_vec_2F _position, float _radius, const rde_color _color) {
	UNUSED(_position);
	UNUSED(_radius);
	UNUSED(_color);
	UNIMPLEMENTED("rde_rendering_draw_circle_2d")
}

void rde_rendering_draw_line_3d(const rde_vec_3F _init, const rde_vec_3F _end, const rde_color _color) {
	UNUSED(_init);
	UNUSED(_end);
	UNUSED(_color);
	UNIMPLEMENTED("rde_rendering_draw_line_3d")
}

void rde_rendering_end_drawing_2d() {
	UNIMPLEMENTED("rde_rendering_end_drawing_2d")
}

void rde_rendering_end_drawing_3d() {
	UNIMPLEMENTED("rde_rendering_end_drawing_3d")
}

/// ============================ AUDIO ======================================

#ifdef RDE_AUDIO_MODULE

#endif

/// ============================ PHYSICS ====================================

#ifdef RDE_PHYSICS_MODULE

#endif

/// ============================ FILE SYSTEM ================================