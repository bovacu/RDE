#include <stdio.h>
#include <stdarg.h>
#include <time.h>

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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-function"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PNM
#define STBI_NO_TGA
#define STBI_NO_PNM
#define STBI_NO_HDR
#define STBI_MAX_DIMENSIONS (1 << 13)
#include "stb/stb_image.h"
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#if IS_WINDOWS()
#define STBIW_WINDOWS_UTF8
#endif
#include "stb/stb_image_write.h"
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#define STB_DS_IMPLEMENTATION
#define STBDS_NO_SHORT_NAMES
#define STBDS_SIPHASH_2_4
#include "stb/stb_ds.h"
#pragma clang diagnostic pop

//#ifdef RDE_AUDIO_MODULE
//#define MINIAUDIO_IMPLEMENTATION
//#include "miniaudio/miniaudio.h"
//#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "json/cJSON.c"
#pragma clang diagnostic pop

#define RDE_WIN_EVENT_INIT (RDE_EVENT_TYPE_WINDOW_BEGIN + 1)
#define RDE_WIN_EVENT_COUNT (RDE_EVENT_TYPE_WINDOW_END - RDE_EVENT_TYPE_WINDOW_BEGIN)

#define RDE_DISPLAY_EVENT_INIT (RDE_EVENT_TYPE_DISPLAY_BEGIN + 1)
#define RDE_DISPLAY_EVENT_COUNT (RDE_EVENT_TYPE_DISPLAY_END - RDE_EVENT_TYPE_DISPLAY_BEGIN)

#define RDE_KEY_EVENT_INIT (RDE_EVENT_TYPE_KEY_BEGIN + 1)
#define RDE_KEY_EVENT_COUNT (RDE_EVENT_TYPE_KEY_END - RDE_EVENT_TYPE_KEY_BEGIN)

#define RDE_MOUSE_EVENT_INIT (RDE_EVENT_TYPE_MOUSE_BEGIN + 1)
#define RDE_MOUSE_EVENT_COUNT (RDE_EVENT_TYPE_MOUSE_END - RDE_EVENT_TYPE_MOUSE_BEGIN)

#define RDE_AMOUNT_OF_KEYS 256
#define RDE_AMOUNT_OF_MOUSE_BUTTONS 16

size_t current_frame = 0;

// TODO: Not to forget
// 		- [DONE] Set stbi_convert_iphone_png_to_rgb(1) and stbi_set_unpremultiply_on_load(1) for iOS, as 
//		  the format is BGRA instead of RGBA (problem solved by first method) and the second fixes
//		  an error that the first method can generate on some images.
//
//		- 2D rendering:
//			- Camera system
//			- [DONE] Texture rendering
//			- Debug and non debug geometry rendering
//			- [DONE] Spritebatch
//			- [DONE] Text
//			- CPU Textures
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
//			- Multiple window rendering is not working properly
//
//		- TOOL: [DONE] command line atlas packing tool for textures.
//				- https://dl.gi.de/server/api/core/bitstreams/f63b9b2f-8c00-4324-b758-22b7d36cb49e/content
//				- https://www.david-colson.com/2020/03/10/exploring-rect-packing.html
//
//		- TOOL: [DONE] Command line font atlas creator.
//				- [DONE] Improve algorithm to use most part of space
//
//		- TOOL: command line project creation, compilation and export.

struct rde_window {
	SDL_Window* sdl_window;
	SDL_GLContext sdl_gl_context;
	RDE_INPUT_STATUS_ key_states[RDE_AMOUNT_OF_KEYS];
	RDE_INPUT_STATUS_ mouse_states[RDE_AMOUNT_OF_MOUSE_BUTTONS];
};
rde_window rde_struct_create_window() {
	rde_window _w;
	_w.sdl_window = NULL;
	memset(_w.key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_w.mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
	return _w;
}

struct rde_shader {
	GLuint vertex_program_id;
	GLuint fragment_program_id;
	int compiled_program_id;
	GLuint vertex_buffer_object;
	GLuint index_buffer_object;
	GLuint vertex_array_object;
};
rde_shader rde_struct_create_shader() {
	rde_shader _s;
	_s.vertex_program_id = 0;
	_s.fragment_program_id = 0;
	_s.compiled_program_id = -1;
	_s.vertex_buffer_object = 0;
	_s.vertex_array_object = 0;
	_s.index_buffer_object = 0;
	return _s;
}

struct rde_texture {
	int opengl_texture_id;
	rde_vec_2UI size;
	rde_vec_2UI position; // This is only used for atlases
	int channels;
	GLenum internal_format;
	GLenum data_format;
	const char* file_path;
	rde_texture* atlas_texture;
	unsigned char* pixels;
	bool pixels_changed;
};
rde_texture rde_struct_create_texture() {
	rde_texture _t;
	_t.opengl_texture_id = -1;
	_t.size.x = 0;
	_t.size.y = 0;
	_t.position.x = 0;
	_t.position.y = 0;
	_t.channels = 0;
	_t.internal_format = 0;
	_t.data_format = 0;
	_t.file_path = NULL;
	_t.atlas_texture = NULL;
	_t.pixels = NULL;
	_t.pixels_changed = false;
	return _t;
}

struct rde_render_texture {
	UNIMPLEMENTED_STRUCT()
};

typedef struct {
	rde_vec_2I position;
	rde_vec_2I size;
} rde_atlas_sub_texture_data;

typedef struct {
	char* key;
	rde_texture value;
} rde_atlas_sub_textures;

struct rde_atlas {
	rde_texture* texture;
	rde_atlas_sub_textures* sub_textures;
};
rde_atlas rde_struct_create_atlas() {
	rde_atlas _a;
	_a.texture = NULL;
	_a.sub_textures = NULL;
	return _a;
}

typedef struct {
	rde_vec_2I advance;
	rde_vec_2I bearing;
	rde_vec_2I size;
	rde_vec_2I offset;
	rde_vec_2I metrics;
	rde_texture texture;
} rde_font_char_info;
rde_font_char_info rde_struct_create_font_char_info() {
	rde_font_char_info _f;
	_f.advance = (rde_vec_2I) { 0, 0 };
	_f.bearing = (rde_vec_2I) { 0, 0 };
	_f.size = (rde_vec_2I) { 0, 0 };
	_f.offset = (rde_vec_2I) { 0.0f, 0.0f };
	_f.texture = rde_struct_create_texture();
	return _f;
}

struct rde_font {
	rde_texture* texture;
	rde_font_char_info* chars;
};
rde_font rde_struct_create_font() {
	rde_font _f;
	_f.texture = NULL;
	_f.chars = NULL;
	return _f;
}

typedef struct {
	rde_shader* shader;
	rde_window* window;
	rde_texture texture;
	rde_vertex_2d vertices[RDE_MAX_VERTICES_PER_BATCH];
	size_t amount_of_vertices;
} rde_batch_2d;
rde_batch_2d rde_struct_create_2d_batch() {
	rde_batch_2d _b;
	_b.shader = NULL;
	_b.window = NULL;
	_b.texture = rde_struct_create_texture();
	memset(_b.vertices, 0, RDE_MAX_VERTICES_PER_BATCH);
	_b.amount_of_vertices = 0;
	return _b;
}

struct rde_batch_3d {
	UNIMPLEMENTED_STRUCT()
};

struct rde_engine {
	float delta_time;
	float fixed_delta_time;
	float fixed_time_step_accumulator;
	RDE_PLATFORM_TYPE_ platform_type;
	bool running;
	bool use_rde_2d_physics_system;
	rde_display_callbacks display_callbacks;
	rde_window_callbacks window_callbacks;
	rde_end_user_mandatory_callbacks mandatory_callbacks;
	rde_shader* color_shader_2d;
	rde_shader* texture_shader_2d;
	rde_shader* text_shader_2d;
	rde_shader* frame_buffer_shader;
	rde_shader* shaders;
	rde_window* windows;
	rde_texture* textures;
	rde_atlas* atlases;
	rde_font* fonts;
	rde_event_func_outer window_events[RDE_WIN_EVENT_COUNT];
	rde_event_func_outer display_events[RDE_DISPLAY_EVENT_COUNT];
	rde_event_func_outer key_events[RDE_KEY_EVENT_COUNT];
	rde_event_func_outer mouse_events[RDE_MOUSE_EVENT_COUNT];

	rde_engine_heap_allocs_config heap_allocs_config;

	#if IS_WINDOWS()
	HANDLE console_handle;
	#endif
};
rde_engine rde_struct_create_engine(rde_engine_heap_allocs_config _heap_allocs_config) {
	rde_engine _e;

	_e.heap_allocs_config = _heap_allocs_config;

	_e.delta_time = 0.f;
	_e.fixed_delta_time = 1.f / 60.f;
	_e.fixed_time_step_accumulator = 0.f;
	_e.platform_type = RDE_PLATFORM_TYPE_UNSUPPORTED;
	_e.running = true;
	_e.use_rde_2d_physics_system = true;
	_e.display_callbacks = rde_struct_create_display_callbacks();
	_e.window_callbacks = rde_struct_create_window_callbacks();
	_e.mandatory_callbacks = rde_struct_create_end_user_mandatory_callbacks();
	_e.color_shader_2d = NULL;
	_e.texture_shader_2d = NULL;
	_e.text_shader_2d = NULL;
	_e.frame_buffer_shader = NULL;

	assert(_e.heap_allocs_config.max_number_of_shaders > 0 && "Heap Allocs Congig -> number of shaders cannot be <= 0");
	_e.shaders = (rde_shader*)malloc(sizeof(rde_shader) * _e.heap_allocs_config.max_number_of_shaders);
	for(size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_shaders; _i++) {
		_e.shaders[_i] = rde_struct_create_shader();
	}

	assert(_e.heap_allocs_config.max_number_of_windows > 0 && "Heap Allocs Congig -> number of windows cannot be <= 0");
	_e.windows = (rde_window*)malloc(sizeof(rde_window) * _e.heap_allocs_config.max_number_of_windows);
	for(size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_windows; _i++) {
		_e.windows[_i] = rde_struct_create_window();
	}

	if (_e.heap_allocs_config.max_number_of_atlases > 0) {
		_e.textures = (rde_texture*)malloc(sizeof(rde_texture) * _e.heap_allocs_config.max_number_of_textures);
		for (size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_textures; _i++) {
			_e.textures[_i] = rde_struct_create_texture();
		}
	} else {
		_e.textures = NULL;
	}

	if(_e.heap_allocs_config.max_number_of_atlases > 0) {
		_e.atlases = (rde_atlas*)malloc(sizeof(rde_atlas) * _e.heap_allocs_config.max_number_of_atlases);
		for(size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_atlases; _i++) {
			_e.atlases[_i] = rde_struct_create_atlas();
		}
	} else {
		_e.atlases = NULL;
	}

	if (_e.heap_allocs_config.max_number_of_fonts > 0) {
		_e.fonts = (rde_font*)malloc(sizeof(rde_font) * _e.heap_allocs_config.max_number_of_fonts);
		for (size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_fonts; _i++) {
			_e.fonts[_i] = rde_struct_create_font();
		}
	} else {
		_e.fonts = NULL;
	}


	memset(_e.window_events, 0, RDE_WIN_EVENT_COUNT);
	memset(_e.display_events, 0, RDE_DISPLAY_EVENT_COUNT);
	memset(_e.key_events, 0, RDE_KEY_EVENT_COUNT);
	memset(_e.mouse_events, 0, RDE_MOUSE_EVENT_COUNT);

	#if IS_WINDOWS()
	_e.console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	#endif

	return _e;
}

rde_engine ENGINE;

#include "math.c"
#include "util.c"
#include "file_system.c"
#include "events.c"
#include "window.c"
#include "rendering.c"
#include "physics.c"
#include "audio.c"

void rde_engine_on_event();
void rde_engine_on_update(float _dt);
void rde_engine_on_fixed_update(float _fixed_dt);
void rde_engine_on_late_update(float _dt);
void rde_engine_on_render(float _dt, rde_window* _window);
void rde_engine_sync_events();

void rde_engine_on_event() {

	SDL_Event _event;
        
	SDL_PumpEvents();

	while (SDL_PollEvent(&_event)) {

		rde_event _rde_event = rde_engine_sdl_event_to_rde_event(&_event);
			
		for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_windows; _i++) {
			rde_window* _window = &ENGINE.windows[_i];

			if(_window->sdl_window == NULL) {
				continue;
			}

			// TODO: Use the if statement on the following event types:
			//			- SDL_TextEditingEvent
			//			- SDL_TextInputEvent
			//			- SDL_MouseMotionEvent
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
				
				case SDL_DISPLAYEVENT: rde_events_display_consume_events(_window, &_rde_event); break;
				
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					if(SDL_GetWindowID(_window->sdl_window) != _rde_event.window_id) {
						continue;
					}
					rde_events_keyboard_consume_events(_window, &_rde_event);
				} break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP: {
					if(SDL_GetWindowID(_window->sdl_window) != _rde_event.window_id) {
						continue;
					}
					rde_events_mouse_consume_events(_window, &_rde_event);
				} break;
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
	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	glViewport(0, 0, _window_size.x, _window_size.y);
}

rde_display_info* rde_engine_get_available_displays() {
	UNIMPLEMENTED("Not implemented");
	return NULL;
}

// ======================= API ===========================

rde_window* rde_engine_create_engine(int _argc, char** _argv, rde_engine_heap_allocs_config _heap_allocs_config) {
	static bool _instantiated = false;
	
	UNUSED(_argc)
	UNUSED(_argv)
	
	if(_instantiated) {
		assert(false && "[ERROR]: Only one engine can be created");
		exit(-1);
	}

	ENGINE = rde_struct_create_engine(_heap_allocs_config);

	stbds_rand_seed(time(NULL));

	rde_window* _default_window = rde_window_create_window();

	rde_events_window_create_events();
	rde_events_display_create_events();
	rde_events_key_create_events();
	rde_events_mouse_button_create_events();
	rde_rendering_set_rendering_configuration();

	srand(time(NULL));

	_instantiated = true;

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

	assert(ENGINE.mandatory_callbacks.on_update != NULL && "User-End callback 'rde_engine_user_on_update' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");
	assert(ENGINE.mandatory_callbacks.on_fixed_update != NULL && "User-End callback 'rde_engine_user_on_fixed_update' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");
	assert(ENGINE.mandatory_callbacks.on_late_update != NULL && "User-End callback 'rde_engine_user_on_late_upadte' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");
	assert(ENGINE.mandatory_callbacks.on_render != NULL && "User-End callback 'rde_engine_user_on_render' is not defined, before creating the engine call 'rde_setup_initial_info(...)'");

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

		for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_windows; _i++) {
			rde_window* _window = &ENGINE.windows[_i];

			if(_window->sdl_window == NULL) {
				continue;
			}

			rde_engine_on_render(ENGINE.delta_time, _window);
			ENGINE.mandatory_callbacks.on_render(ENGINE.delta_time, _window);

			rde_events_sync_events(_window);

			SDL_GL_SwapWindow(_window->sdl_window);
		}

		Uint64 _end = SDL_GetPerformanceCounter();
		float _elapsedMS = (float)(_end - _start) / (float)SDL_GetPerformanceFrequency();
		ENGINE.delta_time = _elapsedMS;

		current_frame++;
		if(current_frame + 1 > RDE_INT_MAX) {
			current_frame = 0;
		}
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
	return (rde_vec_2I){ _displayMode.w, _displayMode.h };
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

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_atlases; _i++) {
		if(ENGINE.atlases[_i].texture == NULL) {
			continue;
		}

		rde_rendering_unload_atlas(&ENGINE.atlases[_i]);
	}
	free(ENGINE.atlases);

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_fonts; _i++) {
		if(ENGINE.fonts[_i].texture == NULL) {
			continue;
		}

		rde_rendering_unload_font(&ENGINE.fonts[_i]);
	}
	free(ENGINE.fonts);

	for (size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_textures; _i++) {
		if (ENGINE.textures[_i].opengl_texture_id != -1) {
			continue;
		}

		if(ENGINE.textures[_i].pixels != NULL) {
			rde_rendering_destroy_memory_texture(&ENGINE.textures[_i]);
		} else {
			rde_rendering_unload_texture(&ENGINE.textures[_i]);
		}
	}
	free(ENGINE.textures);

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_shaders; _i++) {
		if(ENGINE.shaders[_i].compiled_program_id == -1) {
			continue;
		}

		rde_rendering_unload_shader(&ENGINE.shaders[_i]);
	}
	free(ENGINE.shaders);

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_windows; _i++) {
		if(ENGINE.windows[_i].sdl_window == NULL) {
			continue;
		}

		rde_window_destroy_window(&ENGINE.windows[_i]);
	}
	free(ENGINE.windows);

	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
	rde_log_level(RDE_LOG_LEVEL_INFO, "Exited cleanly");
}

void rde_engine_switch_window_display(rde_window* _window, size_t _new_display) {
	UNUSED(_window)
	UNUSED(_new_display)
	UNIMPLEMENTED("Not implemented");
}

rde_window* rde_engine_get_focused_window() {
	SDL_Window* _window = SDL_GetMouseFocus();

	for(size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_windows; _i++) {
		rde_window* _w = &ENGINE.windows[_i];
		
		if(_w->sdl_window == _window) {
			return _w;
		}
	}

	return NULL;
}
