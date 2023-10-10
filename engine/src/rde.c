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

#include "errors.c"

#include <math.h>
#include "cglm/call.h"

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

#ifdef RDE_AUDIO_MODULE
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#endif

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

#define RDE_MAX_PATH 2048

size_t current_frame = 0;

bool rde_util_check_opengl_error(const char* _message) {
	GLenum _err;
	while((_err = glGetError()) != GL_NO_ERROR){
		switch(_err) {
			case GL_NO_ERROR:
				printf("GL_NO_ERROR: No error has been recorded. The value of this symbolic constant is guaranteed to be 0. | %s -> %u \n",  _message, _err);
				return true;
			case GL_INVALID_ENUM:
				printf("GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag. | %s -> %u \n",  _message, _err);
				return true;
			case GL_INVALID_VALUE:
				printf("GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag. | %s -> %u \n",  _message, _err);
				return true;
			case GL_INVALID_OPERATION:
				printf("GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag. | %s -> %u \n",  _message, _err);
				return true;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				printf("GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete. | %s -> %u \n",  _message, _err);
				return true;
			case GL_OUT_OF_MEMORY:
				printf("GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. | %s -> %u \n",  _message, _err);
				return true;
			case GL_STACK_UNDERFLOW:
				printf("GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow. | %s -> %u \n",  _message, _err);
				return true;
			case GL_STACK_OVERFLOW:
				printf("GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow. | %s -> %u \n",  _message, _err);
				return true;
			default:
				printf("No description. | %s -> %u \n",  _message, _err);
				return true;
		}
	}

	return false;
}

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
//			- Instancing (3d batching)
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
	rde_vec_2I mouse_position;
	rde_vec_2F mouse_scroll;
};
rde_window rde_struct_create_window() {
	rde_window _w;
	_w.sdl_window = NULL;
	_w.mouse_position = (rde_vec_2I) { 0, 0 };
	_w.mouse_scroll = (rde_vec_2F) { 0.0f, 0.0f };
	memset(_w.key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_w.mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);
	return _w;
}

struct rde_shader {
	GLuint vertex_program_id;
	GLuint fragment_program_id;
	int compiled_program_id;
};
rde_shader rde_struct_create_shader() {
	rde_shader _s;
	_s.vertex_program_id = 0;
	_s.fragment_program_id = 0;
	_s.compiled_program_id = -1;
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
void rde_struct_init_alloc_ptr_texture(rde_texture* _t) {
	_t->opengl_texture_id = -1;
	_t->size.x = 0;
	_t->size.y = 0;
	_t->position.x = 0;
	_t->position.y = 0;
	_t->channels = 0;
	_t->internal_format = 0;
	_t->data_format = 0;
	_t->file_path = NULL;
	_t->atlas_texture = NULL;
	_t->pixels = NULL;
	_t->pixels_changed = false;
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
	rde_texture texture;
	rde_vertex_2d* vertices;
	size_t amount_of_vertices;
	GLuint vertex_buffer_object;
	GLuint index_buffer_object;
	GLuint vertex_array_object;
} rde_batch_2d;
rde_batch_2d rde_struct_create_2d_batch() {
	rde_batch_2d _b;
	_b.shader = NULL;
	_b.texture = rde_struct_create_texture();
	_b.vertices = NULL;
	_b.amount_of_vertices = 0;
	_b.vertex_buffer_object = 0;
	_b.vertex_array_object = 0;
	_b.index_buffer_object = 0;
	return _b;
}

struct rde_batch_3d {
	UNIMPLEMENTED_STRUCT()
};

struct rde_mesh {
	size_t vertex_count;
	size_t index_count;

	float* vertex_positions;
	float* vertex_normals;
	float* vertex_texture_coordinates;
	unsigned int* vertex_colors;

	rde_texture* texture;

	unsigned int* indices;

	unsigned int vao;
	unsigned int vbo[4]; // 0 -> positions, 1 -> colors, 2 -> normals, 3 -> texture coords
	unsigned int ibo;

	bool free_vertex_positions_on_end;
	bool free_vertex_colors_on_end;
	bool free_vertex_normals_on_end;
	bool free_vertex_texture_coordinates_on_end;
	bool free_vertex_texture_on_end;
	bool free_indices_on_end;
};
rde_mesh rde_struct_create_mesh(size_t _vertex_count, size_t _indices_count) {
	rde_mesh _mesh;

	glGenVertexArrays(1, &_mesh.vao);
	rde_util_check_opengl_error("ERROR: Creating mesh");

	_mesh.vertex_count = _vertex_count;
	_mesh.vertex_positions = NULL;
	_mesh.vertex_colors = NULL;
	_mesh.index_count = _indices_count;
	_mesh.vertex_normals = NULL;
	_mesh.vertex_texture_coordinates = NULL;
	_mesh.indices = NULL;
	_mesh.texture = NULL;

	_mesh.vbo[0] = RDE_UINT_MAX;
	_mesh.vbo[1] = RDE_UINT_MAX;
	_mesh.vbo[2] = RDE_UINT_MAX;
	_mesh.vbo[3] = RDE_UINT_MAX;

	_mesh.ibo = RDE_UINT_MAX;

	_mesh.free_vertex_positions_on_end = false;
	_mesh.free_vertex_colors_on_end = false;
	_mesh.free_vertex_normals_on_end = false;
	_mesh.free_vertex_texture_coordinates_on_end = false;
	_mesh.free_vertex_texture_on_end = false;
	_mesh.free_indices_on_end = false;

	return _mesh;
}

typedef struct {
	rde_texture* texture;
} rde_model_material;
rde_model_material rde_struct_create_model_material() {
	rde_model_material _m;
	_m.texture = NULL;
	return _m;
}

struct rde_model {
	rde_mesh* mesh_array;
	unsigned int mesh_array_size;

	rde_model_material* material_array;
	unsigned int material_array_size;
};
rde_model rde_struct_create_model() {
	rde_model _m;
	_m.mesh_array = NULL;
	_m.mesh_array_size = 0;
	_m.material_array = NULL;
	_m.material_array_size = 0;
	return _m;
}

#ifdef RDE_AUDIO_MODULE
struct rde_sound {
	bool used;
	
	bool playing;
	bool paused;
	bool looping;

	size_t played_frame;

	ma_decoder miniaudio_decoder;
};
rde_sound rde_struct_create_sound() {
	rde_sound _s;
	_s.used = false;
	_s.playing = false;
	_s.paused = false;
	_s.looping = false;
	_s.played_frame = 0;
	return _s;
}
#endif

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
	rde_shader* mesh_shader;
	rde_shader* shaders;

	size_t total_amount_of_textures;
	rde_window* windows;
	
#if defined(RDE_RENDERING_2D_MODULE) || defined(RDE_RENDERING_3D_MODULE)
	rde_texture* textures;
#endif

#ifdef RDE_RENDERING_2D_MODULE
	rde_atlas* atlases;
	rde_font* fonts;
#endif

#ifdef RDE_RENDERING_3D_MODULE
	rde_mesh* meshes;
	rde_model* models;
#endif

#ifdef RDE_AUDIO_MODULE
	rde_sound* sounds;
	ma_device miniaudio_device;
	rde_sound_config device_config;
#endif
	
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
	_e.total_amount_of_textures = 0;
	
#ifdef RDE_RENDERING_2D_MODULE
	_e.total_amount_of_textures += _e.heap_allocs_config.max_number_of_textures;
	_e.total_amount_of_textures += _e.heap_allocs_config.max_number_of_fonts;
	_e.total_amount_of_textures += _e.heap_allocs_config.max_number_of_atlases;
#endif

#ifdef RDE_RENDERING_3D_MODULE
	_e.total_amount_of_textures += _e.heap_allocs_config.max_number_of_models_textures;
#endif


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

	rde_critical_error(_e.heap_allocs_config.max_number_of_shaders <= 0, RDE_ERROR_HEAP_ALLOC_BAD_VALUE, "shaders", _e.heap_allocs_config.max_number_of_shaders);
	_e.shaders = (rde_shader*)malloc(sizeof(rde_shader) * _e.heap_allocs_config.max_number_of_shaders);
	rde_critical_error(_e.shaders == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_shader) * _e.heap_allocs_config.max_number_of_shaders, "shaders");
	for(size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_shaders; _i++) {
		_e.shaders[_i] = rde_struct_create_shader();
	}

	rde_critical_error(_e.heap_allocs_config.max_number_of_windows <= 0, RDE_ERROR_HEAP_ALLOC_BAD_VALUE, "windows", _e.heap_allocs_config.max_number_of_windows);
	_e.windows = (rde_window*)malloc(sizeof(rde_window) * _e.heap_allocs_config.max_number_of_windows);
	rde_critical_error(_e.windows == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_window) * _e.heap_allocs_config.max_number_of_windows, "windows");
	for(size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_windows; _i++) {
		_e.windows[_i] = rde_struct_create_window();
	}

#if defined(RDE_RENDERING_2D_MODULE) || defined(RDE_RENDERING_3D_MODULE)
	if (_e.total_amount_of_textures > 0) {
		_e.textures = (rde_texture*)malloc(sizeof(rde_texture) * _e.total_amount_of_textures);
		rde_critical_error(_e.textures == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_texture) * _e.total_amount_of_textures, "textures");
		for (size_t _i = 0; _i < _e.total_amount_of_textures; _i++) {
			_e.textures[_i] = rde_struct_create_texture();
		}
	} else {
		_e.textures = NULL;
	}
#endif

#ifdef RDE_RENDERING_2D_MODULE
	if(_e.heap_allocs_config.max_number_of_atlases > 0) {
		_e.atlases = (rde_atlas*)malloc(sizeof(rde_atlas) * _e.heap_allocs_config.max_number_of_atlases);
		rde_critical_error(_e.atlases == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_atlas) * _e.heap_allocs_config.max_number_of_atlases, "atlases");
		for(size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_atlases; _i++) {
			_e.atlases[_i] = rde_struct_create_atlas();
		}
	} else {
		_e.atlases = NULL;
	}

	if (_e.heap_allocs_config.max_number_of_fonts > 0) {
		_e.fonts = (rde_font*)malloc(sizeof(rde_font) * _e.heap_allocs_config.max_number_of_fonts);
		rde_critical_error(_e.fonts == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_font) * _e.heap_allocs_config.max_number_of_fonts, "fonts");
		for (size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_fonts; _i++) {
			_e.fonts[_i] = rde_struct_create_font();
		}
	} else {
		_e.fonts = NULL;
	}
#endif

#ifdef RDE_RENDERING_3D_MODULE
	if(_e.heap_allocs_config.max_number_of_models > 0) {
		_e.models = (rde_model*)malloc(sizeof(rde_model) * _e.heap_allocs_config.max_number_of_models);
		rde_critical_error(_e.models == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_atlas) * _e.heap_allocs_config.max_number_of_models, "atlases");
		for(size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_models; _i++) {
			_e.models[_i] = rde_struct_create_model();
		}
	} else {
		_e.models = NULL;
	}
#endif

#ifdef RDE_AUDIO_MODULE
	if (_e.heap_allocs_config.max_number_of_sounds > 0) {
		_e.sounds = (rde_sound*)malloc(sizeof(rde_sound) * _e.heap_allocs_config.max_number_of_sounds);
		rde_critical_error(_e.sounds == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_font) * _e.heap_allocs_config.max_number_of_sounds, "audio");
		for (size_t _i = 0; _i < _e.heap_allocs_config.max_number_of_sounds; _i++) {
			_e.sounds[_i] = rde_struct_create_sound();
		}
	} else {
		_e.sounds = NULL;
	}
#endif

	memset(_e.window_events, 0, RDE_WIN_EVENT_COUNT);
	memset(_e.display_events, 0, RDE_DISPLAY_EVENT_COUNT);
	memset(_e.key_events, 0, RDE_KEY_EVENT_COUNT);
	memset(_e.mouse_events, 0, RDE_MOUSE_EVENT_COUNT);

#if IS_WINDOWS()
	_e.console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	#ifdef RDE_ERROR_MODULE
	SetUnhandledExceptionFilter(rde_error_sig_handler);
	#endif
#endif

	return _e;
}

rde_engine ENGINE;

#include "math.c"
#include "util.c"
#include "file_system.c"
#include "events.c"
#include "window.c"
#include "rendering_common.c"
#include "rendering_2d.c"
#include "fbx_importer.c"
#include "obj_importer.c"
#include "rendering_3d.c"
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

				case SDL_MOUSEMOTION:
				case SDL_MOUSEWHEEL:
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
	
	rde_critical_error(_instantiated, RDE_ERROR_MULTIPLE_ENGINE);
	
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

	rde_critical_error(ENGINE.mandatory_callbacks.on_update == NULL, RDE_ERROR_NULL_MANDATORY_CALLBACK, "rde_engine_user_on_update");
	rde_critical_error(ENGINE.mandatory_callbacks.on_fixed_update == NULL, RDE_ERROR_NULL_MANDATORY_CALLBACK, "rde_engine_user_on_fixed_update");
	rde_critical_error(ENGINE.mandatory_callbacks.on_late_update == NULL, RDE_ERROR_NULL_MANDATORY_CALLBACK, "rde_engine_user_on_late_upadte");
	rde_critical_error(ENGINE.mandatory_callbacks.on_render == NULL, RDE_ERROR_NULL_MANDATORY_CALLBACK, "rde_engine_user_on_render");

	#if IS_MOBILE()
	SDL_SetEventFilter(rde_mobile_consume_events);
	#endif

#ifdef RDE_RENDERING_2D_MODULE
	rde_rendering_init_2d();
#endif

#ifdef RDE_RENDERING_3D_MODULE
	rde_rendering_init_3d();
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

#ifdef RDE_RENDERING_2D_MODULE
	rde_rendering_end_2d();
#endif

#ifdef RDE_RENDERING_3D_MODULE
	rde_rendering_end_3d();
#endif

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

#ifdef RDE_RENDERING_2D_MODULE
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
#endif

#if defined(RDE_RENDERING_2D_MODULE) || defined(RDE_RENDERING_3D_MODULE)
	for (size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if (ENGINE.textures[_i].opengl_texture_id == -1) {
			continue;
		}

		if(ENGINE.textures[_i].pixels != NULL) {
			rde_rendering_destroy_memory_texture(&ENGINE.textures[_i]);
		} else {
			rde_rendering_unload_texture(&ENGINE.textures[_i]);
		}
	}
	free(ENGINE.textures);
#endif

#if RDE_RENDERING_3D_MODULE
	for (size_t _i = 0; _i < ENGINE.heap_allocs_config.max_number_of_models; _i++) {
		if (ENGINE.models[_i].mesh_array == NULL) {
			continue;
		}

		rde_rendering_unload_model(&ENGINE.models[_i]);
	}
	free(ENGINE.models);
#endif

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

#ifdef RDE_AUDIO_MODULE
	rde_audio_end();
	free(ENGINE.sounds);
	
#endif

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
