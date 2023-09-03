#include <stdio.h>
#include <stdarg.h>

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

#ifdef RDE_AUDIO_MODULE
#include "SDL2/SDL_mixer.h"
#endif

#define WIN_EVENT_INIT (RDE_EVENT_TYPE_WINDOW_BEGIN + 1)
#define WIN_EVENT_COUNT (RDE_EVENT_TYPE_WINDOW_END - RDE_EVENT_TYPE_WINDOW_BEGIN)

#define DISPLAY_EVENT_INIT (RDE_EVENT_TYPE_DISPLAY_BEGIN + 1)
#define DISPLAY_EVENT_COUNT (RDE_EVENT_TYPE_DISPLAY_END - RDE_EVENT_TYPE_DISPLAY_BEGIN)

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

struct rde_window {
	SDL_Window* sdl_window = nullptr;
	SDL_GLContext sdl_gl_context;
};

struct rde_shader {
	GLuint vertex_program_id = 0;
	GLuint fragment_program_id = 0;
	int compiled_program_id = -1;

	GLuint vertex_buffer_object = 0;
	GLuint index_buffer_object = 0;
	GLuint vertex_array_object = 0;
};

struct rde_texture {
	int opengl_texture_id = -1;
	rde_vec_2UI size = { 0, 0 };
	int channels = 0;
	GLenum internal_format = 0;
	GLenum data_format = 0;
	const char* file_path = nullptr;
};

struct rde_cpu_texture {
	UNIMPLEMENTED_STRUCT()
};

struct rde_render_texture {
	UNIMPLEMENTED_STRUCT()
};

struct rde_atlas {
	UNIMPLEMENTED_STRUCT()
};

struct rde_batch_2d {
	rde_shader* shader = nullptr;
	rde_vertex_2d vertices[RDE_MAX_VERTICES_PER_BATCH];
	size_t amount_of_vertices = 0;
	int texture_id = -1;
};

struct rde_batch_3d {
	UNIMPLEMENTED_STRUCT()
};


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
	rde_end_user_mandatory_callbacks mandatory_callbacks;

	rde_shader* color_shader_2d = nullptr;
	rde_shader* texture_shader_2d = nullptr;
	rde_shader* text_shader_2d = nullptr;
	rde_shader* frame_buffer_shader = nullptr;

	rde_shader shaders[RDE_MAX_LOADABLE_SHADERS];
	rde_window windows[RDE_MAX_NUMBER_OF_WINDOWS];
	rde_texture textures[RDE_MAX_LOADABLE_TEXTURES];

	rde_event_func_outer window_events[WIN_EVENT_COUNT];
	rde_event_func_outer display_events[DISPLAY_EVENT_COUNT];
};

rde_engine ENGINE;

#include "events.cpp"
#include "window.cpp"
#include "rendering.cpp"
#include "physics.cpp"
#include "audio.cpp"
#include "file_system.cpp"

void rde_engine_on_event();
void rde_engine_on_update(float _dt);
void rde_engine_on_fixed_update(float _fixed_dt);
void rde_engine_on_late_update(float _dt);
void rde_engine_on_render(float _dt);
void rde_engine_sync_events();

void rde_engine_on_event() {

	SDL_Event _event;
        
	SDL_PumpEvents();

	while (SDL_PollEvent(&_event)) {

		rde_event _rde_event = rde_engine_sdl_event_to_rde_event(&_event);
			
		for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
			rde_window* _window = &ENGINE.windows[_i];

			if(_window->sdl_window == nullptr) {
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
					if(SDL_GetWindowID(_window->sdl_window) != _rde_event.window_id) {
						continue;
					}
					rde_events_window_consume_events(_window, &_rde_event);
				} break;
				case SDL_DISPLAYEVENT: 	rde_events_display_consume_events(_window, &_rde_event); break;
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

void rde_engine_on_render(float _dt, rde_window* _window) {
	UNUSED(_dt)
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	glViewport(0, 0, _window_size.x, _window_size.y);
}

void rde_engine_sync_events() {
	
}

rde_display_info* rde_engine_get_available_displays() {
	UNIMPLEMENTED("Not implemented");
	return nullptr;
}

// ======================= API ===========================

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
	ENGINE.mandatory_callbacks.on_update = _end_user_callbacks.on_update;
	ENGINE.mandatory_callbacks.on_fixed_update = _end_user_callbacks.on_fixed_update;
	ENGINE.mandatory_callbacks.on_late_update = _end_user_callbacks.on_late_update;
	ENGINE.mandatory_callbacks.on_render = _end_user_callbacks.on_render;
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

	assert(ENGINE.mandatory_callbacks.on_update != nullptr && "User-End callback 'rde_engine_user_on_update' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");
	assert(ENGINE.mandatory_callbacks.on_fixed_update != nullptr && "User-End callback 'rde_engine_user_on_fixed_update' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");
	assert(ENGINE.mandatory_callbacks.on_late_update != nullptr && "User-End callback 'rde_engine_user_on_late_upadte' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");
	assert(ENGINE.mandatory_callbacks.on_render != nullptr && "User-End callback 'rde_engine_user_on_render' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");

	#if IS_MOBILE()
	SDL_SetEventFilter(rde_mobile_consume_events);
	#endif

	while(ENGINE.running) {
		Uint64 _start = SDL_GetPerformanceCounter();
		ENGINE.fixed_time_step_accumulator += ENGINE.delta_time;

		rde_engine_on_event();
		if (!ENGINE.running) return;

		rde_engine_on_update(ENGINE.delta_time);
		ENGINE.mandatory_callbacks.on_update(ENGINE.delta_time);

		while (ENGINE.fixed_time_step_accumulator >= ENGINE.fixed_delta_time) {
			ENGINE.fixed_time_step_accumulator -= ENGINE.fixed_delta_time;
			rde_engine_on_fixed_update(ENGINE.fixed_delta_time);
			ENGINE.mandatory_callbacks.on_fixed_update(ENGINE.fixed_delta_time);
		}

		rde_engine_on_late_update(ENGINE.delta_time);
		ENGINE.mandatory_callbacks.on_late_update(ENGINE.delta_time);

		for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
			rde_window* _window = &ENGINE.windows[_i];

			if(_window->sdl_window == nullptr) {
				continue;
			}

			rde_engine_on_render(ENGINE.delta_time, _window);
			ENGINE.mandatory_callbacks.on_render(ENGINE.delta_time, _window);

			SDL_GL_SwapWindow(_window->sdl_window);
		}

		rde_engine_sync_events();

		Uint64 _end = SDL_GetPerformanceCounter();
		float _elapsedMS = (float)(_end - _start) / (float)SDL_GetPerformanceFrequency();
		ENGINE.delta_time = _elapsedMS;
	}

	rde_engine_destroy_engine();
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
		if(ENGINE.windows[_i].sdl_window == nullptr) {
			continue;
		}

		rde_window_destroy_window(&ENGINE.windows[_i]);
	}

	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
}

void rde_engine_switch_window_display(rde_window* _window, size_t _new_display) {
	UNUSED(_window)
	UNUSED(_new_display)
	UNIMPLEMENTED("Not implemented");
}

rde_window* rde_engine_get_focused_window() {
	SDL_Window* _window = SDL_GetMouseFocus();

	for(size_t _i = 0; _i < RDE_MAX_NUMBER_OF_WINDOWS; _i++) {
		rde_window* _w = &ENGINE.windows[_i];
		
		if(_w->sdl_window == _window) {
			return _w;
		}
	}

	return nullptr;
}
