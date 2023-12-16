#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "rde.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl32.h>
#elif IS_ANDROID()
#include <GLES3/gl32.h>
#elif IS_IOS()
#include <OpenGLES/ES3/gl.h>
#elif IS_MAC()
#include <OpenGL/gl3.h>
#else
#include "glad/glad.h"
#endif

#if IS_MOBILE()
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#endif

#if IS_ANDROID()
// This include comes directly from the RDEAndroid module SDL JNI. It contains some extra fanzy features which normal SDL doesn't.
#include "SDL.h"
#else
#include "SDL2/SDL.h"
#endif


#if IS_MAC()
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <math.h>
#include "cglm/call.h"

#define MINICORO_IMPL
#include "minicoro/minicoro.h"

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

#if !IS_ANDROID()
#define STBDS_SIPHASH_2_4
#endif

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

#include "rendering_3d_default_meshes.c"

#if defined(RDE_OBJ_MODULE)
#include "fast_obj/fast_obj.c"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#endif

#if defined(RDE_FBX_MODULE)
#include "ufbx/ufbx.c"
#endif

#ifdef RDE_ERROR_MODULE
#include <signal.h>
#if IS_WINDOWS()
#include <dbghelp.h>
void rde_inner_print_stack_trace(FILE* _f);
LONG WINAPI rde_inner_error_sig_handler(PEXCEPTION_POINTERS _sigfault_info);
#endif
#if !IS_WINDOWS() && !IS_ANDROID()
#include <execinfo.h>
#include <dlfcn.h>
#include <err.h>
void rde_inner_buf_printf(FILE* _f, const char* _fmt, ...);
void rde_inner_print_stack_trace(FILE* _f);
void rde_inner_posix_signal_handler(int _sig, siginfo_t* _sig_info, void* _context);
void rde_inner_set_posix_signal_handler();
#endif
void rde_critical_error(bool _condition, const char* _fmt, ...);
#endif

// TODO: Not to forget
// 		- [DONE] Set stbi_convert_iphone_png_to_rgb(1) and stbi_set_unpremultiply_on_load(1) for iOS, as 
//		  the format is BGRA instead of RGBA (problem solved by first method) and the second fixes
//		  an error that the first method can generate on some images.
//
//		- 2D rendering:
//			- [DONE] Camera system
//			- [DONE] Texture rendering
//			- Debug and non debug geometry rendering
//			- [DONE] Spritebatch
//			- [DONE] Text
//			- CPU Textures
//
//		- Basic 3D:
//			- [DONE] Camera system
//			- [DONE] 3D mathematical operations
//			- [DONE] Renderer
//			- [DONE] Mesh creation and loading
//			- [DONE(obj), NOT DONE(glft)] Model loading
//			- [DONE] Texturing and [NOT COMPLETELY DONE] materials
//			- [DONE] Instancing (3d batching)
//			- [DONE] MSAA (ANDROID NOT WORKING, I SUPPOSE IOS NEITHER)
//			- Lighting
//				- [DONE] Phong
//				- [DONE] Directional
//				- [DONE] Point
//				- [DONE] Spot
//				- Shadows
//			- Model animations
//			- Text
//			- [DONE] Line rendering
//
//		- Shaders:
//			- [DONE] Create "header" file that will contain all of the common uniforms and variables that are passed to every shader
//			- [DONE] Pass dt and mouse pos as a uniform
//
//		- Other:
//			- [DONE] Render Textures
//			- Particles
//			- Multiple window rendering is not working properly
//			- [DONE] On every 'load' function, if a resource is going to be reloaded, return the already loaded one.
//
//		- TOOL: [DONE] command line atlas packing tool for textures.
//				- https://dl.gi.de/server/api/core/bitstreams/f63b9b2f-8c00-4324-b758-22b7d36cb49e/content
//				- https://www.david-colson.com/2020/03/10/exploring-rect-packing.html
//				- TODO: sanitaze windows paths like in rde_build
//
//		- TOOL: [DONE] Command line font atlas creator.
//				- [DONE] Improve algorithm to use most part of space
//				- TODO: sanitaze windows paths like in rde_build
//
//		- TOOL: command line project creation, compilation and export.


/// *************************************************************************************************
/// *                                		DEFINES                         			  		 *
/// *************************************************************************************************

/// File System
#define RDE_MAX_CONCURRENT_FILES_OPENED 64
#define RDE_MAX_PATH 2048

/// Rendering
#define RDE_DEFAULT_SHADERS_AMOUNT 6
#define RDE_SHADER_MAX_NAME 256
#if RDE_DEBUG
#define RDE_CHECK_GL(_function, ...) { 																																																														\
		_function(__VA_ARGS__);																																																																\
		GLenum _err = GL_NO_ERROR;																																																															\
		while((_err = glGetError()) != GL_NO_ERROR){																																																										\
			switch(_err) {																																																																	\
				case GL_NO_ERROR:																																																															\
					printf("GL_NO_ERROR: No error has been recorded. The value of this symbolic constant is guaranteed to be 0. Erro(%u). %s, line: %d, file: %s \n", _err, #_function, __LINE__, __FILE__);																				\
					break;																																																																	\
				case GL_INVALID_ENUM:																																																														\
					printf("GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag. Erro(%u). %s, line: %d, file: %s \n", _err, #_function, __LINE__, __FILE__);			\
					break;																																																																	\
				case GL_INVALID_VALUE:																																																														\
					printf("GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag. Erro(%u). %s, line: %d, file: %s \n", _err, #_function, __LINE__, __FILE__);									\
					break;																																																																	\
				case GL_INVALID_OPERATION:																																																													\
					printf("GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag. Erro(%u). %s, line: %d, file: %s \n", _err, #_function, __LINE__, __FILE__);		\
					break;																																																																	\
				case GL_INVALID_FRAMEBUFFER_OPERATION:																																																										\
					printf("GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete. Erro(%u). %s, line: %d, file: %s \n", _err, #_function, __LINE__, __FILE__);																											\
					break;																																																																	\
				case GL_OUT_OF_MEMORY:																																																														\
					printf("GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. Erro(%u). %s, line: %d, file: %s \n", _err, #_function, __LINE__, __FILE__);	\
					break;																																																																	\
				default:																																																																	\
					printf("No description. Error(%u) \n", _err);																																																							\
					break;																																																																	\
			}																																																																				\
		}																																																																					\
	}
#else
#define RDE_CHECK_GL(_function, ...) _function(__VA_ARGS__);
#endif

#define RDE_CHECK_SHADER_COMPILATION_STATUS(_program_id, _compiled, _path)												\
	if(!_compiled) {																									\
		char _infolog[1024];																							\
		glGetShaderInfoLog(_program_id, 1024, NULL, _infolog);															\
		glDeleteShader(_program_id);																					\
		rde_critical_error(true, "Shader(%d) '%s' compile failed with error: \n%s \n", _program_id, _path, _infolog);	\
		return NULL;																									\
	}

#define RDE_IMPLEMENT_SAFE_ARR_ACCESS(_type)																							\
	_type rde_arr_s_get_##_type(uint _index, _type* _arr, size_t _arr_size, char* _fmt, ...) {										  \
		if(_index >= _arr_size) {																									   \
			va_list _args;																											  \
			va_start(_args, _fmt);																									  \
			vfprintf(stdout, _fmt, _args);																						  	\
			va_end(_args);																										  	\
			if(!rde_engine_logs_supressed()) {																					  	\
				printf("\n");																									   	\
			}																													   	\
		}																														   	\
		rde_critical_error(_index >= _arr_size, "Index accessed '%u' is greater than array size '%llu' in '%s'.\n", _index, _arr_size); \
		return _arr[_index];																											\
	}

#define RDE_IMPLEMENT_SAFE_ARR_SET(_type)																									\
	void rde_arr_s_set_##_type(uint _index, _type _value, _type* _arr, size_t _arr_size, char* _fmt, ...) {								\
		if(_index >= _arr_size) {																												\
			va_list _args;																														\
			va_start(_args, _fmt);																												\
			vfprintf(stdout, _fmt, _args);																										\
			va_end(_args);																														\
			if(!rde_engine_logs_supressed()) {																									\
				printf("\n");																													\
			}																																	\
		}																																		\
		rde_critical_error(_index >= _arr_size, "Index accessed '%u' is greater than array size '%llu''.\n", _index, _arr_size);				\
		_arr[_index] = _value;																													\
}

RDE_IMPLEMENT_SAFE_ARR_ACCESS(int)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(uint)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(size_t)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(short)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(float)
RDE_IMPLEMENT_SAFE_ARR_ACCESS(double)

RDE_IMPLEMENT_SAFE_ARR_SET(int)
RDE_IMPLEMENT_SAFE_ARR_SET(uint)
RDE_IMPLEMENT_SAFE_ARR_SET(size_t)
RDE_IMPLEMENT_SAFE_ARR_SET(short)
RDE_IMPLEMENT_SAFE_ARR_SET(float)
RDE_IMPLEMENT_SAFE_ARR_SET(double)

#ifdef RDE_OBJ_MODULE
RDE_IMPLEMENT_SAFE_ARR_ACCESS(fastObjIndex)
#endif

#define RDE_SHADOW_MAP_SIZE 1024

#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_COLOR 1
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL 3
#define RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD 2

#define RDE_DEFAULT_TEXTURE_SIZE 64
#define RDE_DEFAULT_TEXTURE_CHANNELS 3

#define RDE_MAX_MODELS_PER_DRAW 1000
#define RDE_MAX_LINES_PER_DRAW 10000

#define RDE_OBJ_EXTENSION "obj"
#define RDE_FBX_EXTENSION "fbx"
#define RDE_GLTF_EXTENSION "gltf"

/// Event
#define RDE_WIN_EVENT_INIT (RDE_EVENT_TYPE_WINDOW_BEGIN + 1)
#define RDE_WIN_EVENT_COUNT (RDE_EVENT_TYPE_WINDOW_END - RDE_EVENT_TYPE_WINDOW_BEGIN)

#define RDE_DISPLAY_EVENT_INIT (RDE_EVENT_TYPE_DISPLAY_BEGIN + 1)
#define RDE_DISPLAY_EVENT_COUNT (RDE_EVENT_TYPE_DISPLAY_END - RDE_EVENT_TYPE_DISPLAY_BEGIN)

#define RDE_KEY_EVENT_INIT (RDE_EVENT_TYPE_KEY_BEGIN + 1)
#define RDE_KEY_EVENT_COUNT (RDE_EVENT_TYPE_KEY_END - RDE_EVENT_TYPE_KEY_BEGIN)

#define RDE_MOUSE_EVENT_INIT (RDE_EVENT_TYPE_MOUSE_BEGIN + 1)
#define RDE_MOUSE_EVENT_COUNT (RDE_EVENT_TYPE_MOUSE_END - RDE_EVENT_TYPE_MOUSE_BEGIN)

#define RDE_DRAG_AND_DROP_EVENT_INIT (RDE_EVENT_TYPE_DRAG_AND_DROP_BEGIN + 1)
#define RDE_DRAG_AND_DROP_EVENT_COUNT (RDE_EVENT_TYPE_DRAG_AND_DROP_END - RDE_EVENT_TYPE_DRAG_AND_DROP_BEGIN)

#define RDE_MOBILE_EVENT_INIT (RDE_EVENT_TYPE_MOBILE_BEGIN + 1)
#define RDE_MOBILE_EVENT_COUNT (RDE_EVENT_TYPE_MOBILE_END - RDE_EVENT_TYPE_MOBILE_BEGIN)

#define RDE_AMOUNT_OF_KEYS 256
#define RDE_AMOUNT_OF_MOUSE_BUTTONS 16
#define RDE_AMOUNT_OF_MOBILE_FINGERS 10

size_t current_frame = 0;

/// Audio
#define DEFAULT_FORMAT ma_format_f32

/// Errors
#define RDE_ERROR_NO_MEMORY "Could not allocate enought memory (%d bytes) for %s.\n"
#define RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED "Max number of loaded %s (%d) reached.\n"
#define RDE_ERROR_NO_NULL_ALLOWED "Tried to use a NULL %s.\n"
#define RDE_ERROR_FILE_NOT_FOUND "File '%s' not found or could not open it.\n"
#define RDE_ERROR_JSON "Could not load JSON from file '%s' due to error '%s'.\n"
#define RDE_ERROR_HEAP_ALLOC_BAD_VALUE "Heap Allocs Config -> number of %s cannot be <= 0 and %d was provided.\n"
#define RDE_ERROR_MULTIPLE_ENGINE "Only one engine can be created.\n"
#define RDE_ERROR_NULL_MANDATORY_CALLBACK "User-End callback %s' is not defined, before creating the engine call 'rde_setup_initial_info(...)'.\n"
#define RDE_ERROR_UNSUPPORTED_PLATFORM "Unsupported or unimplemented platform.\n"
#define RDE_ERROR_ATLAS_SUB_TEXTURE "Could not load sub texture %s for atlas at %s.\n"
#define RDE_ERROR_BEGIN_RENDER "Tried to begin drawing again before ending the previous one or provided 'camera' or 'window' = NULL.\n"
#define RDE_ERROR_BAD_MESH_DATA "Error while creating mesh, _vertex_count = %d and _index_count = %d. _vertex_count must be >= 3 and _index_count >= 3.\n"
#define RDE_ERROR_SDL_WINDOW "SDL window creation failed: %s.\n"
#define RDE_ERROR_SDL_OPENGL "OpenGL context couldn't initialize: %s.\n"
#define RDE_ERROR_GLAD_INIT "Failed to initialize GLAD.\n"
#define RDE_ERROR_SANITIZATION_PATH "Path '%s' is greater than the output sanitized buffer size '%u'\n"
#define RDE_ERROR_MESSAGE_BOX "Only 'RDE_LOG_LEVEL_INFO', 'RDE_LOG_LEVEL_WARNING', 'RDE_LOG_LEVEL_ERROR' work on rde_engine_show_message_box. Defaulting to 'RDE_LOG_LEVEL_INFO'"
#define RDE_ERROR_FEATURE_NOT_SUPPORTED_YET "Feature '%s' is not supported or implemented yet."
#define RDE_ERROR_EVENT_NOT_HANDLED "%s Event: %i, not handled."
#define RDE_WARNING_MOBILE_FINGER_INPUT "Maximum number of fingers '%d' reached on mobile, ignoring this one \n."

#ifdef RDE_AUDIO_MODULE
#define RDE_ERROR_MA_CONTEXT "Failed to initialize context.\n"
#define RDE_ERROR_MA_DEVICE_INFO "Failed to retrieve device information.\n"
#define RDE_ERROR_MA_DEVICE_INIT "Could not init audio device. Error code for MiniAudio %d\n"
#define RDE_ERROR_MA_DEVICE_START "Could not start the audio device to play sounds. Error code for MiniAudio %d\n"
#define RDE_ERROR_MA_FILE_NOT_FOUND "Could not load sound '%s'. error code for MiniAudio %d.\n"
#endif

#define RDE_ERROR_RENDERING_TEXTURE_UNSUPPORTED_FORMAT "Tried to load model '%s' which has an unsupported format '%s'. Valid formats are [png,jpeg,jpg]."
#define RDE_ERROR_RENDERING_MODEL_MODULE_FORMAT_NOT_COMPILED "Tried to load model '%s' format that was not compiled as a module in RDE, to fix this error change to another format or compile RDE with module '%s'."
#define RDE_ERROR_RENDERING_MODEL_UNSUPPORTED_FORMAT "Tried to load model '%s' which has an unsupported format '%s'."
#define RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER "Framebuffer is incomplete. \n"
#define RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER_MSAA "Framebuffer for antialiasing is incomplete. \n"
#define RDE_ERROR_RENDERING_WRONG_ANTIALIASING_VALUE "Wrong antialiasing value '%d'. \n"
#define RDE_WARNING_RENDERING_ANTIALIASING_LEVEL_ALREADY_SET "Antialiasing level '%d' is already configured. \n"
#define RDE_WARNING_RENDERING_ANTIALIASING_LEVEL_NOT_SUPPORTED "Antialiasing level '%d' is not supported, trying with '%d'. \n"
#define RDE_WARNING_RENDERING_WRONG_TEXTURE_PARAM "Texture param with value '%d' is not applicable to %s. Defaulting to '%d' \n"

#ifdef RDE_FBX_MODULE
#define RDE_ERROR_FBX_COULD_NOT_LOAD "Could not load fbx file at '%s' due to error %s\n"
#define RDE_ERROR_FBX_MODEL_FACE_INDICES "Model '%s' has a face with a different amount of indices of 3 or 4, '%u'\n"
#endif

#ifdef RDE_OBJ_MODULE
#define RDE_ERROR_OBJ_COULD_NOT_LOAD "Could not load obj file at '%s', file not found\n"
#define RDE_ERROR_OBJ_MATERIAL_COULD_NOT_LOAD "Could not load obj material file at '%s', file not found\n"
#define RDE_ERROR_OBJ_INVALID_DATA "Some data in '%s' could not be parsed correctly, failed to parse %s at line %lu \n"
#define RDE_ERROR_OBJ_INVALID_MATERIAL_DATA "%s in '%s' with value %s could not be parsed correctly, failed to parse %s at line %lu \n"
#define RDE_WARNING_OBJ_VERTEX_INCLUDES_COLORS "Model '%s' includes vertex colors in 'v' and this is still not supported so it will be ignored \n"
#endif

#define RDE_ERROR_FILE_SYSTEM_MAX_OPENED_FILES_REACHED "Max number of concurrent files opened '%d' by File System Modle has been reached. Please close some files before opening more. \n"

#define RDE_MAX_STACK 100

#if !IS_WINDOWS() && !IS_ANDROID()
#define RDE_STACKTRACE_MAX_DEPTH 1024
// Same value as SIGSTKSZ
#define RDE_STACKTRACE_BUFF_SIZE 13504
#define RDE_STACKTRACE_PRINT_BUFF_SIZE 1024
#define RDE_STACKTRACE_PRINT_LINE_BUFF_SIZE 2048
#endif

/// Engine
#define RDE_MAX_TRANSFORMS 100000

/// *************************************************************************************************
/// *                                INNER STRUCT DEFINITIONS                         			  *
/// *************************************************************************************************

struct rde_transform {
	rde_vec_3F position;
	rde_vec_3F rotation;
	rde_vec_3F scale;
	int parent;
	int* children;
	int index;
	bool dirty;
	bool updated_this_frame;
};

/// File System
struct rde_file_handle {
	char file_path[RDE_MAX_PATH];
	SDL_RWops* sdl_handle;
	RDE_FILE_MODE_ file_mode;
	char* text_allocated;
	unsigned char* bytes_allocated;
};

/// Window
struct rde_window {
	SDL_Window* sdl_window;
	SDL_GLContext sdl_gl_context;
	RDE_INPUT_STATUS_ key_states[RDE_AMOUNT_OF_KEYS];
	RDE_INPUT_STATUS_ mouse_states[RDE_AMOUNT_OF_MOUSE_BUTTONS];
	RDE_INPUT_STATUS_ mobile_states[RDE_AMOUNT_OF_MOBILE_FINGERS];
	rde_vec_2I mouse_position;
	rde_vec_2F mouse_scroll;
};


/// Rendering
struct rde_shader {
	GLuint vertex_program_id;
	GLuint fragment_program_id;
	int compiled_program_id;
	char name[RDE_SHADER_MAX_NAME];
};
	
struct rde_texture {
	int opengl_texture_id;
	rde_vec_2UI size;
	rde_vec_2UI position; // This is only used for atlases
	int channels;
	GLenum internal_format;
	GLenum data_format;
	char file_path[RDE_MAX_PATH];
	rde_texture* atlas_texture;
	unsigned char* pixels;
	bool pixels_changed;
};
	
typedef struct {
	rde_vec_2I position;
	rde_vec_2I size;
} rde_atlas_sub_texture_data;
	
typedef struct {
	char* key;
	rde_texture value;
} rde_atlas_sub_textures;
	
typedef struct {
	rde_vec_2I advance;
	rde_vec_2I bearing;
	rde_vec_2I size;
	rde_vec_2I offset;
	rde_vec_2I metrics;
	rde_texture texture;
} rde_font_char_info;
	
typedef struct {
	rde_vec_3F position;
	int color;
	rde_vec_2F texture_coordinates;
} rde_vertex_2d;
	
typedef struct {
	rde_shader* shader;
	rde_texture texture;
	rde_vertex_2d* vertices;
	size_t amount_of_vertices;
	GLuint vertex_buffer_object;
	GLuint index_buffer_object;
	GLuint vertex_array_object;
	mat4 mvp;
	bool is_hud;
} rde_batch_2d;
	
typedef struct {
	rde_vec_3F position;
	uint color;
} rde_line_vertex;
rde_line_vertex rde_struct_create_line_vertex();
	
typedef struct {
	rde_line_vertex* vertices;
	size_t amount_of_vertices;
	GLuint vertex_buffer_object;
	GLuint vertex_array_object;
	unsigned short thickness;
	rde_shader* shader;
} rde_line_batch;
	
typedef struct {
	rde_mesh* mesh;
	rde_shader* shader;
	size_t amount_of_models_per_draw;
	rde_line_batch line_batch;
	bool draw_mesh_wireframe;
} rde_batch_3d;
	
struct rde_atlas {
	rde_texture* texture;
	rde_atlas_sub_textures* sub_textures;
};
	
struct rde_font {
	rde_texture* texture;
	rde_font_char_info* chars;
};
	
#define RDE_MESH_NAME_MAX 512
struct rde_mesh {
	char name[RDE_MESH_NAME_MAX];
	size_t vertex_count;
	
	float* vertex_positions;
	float* vertex_normals;
	float* vertex_texcoords;
	mat4* transformation_matrices;
	
	uint vao;
	uint vbo[4]; // 0 -> positions (static), 
				 // 1 -> normals (static), 
				 // 2 -> texture coords (static)
				 // 3 -> transforms to render (dynamic)
	rde_material material;
};
	
struct rde_model {
	rde_mesh* mesh_array;
	uint mesh_array_size;
	char file_path[RDE_MAX_PATH];
};
	
typedef struct {
	uint vao;
	uint vbo;
	int opengl_texture_id;
} rde_skybox;
	
struct rde_render_texture {
	uint opengl_framebuffer_id;
	uint opengl_renderbuffer_id;
	uint opengl_texture_id;

	rde_vec_2UI size;

	uint vao;
	uint vbo;
};

typedef struct {
	rde_directional_light directional_light;
	rde_point_light** point_lights;
	rde_spot_light** spot_lights;
} rde_illumination;

typedef struct {
	uint frame_buffer_id;
	uint render_buffer_id;
	int opengl_texture_id; 
	int samples;
} rde_antialiasing;

typedef struct {
	uint frame_buffer_id;
	int opengl_texture_id;
} rde_shadows;

typedef struct {
	size_t number_of_drawcalls;
} rde_rendering_statistics;

#ifdef RDE_OBJ_MODULE
typedef struct {
	char* name;
	size_t vertex_count;

	float* positions;
	size_t positions_size;
	size_t positions_pointer;

	float* texcoords;
	size_t texcoords_size;
	size_t texcoords_pointer;

	float* normals;
	size_t normals_size;
	size_t normals_pointer;

	fastObjMaterial* material;
	rde_texture* map_ka;
	rde_texture* map_kd;
	rde_texture* map_ks;
	rde_texture* map_bump;
} rde_obj_mesh_data;

typedef struct {
	rde_obj_mesh_data value;
	int key;
} rde_obj_map_entry;
#endif

/// Audio
#ifdef RDE_AUDIO_MODULE
struct rde_sound {
	bool used;
		
	bool playing;
	bool paused;
	bool looping;
	
	size_t played_frame;
	
	ma_decoder miniaudio_decoder;
};
#endif

/// Error
#if defined(RDE_ERROR_MODULE) && !IS_WINDOWS() && !IS_ANDROID()
typedef struct {
	    char* buf;
	    int pos;
	    int size;
	} rde_print_buf;

	typedef struct rde_posix_stacktrace {
	    void* trace[RDE_STACKTRACE_MAX_DEPTH];
	    int trace_size;
	} rde_posix_stacktrace;
#endif

/// Engine
struct rde_engine {
	float delta_time;
	float fixed_delta_time;
	float fixed_time_step_accumulator;
		
	RDE_PLATFORM_TYPE_ platform_type;
		
	bool running;
	bool pause_loop;
	bool use_rde_2d_physics_system;
	bool supress_engine_logs;
		
	rde_end_user_mandatory_callbacks mandatory_callbacks;
	rde_event_func user_event_callback;
		
	rde_window* windows;
	rde_transform* transforms;
	mat4* world_transforms;
	int last_transform_used;
	int* free_transforms;
		
#if IS_ANDROID()
	ANativeWindow* android_native_window;
	JNIEnv* android_env;
#endif

#define RDE_SHADERS_AMOUNT 7
rde_shader* line_shader;
rde_shader* color_shader_2d;
rde_shader* texture_shader_2d;
rde_shader* text_shader_2d;
rde_shader* framebuffer_shader;
rde_shader* mesh_shader;
rde_shader* skybox_shader;
rde_shader* shaders;
	
size_t total_amount_of_textures;
	
rde_texture* textures;
rde_atlas* atlases;
rde_font* fonts;
rde_mesh* meshes;
rde_model* models;
	
rde_illumination illumination;
rde_skybox skybox;
rde_antialiasing antialiasing;
rde_shadows shadows;
	
#ifdef RDE_AUDIO_MODULE
	rde_sound* sounds;
	ma_device miniaudio_device;
	rde_sound_config device_config;
#endif
		
rde_event_func window_events[RDE_WIN_EVENT_COUNT];
rde_event_func display_events[RDE_DISPLAY_EVENT_COUNT];
rde_event_func key_events[RDE_KEY_EVENT_COUNT];
rde_event_func mouse_events[RDE_MOUSE_EVENT_COUNT];
rde_event_func drag_and_drop_events[RDE_DRAG_AND_DROP_EVENT_COUNT];
rde_event_func mobile_events[RDE_MOBILE_EVENT_COUNT];
	
rde_engine_init_info init_info;
	
#if IS_WINDOWS()
	HANDLE console_handle;
#endif
};


/// *************************************************************************************************
/// *                                INNER STRUCT CONSTRUCTORS                         			 *
/// *************************************************************************************************

rde_transform rde_struct_create_transform() {
	rde_transform _t;
	_t.position = (rde_vec_3F) { 0.0f, 0.0f, 0.0f };
	_t.rotation = (rde_vec_3F) { 0.0f, 0.0f, 0.0f };
	_t.scale = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
	_t.parent = RDE_INT_MIN;
	_t.dirty = false;
	_t.updated_this_frame = false;
	_t.children = NULL;
	// _t.index = RDE_INT_MIN;
	return _t;
}

rde_probability rde_struct_create_probability() {
	rde_probability _p;
	_p.probability_rolled = 0.f;
	_p.happened = false;
	return _p;
}

rde_end_user_mandatory_callbacks rde_struct_create_end_user_mandatory_callbacks() {
	rde_end_user_mandatory_callbacks _e;
	_e.on_update = NULL;
	_e.on_fixed_update = NULL;
	_e.on_late_update = NULL;
	_e.on_render = NULL;
	return _e;
}

rde_display_info rde_struct_create_display_info() {
	rde_display_info _d;
	_d.index = -1;
	return _d;
}
	
rde_event_window rde_struct_create_event_window() {
	rde_event_window _e;
	_e.position.x = -1;
	_e.position.y = -1;
	_e.size.x = -1;
	_e.size.y = -1;
	_e.display_index = -1;
	_e.minimized = false;
	_e.maximized = false;
	return _e;
}

rde_event_display rde_struct_create_event_display() {
	rde_event_display _e;
	_e.orientation = -1;
	_e.display_index = -1;
	return _e;
}

rde_event_key rde_struct_create_event_key() {
	rde_event_key _e;
	_e.key = RDE_KEYBOARD_KEY_NONE;
	_e.typed_char = '\0';
	_e.amount_of_times_pressed = -1;
	return _e;
}

rde_event_mouse rde_struct_create_event_mouse() {
	rde_event_mouse _e;
	_e.button = RDE_MOUSE_BUTTON_NONE;
	_e.position.x = -1;
	_e.position.y = -1;
	_e.scrolled.x = -1.f;
	_e.scrolled.y = -1.f;
	return _e;
}

rde_event_controller rde_struct_create_event_controller() {
	rde_event_controller _e;
	_e.controller_id = -1;
	_e.left_joystick.x = -1.f;
	_e.left_joystick.y = -1.f;
	_e.right_joystick.x = -1.f;
	_e.right_joystick.y = -1.f;
	_e.back_buttons.x = -1.f;
	_e.back_buttons.y = -1.f;
	_e.button = RDE_CONTROLLER_BUTTON_NONE;
	_e.axis = RDE_CONTROLLER_AXIS_NONE;
	return _e;
}

rde_event_mobile_pinch rde_struct_create_event_mobile_pinch() {
	rde_event_mobile_pinch _m;
	_m.rotation_of_fingers = 0.f;
	_m.distance_moved_between_fingers = 0.f;
	_m.num_fingers_used = 0;
	return _m;
}

rde_event_mobile rde_struct_create_event_mobile() {
	rde_event_mobile _e;
	_e.init_touch_position.x = -1;
	_e.init_touch_position.y = -1;
	_e.moved_touch_position.y = -1;
	_e.moved_touch_position.y = -1;
	_e.end_touch_position.x = -1;
	_e.end_touch_position.y = -1;
	_e.pressure = -1.f;
	_e.finger_id = -1;
	_e.pinch = rde_struct_create_event_mobile_pinch();
	return _e;
}

rde_event_drag_and_drop rde_struct_create_event_drag_and_drop() {
	rde_event_drag_and_drop _e;
	_e.window_id = 0;
	_e.file_path = NULL;
	return _e;
}

rde_event_data rde_struct_create_event_data() {
	rde_event_data _e;
	_e.window_event_data = rde_struct_create_event_window();
	_e.key_event_data = rde_struct_create_event_key();
	_e.mouse_event_data = rde_struct_create_event_mouse();
	_e.controller_event_data = rde_struct_create_event_controller();
	_e.mobile_event_data = rde_struct_create_event_mobile();
	_e.display_event_data = rde_struct_create_event_display();
	_e.drag_and_drop_data = rde_struct_create_event_drag_and_drop();
	return _e;
}

rde_camera rde_struct_create_camera(RDE_CAMERA_TYPE_ _camera_type) {
	static size_t _camera_counter = 0;
	rde_camera _c;
	_c.id = _camera_counter++;
	_c.zoom = 1.f;
	_c.fov = 45.f;
	_c.transform = rde_engine_transform_load();
	_c.camera_type = _camera_type;
	_c.enabled = true;
	_c.direction = (rde_vec_3F) { 0.0f, 0.0f, -1.0f };
	_c.up = (rde_vec_3F) { 0.0f, 1.0f, 0.0f };
	_c.near_far = (rde_vec_2F) { 0.1f, 100.f };
	return _c;
}

rde_color rde_struct_create_color() {
	rde_color _c;
	_c.r = 255;
	_c.g = 255;
	_c.b = 255;
	_c.a = 255;
	return _c;
}

rde_event rde_struct_create_event() {
	rde_event _e;
	_e.type = RDE_EVENT_TYPE_NONE;
	_e.time_stamp = 0;
	_e.window_id = 0;
	_e.handled = false;
	_e.data = rde_struct_create_event_data();
	_e.sdl_native_event = NULL;
	return _e;
}



rde_window rde_struct_create_window() {
	rde_window _w;
	_w.sdl_window = NULL;
	_w.mouse_position = (rde_vec_2I) { 0, 0 };
	_w.mouse_scroll = (rde_vec_2F) { 0.0f, 0.0f };

	for(size_t _i = 0; _i < RDE_AMOUNT_OF_KEYS; _i++) {
		_w.key_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}

	for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOUSE_BUTTONS; _i++) {
		_w.mouse_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}

	for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOBILE_FINGERS; _i++) {
		_w.mobile_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}

	return _w;
}

rde_material_light_data rde_struct_create_material_light_data() {
	rde_material_light_data _m;
	_m.shininess = 1.0f;
	_m.ka = (rde_vec_3F) { 0.2f, 0.2f, 0.2f };
	_m.kd = (rde_vec_3F) { 0.5f, 0.5f, 0.5f };
	_m.ks = (rde_vec_3F) { 0.0f, 0.0f, 0.0f };
	return _m;
}

rde_material rde_struct_create_material() {
	rde_material _m;
	_m.map_ka = NULL;
	_m.map_kd = NULL;
	_m.map_ks = NULL;
	_m.map_bump = NULL;
	_m.render_texture = NULL;
	_m.material_light_data = rde_struct_create_material_light_data();
	return _m;
}

rde_directional_light rde_struct_create_directional_light() {
	rde_directional_light _d;
	_d.direction = (rde_vec_3F) { -0.2f, -1.0f, -0.3f };
	_d.ambient_color = (rde_vec_3F) { 0.2f, 0.2f, 0.2f };
	_d.diffuse_color = (rde_vec_3F) { 0.5f, 0.5f, 0.5f };
	_d.specular_color = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
	return _d;
}

rde_point_light rde_struct_create_point_light() {
	rde_point_light _p;
	_p.position = (rde_vec_3F) { 0.0, 0.0f, 0.0f };
	_p.ambient_color = (rde_vec_3F) { 0.2f, 0.2f, 0.2f };
	_p.diffuse_color = (rde_vec_3F) { 0.5f, 0.5f, 0.5f };
	_p.specular_color = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
	_p.constant = 1.0f;
	_p.linear = 0.09f;
	_p.quadratic = 0.032f;
	return _p;
}

rde_spot_light rde_struct_create_spot_light() {
	rde_spot_light _s;
	_s.position = (rde_vec_3F) { 0.0, 0.0f, 0.0f };
	_s.direction = (rde_vec_3F) { 0.0, -1.0f, 0.0f };
	_s.cut_off = 0.99999f;
	_s.outer_cut_off = 0.99999f;
	_s.ambient_color = (rde_vec_3F) { 0.2f, 0.2f, 0.2f };
	_s.diffuse_color = (rde_vec_3F) { 0.8f, 0.8f, 0.8f };
	_s.specular_color = (rde_vec_3F) { 1.0f, 1.0f, 1.0f };
	_s.constant = 1.0f;
	_s.linear = 0.09f;
	_s.quadratic = 0.032f;
	return _s;
}

rde_shader rde_struct_create_shader() {
	rde_shader _s;
	_s.vertex_program_id = 0;
	_s.fragment_program_id = 0;
	_s.compiled_program_id = -1;
	memset(_s.name, 0 , RDE_SHADER_MAX_NAME);
	return _s;
}
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
	memset(_t.file_path, 0, RDE_MAX_PATH);
	_t.atlas_texture = NULL;
	_t.pixels = NULL;
	_t.pixels_changed = false;
	return _t;
}

rde_texture_parameters rde_struct_texture_parameters() {
	rde_texture_parameters _t;
	_t.min_filter = RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR;
	_t.mag_filter = RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR;
	_t.wrap_t = RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT;
	_t.wrap_s = RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT;
	_t.mipmap_min_filter = RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_LINEAR_MIN_FILTER_LINEAR;
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
	memset(_t->file_path, 0, RDE_MAX_PATH);
	_t->atlas_texture = NULL;
	_t->pixels = NULL;
	_t->pixels_changed = false;
}

rde_atlas rde_struct_create_atlas() {
	rde_atlas _a;
	_a.texture = NULL;
	_a.sub_textures = NULL;
	return _a;
}

rde_font_char_info rde_struct_create_font_char_info() {
	rde_font_char_info _f;
	_f.advance = (rde_vec_2I) { 0, 0 };
	_f.bearing = (rde_vec_2I) { 0, 0 };
	_f.size = (rde_vec_2I) { 0, 0 };
	_f.offset = (rde_vec_2I) { 0.0f, 0.0f };
	_f.texture = rde_struct_create_texture();
	return _f;
}

rde_font rde_struct_create_font() {
	rde_font _f;
	_f.texture = NULL;
	_f.chars = NULL;
	return _f;
}

rde_vertex_2d rde_struct_create_vertex_2d() {
	rde_vertex_2d _v;
	_v.position.x = 0.f;
	_v.position.y = 0.f;
	_v.position.z = 0.f;
	_v.texture_coordinates.x = 0.f;
	_v.texture_coordinates.y = 0.f;
	_v.color = 0xFFFFFF;
	return _v;
}

rde_batch_2d rde_struct_create_2d_batch() {
	rde_batch_2d _b;
	_b.shader = NULL;
	_b.texture = rde_struct_create_texture();
	_b.vertices = NULL;
	_b.amount_of_vertices = 0;
	_b.vertex_buffer_object = 0;
	_b.vertex_array_object = 0;
	_b.index_buffer_object = 0;
	glm_mat4_copy(GLM_MAT4_IDENTITY, _b.mvp);
	_b.is_hud = false;
	return _b;
}


rde_line_vertex rde_struct_create_line_vertex() {
	rde_line_vertex _l;
	_l.position = (rde_vec_3F) { 0, 0, 0 };
	_l.color = 0xFFFFFFFF;
	return _l;
}

rde_line_batch rde_struct_create_line_batch() {
	rde_line_batch _b;
	_b.vertices = NULL;
	_b.amount_of_vertices = 0;
	_b.thickness = 0;
	_b.vertex_buffer_object = RDE_UINT_MAX;
	_b.vertex_array_object = RDE_UINT_MAX;
	_b.shader = NULL;
	return _b;
}

rde_batch_3d rde_struct_create_batch_3d() {
	rde_batch_3d _b;
	_b.mesh = NULL;
	_b.shader = NULL;
	_b.amount_of_models_per_draw = 0;
	_b.line_batch = rde_struct_create_line_batch();
	_b.draw_mesh_wireframe = false;
	return _b;
}

rde_model rde_struct_create_model() {
	rde_model _m;
	_m.mesh_array = NULL;
	_m.mesh_array_size = 0;
	memset(_m.file_path, 0, RDE_MAX_PATH);
	return _m;
}

rde_skybox rde_struct_create_skybox() {
	rde_skybox _s;
	_s.vao = RDE_UINT_MAX;
	_s.vbo = RDE_UINT_MAX;
	_s.opengl_texture_id = -1;
	return _s;
}

rde_antialiasing rde_struct_create_antialiasing() {
	rde_antialiasing _a;
	_a.frame_buffer_id = RDE_UINT_MAX;
	_a.render_buffer_id = RDE_UINT_MAX;
	_a.opengl_texture_id = -1;
	_a.samples = 0;
	return _a;
}

rde_polygon rde_struct_create_polygon() {
	rde_polygon _p;
	_p.vertices = NULL;
	_p.vertices_count = 0;
	return _p;
}

rde_shadows rde_struct_create_shadows() {
	rde_shadows _s;
	_s.frame_buffer_id = RDE_UINT_MAX;
	_s.opengl_texture_id = -1;
	return _s;
}

#ifdef RDE_OBJ_MODULE
rde_obj_mesh_data rde_inner_struct_create_obj_mesh_data() {
	rde_obj_mesh_data _o;
	_o.name = NULL;
	_o.vertex_count = 0;
	_o.material = NULL;

	_o.positions_size = 0;
	_o.positions = NULL;
	_o.positions_pointer = 0;

	_o.texcoords = NULL;
	_o.texcoords_size = 0;
	_o.texcoords_pointer = 0;

	_o.normals = NULL;
	_o.normals_size = 0;
	_o.normals_pointer = 0;

	_o.map_ka = NULL;
	_o.map_kd = NULL;
	_o.map_ks = NULL;
	_o.map_bump = NULL;

	return _o;
}
#endif

#ifdef RDE_AUDIO_MODULE
rde_sound rde_struct_create_sound() {
	rde_sound _s;
	_s.used = false;
	_s.playing = false;
	_s.paused = false;
	_s.looping = false;
	_s.played_frame = 0;
	return _s;
}

rde_sound_config rde_struct_create_audio_config() {
	rde_sound_config _s;
	_s.user_data = NULL;
	_s.channels = 2;
	_s.rate = 48000;
	return _s;
}
#endif

rde_engine rde_struct_create_engine(rde_engine_init_info _engine_init_info) {
	rde_engine _e;

	_e.init_info = _engine_init_info;
	_e.init_info.heap_allocs_config.max_amount_of_shaders += RDE_DEFAULT_SHADERS_AMOUNT;

	_e.transforms = (rde_transform*)malloc(sizeof(rde_transform) * RDE_MAX_TRANSFORMS);
	_e.world_transforms = (mat4*)malloc(sizeof(mat4) * RDE_MAX_TRANSFORMS);

	for(size_t _i = 0; _i < RDE_MAX_TRANSFORMS; _i++) {
		_e.transforms[_i] = rde_struct_create_transform();
		_e.transforms[_i].index = _i;
		glm_mat4_identity(_e.world_transforms[_i]);
	}
	_e.last_transform_used = -1;
	_e.free_transforms = NULL;
	stbds_arrput(_e.free_transforms, 0);

#if IS_ANDROID()
	_e.android_env = SDL_AndroidGetJNIEnv();
	rde_critical_error(_e.android_env == NULL, "Native Android window is NULL");
#endif
	
	_e.total_amount_of_textures = 0;
	_e.total_amount_of_textures += _e.init_info.heap_allocs_config.max_amount_of_textures;
	_e.total_amount_of_textures += _e.init_info.heap_allocs_config.max_amount_of_fonts;
	_e.total_amount_of_textures += _e.init_info.heap_allocs_config.max_amount_of_atlases;
	_e.total_amount_of_textures += _e.init_info.heap_allocs_config.max_amount_of_models_textures;

	_e.user_event_callback = NULL;
	_e.delta_time = 0.f;
	_e.fixed_delta_time = 1.f / 60.f;
	_e.fixed_time_step_accumulator = 0.f;
	_e.platform_type = RDE_PLATFORM_TYPE_UNSUPPORTED;
	_e.running = true;
	_e.pause_loop = false;
	_e.use_rde_2d_physics_system = true;
	_e.supress_engine_logs = false;
	_e.mandatory_callbacks = rde_struct_create_end_user_mandatory_callbacks();

	_e.illumination.directional_light = rde_struct_create_directional_light();
	
	if(_e.init_info.illumination_config.max_amount_of_point_lights > 0) {
		_e.illumination.point_lights = (rde_point_light**)malloc(sizeof(rde_point_light*) * _e.init_info.illumination_config.max_amount_of_point_lights);
		for(size_t _i = 0; _i < _e.init_info.illumination_config.max_amount_of_point_lights; _i++) {
			_e.illumination.point_lights[_i] = NULL;
		}
	} else {
		_e.illumination.point_lights = NULL;
	}

	if(_e.init_info.illumination_config.max_amount_of_spot_lights > 0) {
		_e.illumination.spot_lights = (rde_spot_light**)malloc(sizeof(rde_spot_light*) * _e.init_info.illumination_config.max_amount_of_spot_lights);
		for(size_t _i = 0; _i < _e.init_info.illumination_config.max_amount_of_spot_lights; _i++) {
			_e.illumination.spot_lights[_i] = NULL;
		}
	} else {
		_e.illumination.spot_lights = NULL;
	}

	_e.line_shader = NULL;
	_e.color_shader_2d = NULL;
	_e.texture_shader_2d = NULL;
	_e.text_shader_2d = NULL;
	_e.framebuffer_shader = NULL;
	_e.mesh_shader = NULL;
	_e.skybox_shader = NULL;
	_e.skybox = rde_struct_create_skybox();
	_e.antialiasing = rde_struct_create_antialiasing();
	_e.shadows = rde_struct_create_shadows();

	_e.init_info.heap_allocs_config.max_amount_of_shaders += RDE_SHADERS_AMOUNT;

	rde_critical_error(_e.init_info.heap_allocs_config.max_amount_of_shaders <= 0, RDE_ERROR_HEAP_ALLOC_BAD_VALUE, "shaders", _e.init_info.heap_allocs_config.max_amount_of_shaders);
	_e.shaders = (rde_shader*)malloc(sizeof(rde_shader) * _e.init_info.heap_allocs_config.max_amount_of_shaders);
	rde_critical_error(_e.shaders == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_shader) * _e.init_info.heap_allocs_config.max_amount_of_shaders, "shaders");
	for(size_t _i = 0; _i < _e.init_info.heap_allocs_config.max_amount_of_shaders; _i++) {
		_e.shaders[_i] = rde_struct_create_shader();
	}

	rde_critical_error(_e.init_info.heap_allocs_config.max_amount_of_windows <= 0, RDE_ERROR_HEAP_ALLOC_BAD_VALUE, "windows", _e.init_info.heap_allocs_config.max_amount_of_windows);
	_e.windows = (rde_window*)malloc(sizeof(rde_window) * _e.init_info.heap_allocs_config.max_amount_of_windows);
	rde_critical_error(_e.windows == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_window) * _e.init_info.heap_allocs_config.max_amount_of_windows, "windows");
	for(size_t _i = 0; _i < _e.init_info.heap_allocs_config.max_amount_of_windows; _i++) {
		_e.windows[_i] = rde_struct_create_window();
	}

	if (_e.total_amount_of_textures > 0) {
		_e.textures = (rde_texture*)malloc(sizeof(rde_texture) * _e.total_amount_of_textures);
		rde_critical_error(_e.textures == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_texture) * _e.total_amount_of_textures, "textures");
		for (size_t _i = 0; _i < _e.total_amount_of_textures; _i++) {
			_e.textures[_i] = rde_struct_create_texture();
		}
	} else {
		_e.textures = NULL;
	}

	if(_e.init_info.heap_allocs_config.max_amount_of_atlases > 0) {
		_e.atlases = (rde_atlas*)malloc(sizeof(rde_atlas) * _e.init_info.heap_allocs_config.max_amount_of_atlases);
		rde_critical_error(_e.atlases == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_atlas) * _e.init_info.heap_allocs_config.max_amount_of_atlases, "atlases");
		for(size_t _i = 0; _i < _e.init_info.heap_allocs_config.max_amount_of_atlases; _i++) {
			_e.atlases[_i] = rde_struct_create_atlas();
		}
	} else {
		_e.atlases = NULL;
	}

	if (_e.init_info.heap_allocs_config.max_amount_of_fonts > 0) {
		_e.fonts = (rde_font*)malloc(sizeof(rde_font) * _e.init_info.heap_allocs_config.max_amount_of_fonts);
		rde_critical_error(_e.fonts == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_font) * _e.init_info.heap_allocs_config.max_amount_of_fonts, "fonts");
		for (size_t _i = 0; _i < _e.init_info.heap_allocs_config.max_amount_of_fonts; _i++) {
			_e.fonts[_i] = rde_struct_create_font();
		}
	} else {
		_e.fonts = NULL;
	}

	if(_e.init_info.heap_allocs_config.max_amount_of_models > 0) {
		_e.models = (rde_model*)malloc(sizeof(rde_model) * _e.init_info.heap_allocs_config.max_amount_of_models);
		rde_critical_error(_e.models == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_atlas) * _e.init_info.heap_allocs_config.max_amount_of_models, "atlases");
		for(size_t _i = 0; _i < _e.init_info.heap_allocs_config.max_amount_of_models; _i++) {
			_e.models[_i] = rde_struct_create_model();
		}
	} else {
		_e.models = NULL;
	}

#ifdef RDE_AUDIO_MODULE
	if (_e.init_info.heap_allocs_config.max_amount_of_sounds > 0) {
		_e.sounds = (rde_sound*)malloc(sizeof(rde_sound) * _e.init_info.heap_allocs_config.max_amount_of_sounds);
		rde_critical_error(_e.sounds == NULL, RDE_ERROR_NO_MEMORY"This error probably happened because you compiled the engine with audio option but did not set the amount on rde_init_config.", sizeof(rde_sound) * _e.init_info.heap_allocs_config.max_amount_of_sounds, "audio");
		for (size_t _i = 0; _i < _e.init_info.heap_allocs_config.max_amount_of_sounds; _i++) {
			_e.sounds[_i] = rde_struct_create_sound();
		}
	} else {
		_e.sounds = NULL;
	}
#endif

	for(size_t _i = 0; _i < RDE_WIN_EVENT_COUNT; _i++) { 
		_e.window_events[_i] = NULL;
	}

	for(size_t _i = 0; _i < RDE_DISPLAY_EVENT_COUNT; _i++) { 
		_e.display_events[_i] = NULL;
	}

	for(size_t _i = 0; _i < RDE_KEY_EVENT_COUNT; _i++) { 
		_e.key_events[_i] = NULL;
	}

	for(size_t _i = 0; _i < RDE_MOUSE_EVENT_COUNT; _i++) { 
		_e.mouse_events[_i] = NULL;
	}

	for(size_t _i = 0; _i < RDE_DRAG_AND_DROP_EVENT_COUNT; _i++) { 
		_e.drag_and_drop_events[_i] = NULL;
	}
	
	for(size_t _i = 0; _i < RDE_MOBILE_EVENT_COUNT; _i++) { 
		_e.mobile_events[_i] = NULL;
	}

#if IS_WINDOWS()
	_e.console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	#ifdef RDE_ERROR_MODULE
	SetUnhandledExceptionFilter(rde_inner_error_sig_handler);
	#endif
#elif (IS_MAC() || IS_WINDOWS()) && !IS_ANDROID()
	rde_inner_set_posix_signal_handler();
#endif

	return _e;
}

/// *************************************************************************************************
/// *                                GLOBAL VARIABLE DEFINITION                        		     *
/// *************************************************************************************************

/// Engine
rde_engine ENGINE;

/// File System
#define RDE_MAX_CONCURRENT_FILES_OPENED 64
 rde_file_handle concurrent_file_handlers[RDE_MAX_CONCURRENT_FILES_OPENED];

/// Rendering
#if IS_MOBILE()
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC glFramebufferTexture2DMultisampleEXT = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC glRenderbufferStorageMultisampleEXT = NULL;
PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXT = NULL;
#endif

float FRAMEBUFFER_QUAD_DATA[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};


rde_render_texture* DEFAULT_RENDER_TEXTURE = NULL;
rde_render_texture* current_render_texture = NULL;
rde_camera* current_drawing_camera = NULL;
rde_window* current_drawing_window = NULL;
mat4 projection_matrix;
rde_rendering_statistics statistics;

rde_batch_2d current_batch_2d;

rde_batch_3d current_batch_3d;
rde_texture* DEFAULT_TEXTURE;

/// Audio
#ifdef RDE_AUDIO_MODULE
 bool is_miniaudio_initialized = false;
#endif

/// Error
#ifdef RDE_ERROR_MODULE
int times_enter_in_error = 0;
#endif

/// *************************************************************************************************
/// *                                SOURCE FUNCTION DECLARATIONS                        		   *
/// *************************************************************************************************

/// ******************************************* UTIL ***********************************************

char* rde_inner_append_str(char* _string, const char* _append);
char* rde_inner_strtokk(char* _string, const char* _strf);

/// ******************************************* FILE_SYSTEM ***********************************************

rde_atlas_sub_textures* rde_inner_file_system_read_atlas_config(const char* _atlas_path, rde_texture* _atlas);
rde_font_char_info* rde_inner_file_system_read_font_config(const char* _font_path, rde_texture* _atlas);
const char* rde_inner_file_system_file_mode_to_char(const RDE_FILE_MODE_ _mode);
void rde_inner_file_system_free_text_allocation(rde_file_handle* _handler);
void rde_inner_file_system_check_file_mode_or_convert(rde_file_handle* _handler, RDE_FILE_MODE_ _expected);

/// ******************************************* WINDOW ***********************************************

#if IS_WINDOWS()
rde_window* rde_inner_window_create_windows_window(size_t _free_window_index);
#endif

#if IS_MAC()
rde_window* rde_inner_window_create_mac_window(size_t _free_window_index);
#endif

#if IS_LINUX() && !IS_ANDROID()
rde_window* rde_inner_window_create_linux_window(size_t _free_window_index);
#endif

#if IS_ANDROID()
rde_window* rde_inner_window_create_android_window(size_t _free_window_index);
#endif

#if IS_IOS()
rde_window* rde_inner_window_create_ios_window(size_t _free_window_index);
#endif

#if 0
rde_window* rde_window_create_wasm_window(size_t _free_window_index);
#endif

/// ******************************************* RENDERING  ***********************************************

void rde_inner_rendering_generate_gl_vertex_config_for_quad_2d(rde_batch_2d* _batch);
rde_vec_2F rde_inner_rendering_get_aspect_ratio();
void rde_inner_rendering_set_rendering_configuration(rde_window* _window);
rde_texture_parameters rde_innner_rendering_validate_texture_parameters(const rde_texture_parameters* _params);
void rde_inner_rendering_flush_render_texture_3d();
void rde_inner_rendering_destroy_current_antialiasing_config();
void rde_inner_rendering_create_shadows();
void rde_inner_rendering_draw_scene_shadows(rde_window* _window, rde_camera* _camera);
void rde_inner_rendering_destroy_shadows();
void rde_inner_rendering_flush_to_default_render_texture(rde_window* _window);
void rde_inner_engine_on_render(float _dt, rde_window* _window);

void rde_inner_rendering_init_2d();
void rde_inner_rendering_end_2d();
void rde_inner_rendering_transform_to_glm_mat4_2d(const rde_transform* _transform, mat4 _mat);
void rde_inner_rendering_generate_gl_vertex_config_for_quad_2d(rde_batch_2d* _batch);
void rde_inner_rendering_reset_batch_2d();
void rde_inner_rendering_try_create_batch_2d(rde_shader* _shader, const rde_texture* _texture);
void rde_inner_rendering_flush_batch_2d();
void rde_inner_rendering_try_flush_batch_2d(rde_shader* _shader, const rde_texture* _texture, size_t _extra_vertices);

#ifdef RDE_OBJ_MODULE
rde_obj_mesh_data rde_inner_struct_create_obj_mesh_data();
void rde_inner_fill_obj_mesh_data(rde_obj_mesh_data* _data, fastObjGroup* _group, fastObjMaterial* _material, bool _has_t, bool _has_n);
void rde_inner_parse_3_vertices_face_obj(uint _v, uint _offset, fastObjMesh* _mesh, rde_obj_mesh_data* _obj_mesh_data);
rde_model* rde_inner_obj_load_model(const char* _obj_path);
#endif

#ifdef RDE_FBX_MODULE
rde_model* rde_rendering_load_fbx_model(const char* _fbx_path, const char* _texture_path);
void parse_3_vertices_face_fbx(uint _i, uint _v, uint* _mesh_indices, 
                               float* _mesh_positions, float* _mesh_texcoords, ufbx_face* _face, ufbx_mesh* _mesh,
                           	   uint* _indices_pointer, uint* _positions_pointer, 
                           	   uint* _texcoords_pointer);
#endif

rde_mesh rde_inner_struct_create_mesh(rde_mesh_gen_data* _data);
void rde_inner_rendering_try_flush_batch_3d(rde_shader* _shader, rde_mesh* _mesh, size_t _extra_floats);
void rde_inner_rendering_try_flush_line_batch(rde_shader* _shader, unsigned short _thickness, size_t _extra_floats);
void rde_inner_rendering_try_create_batch_3d(rde_shader* _shader, rde_mesh* _mesh);
void rde_inner_rendering_try_create_line_batch(rde_shader* _shader, unsigned short _thickness);
void rde_inner_rendering_flush_batch_3d();
void rde_inner_rendering_flush_line_batch();
void rde_inner_rendering_reset_batch_3d();
void rde_inner_rendering_reset_line_batch();
void rde_inner_create_line_batch_buffers();
void rde_inner_destroy_line_batch_buffers();
void rde_inner_rendering_init_3d();
void rde_inner_rendering_end_3d();
void rde_inner_rendering_transform_to_glm_mat4_3d(const rde_transform* _transform, mat4 _mat);
float* rde_inner_rendering_mesh_calculate_normals(float* _vertex_positions, size_t _indices_count, size_t _vertex_count, uint* _indices);
bool rde_inner_rendering_is_mesh_ok_to_render(rde_mesh* _mesh);

/// ******************************************* PHYSICS *********************************************

#ifdef RDE_PHYSICS_MODULE
rde_vec_3F rde_inner_physics_rotate_box_point(rde_vec_3F _point, rde_vec_3F _rotation, rde_vec_3F _bounds);
void rde_physics_draw_debug_box(rde_transform* _transform, rde_jolt_box_shape_bounds* _box_bounds);
void rde_physics_draw_debug_shapes_specific(rde_jolt_body* _body, rde_jolt_shape* _shape, rde_transform* _transform);
void rde_jolt_draw_debug_shapes(rde_window* _window, rde_camera* _camera);
#endif

/// ******************************************* EVENTS *********************************************

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
void rde_events_sync_events(rde_window* _window);

/// ******************************************* AUDIO *********************************************

#ifdef RDE_MODULE_AUDIO
void data_callback(ma_device* _device, void* _output, const void* _input, ma_uint32 _frame_count);
#endif

/// ******************************************* MOBILE *********************************************

#if IS_ANDROID()
ANativeWindow* rde_android_get_native_window();
#endif

/// ******************************************* ENGINE *********************************************

void rde_inner_engine_on_event();
void rde_inner_engine_on_update(float _dt);
#if defined(RDE_PHYSICS_MODULE) || defined(RDE_PHYSICS_2D_MODULE)
void rde_inner_engine_on_fixed_update(float _fixed_dt);
#endif
void rde_inner_engine_on_late_update(float _dt);
void rde_inner_engine_sync_events();
void rde_engine_transform_update();
void rde_engine_transform_get_matrix(rde_transform* _t, mat4 _mat);
void rde_engine_transform_parse_parent(rde_transform* _transform, mat4 _mat);
void rde_engine_transform_remove_transform_from_parent_children(rde_transform* _transform);








// ==============================================================================
// =							PRIVATE API - ENGINE					 	   =
// ==============================================================================

void rde_inner_engine_on_event() {

	SDL_Event _event;
        
	SDL_PumpEvents();

	while (SDL_PollEvent(&_event)) {

		rde_event _rde_event = rde_inner_event_sdl_event_to_rde_event(&_event);
		_rde_event.sdl_native_event = (void*)&_event;
		
		for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_windows; _i++) {
			rde_window* _window = &ENGINE.windows[_i];

			if(_window->sdl_window == NULL) {
				continue;
			}

			// TODO: Use the if statement on the following event types:
			//			- SDL_TextEditingEvent
			//			- SDL_TextInputEvent
			//
			//		Also, but without the if, still need to implement:
			//			- SDL_ControllerAxisEvent
			//			- SDL_ControllerButtonEvent
			//			- SDL_ControllerDeviceEvent
			//			- SDL_AudioDeviceEvent
			switch(_event.type) {
				case SDL_WINDOWEVENT:	{
					size_t _window_id = SDL_GetWindowID(_window->sdl_window);
					if(_window_id != _rde_event.window_id) {
						continue;
					}
					rde_events_window_consume_events(&_rde_event, _window);
				} break;
				
				case SDL_DISPLAYEVENT: rde_events_display_consume_events(&_rde_event, _window); break;
				
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					size_t _window_id = SDL_GetWindowID(_window->sdl_window);
					if(_window_id != _rde_event.window_id) {
						continue;
					}
					rde_events_keyboard_consume_events(&_rde_event, _window);
				} break;

				case SDL_MOUSEMOTION:
				case SDL_MOUSEWHEEL:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP: {
					size_t _window_id = SDL_GetWindowID(_window->sdl_window);
					if(_window_id != _rde_event.window_id) {
						continue;
					}
					rde_events_mouse_consume_events(&_rde_event, _window);
				} break;

				case SDL_DROPFILE: {
					size_t _window_id = SDL_GetWindowID(_window->sdl_window);
					if(_window_id != _rde_event.window_id) {
						continue;
					}
					rde_events_drag_and_drop_consume_events(&_rde_event, _window);
				} break;

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
				case SDL_LOCALECHANGED: rde_events_mobile_consume_events(&_rde_event, _window); break;
			}

			if(ENGINE.user_event_callback != NULL) {
				ENGINE.user_event_callback(&_rde_event, _window);
			}

			if(_rde_event.type == RDE_EVENT_TYPE_DRAG_AND_DROP_FILE && ((SDL_Event*)(_rde_event.sdl_native_event))->drop.file != NULL) {
				SDL_free(((SDL_Event*)(_rde_event.sdl_native_event))->drop.file);
			}
		}
	}
}

void rde_inner_engine_on_update(float _dt) {
	UNUSED(_dt)
}

#if defined(RDE_PHYSICS_MODULE) || defined(RDE_PHYSICS_2D_MODULE)
void rde_inner_engine_on_fixed_update(float _fixed_dt) {
	rde_jolt_update(_fixed_dt);
}
#endif

void rde_inner_engine_on_late_update(float _dt) {
	UNUSED(_dt)
}

rde_display_info* rde_engine_get_available_displays() {
	UNIMPLEMENTED();
	return NULL;
}

void rde_engine_transform_get_matrix(rde_transform* _t, mat4 _mat) {
		vec3 _scale;
	    _scale[0] = _t->scale.x;
	    _scale[1] = _t->scale.y;
	    _scale[2] = _t->scale.z;

		glm_mat4_identity(_mat);

		mat4 _scale_mat;
		glm_mat4_identity(_scale_mat);
		glm_scale(_scale_mat, _scale);

		mat4 _rotation_mat;
		glm_mat4_identity(_rotation_mat);
		glm_rotate_x(_rotation_mat, rde_math_degrees_to_radians(_t->rotation.x), _rotation_mat);
		glm_rotate_y(_rotation_mat, rde_math_degrees_to_radians(_t->rotation.y), _rotation_mat);
		glm_rotate_z(_rotation_mat, rde_math_degrees_to_radians(_t->rotation.z), _rotation_mat);

		mat4 _position_mat;
		glm_mat4_identity(_position_mat);
		glm_translate(_position_mat, (vec3) { _t->position.x, _t->position.y, _t->position.z });

		glm_mat4_mul(_mat, _position_mat, _mat);
		glm_mat4_mul(_mat, _scale_mat, _mat);
		glm_mat4_mul(_mat, _rotation_mat, _mat);
}

bool rde_engine_transform_get_or_calculate_frame_world_matrix(rde_transform* _transform, mat4 _mat) {
	bool _was_updated_this_frame = false;
	if(_transform->updated_this_frame) {
		glm_mat4_copy(ENGINE.world_transforms[_transform->index], _mat);
		_was_updated_this_frame = true;
	} else {
		rde_engine_transform_get_matrix(_transform, _mat);
		glm_mat4_copy(_mat, ENGINE.world_transforms[_transform->index]);
	}

	_transform->updated_this_frame = true;
	return _was_updated_this_frame;
}

void rde_engine_transform_parse_parent(rde_transform* _transform, mat4 _mat) {
	mat4 _p_local_matrix;
	bool _was_updated = rde_engine_transform_get_or_calculate_frame_world_matrix(&ENGINE.transforms[_transform->parent], _p_local_matrix);
	
	if(ENGINE.transforms[_transform->parent].parent > -1 && !_was_updated) {
		rde_engine_transform_parse_parent(&ENGINE.transforms[_transform->parent], _p_local_matrix);
	}

	glm_mat4_mul(_p_local_matrix, _mat, _mat);
}

void rde_engine_transform_remove_transform_from_parent_children(rde_transform* _transform) {
	if(_transform->parent != -1) {
		rde_transform* _parent = &ENGINE.transforms[_transform->parent];
		for(size_t _i = 0; _i < stbds_arrlenu(_parent->children); _i++) {
			if(_parent->children[_i] == _transform->index) {
				stbds_arrdel(_parent->children, _i);
				break;
			}
		}
	}

	for(size_t _i = 0; _i < stbds_arrlenu(_transform->children); _i++) {
		ENGINE.transforms[_transform->children[_i]].parent = -1;
	}

	stbds_arrfree(_transform->children);
	_transform->parent = -1;
	_transform->children = NULL;
}

void rde_engine_transform_update() {
	for(int _i = 0; _i <= ENGINE.last_transform_used; _i++) {
		rde_transform* _t = &ENGINE.transforms[_i];
		if(_t->parent > -1) {
			int _parent_index = _t->parent;
			bool _any_parent_dirty = false;
			while(_parent_index != -1) {
				rde_transform* _parent_t = &ENGINE.transforms[_parent_index];
				_any_parent_dirty |= _parent_t->dirty;
				_parent_index = _parent_t->parent;
			}
			
			if(_any_parent_dirty || _t->dirty) {
				mat4 _t_local_matrix;
				rde_engine_transform_get_or_calculate_frame_world_matrix(_t, _t_local_matrix);
				rde_engine_transform_parse_parent(_t, _t_local_matrix);
				glm_mat4_copy(_t_local_matrix, ENGINE.world_transforms[_t->index]);
			}
			_t->updated_this_frame = true;
		} else if(_t->dirty) {
			mat4 _t_local_matrix;
			rde_engine_transform_get_or_calculate_frame_world_matrix(_t, _t_local_matrix);
			glm_mat4_copy(_t_local_matrix, ENGINE.world_transforms[_t->index]);
		}
	}

	for(int _i = 0; _i <= ENGINE.last_transform_used; _i++) {
		ENGINE.transforms[_i].dirty = false;
		ENGINE.transforms[_i].updated_this_frame = false;
	}
}

// ==============================================================================
// =							PUBLIC API - ENGINE					 		=
// ==============================================================================

rde_window* rde_engine_create_engine(int _argc, char** _argv, const char* _config_path) {
	static bool _instantiated = false;
	
	UNUSED(_argc)
	UNUSED(_argv)
	
	rde_critical_error(_instantiated, RDE_ERROR_MULTIPLE_ENGINE);
	
	rde_engine_init_info _engine_init_info = rde_engine_load_config(_config_path);
	ENGINE = rde_struct_create_engine(_engine_init_info);

	SDL_version _version;
	SDL_GetVersion(&_version);
	rde_log_level(RDE_LOG_LEVEL_INFO, "SDL Version: %d.%d.%d", _version.major, _version.minor, _version.patch);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        rde_critical_error(true, "Could not initialize components of SDL on Init, fatal error: %s", SDL_GetError());
    }

	stbds_rand_seed(time(NULL));

	rde_window* _default_window = rde_window_create_window_os();

	#if IS_ANDROID()
	ENGINE.android_native_window = SDL_AndroidGetNativeWindow();
	rde_critical_error(ENGINE.android_native_window == NULL, "Native Android Window is NULL");
	#endif

	rde_inner_events_window_create_events();
	rde_inner_events_display_create_events();
	rde_inner_events_key_create_events();
	rde_inner_events_mouse_button_create_events();
	rde_inner_events_drag_and_drop_create_events();
	rde_inner_events_mobile_create_events();

	rde_inner_rendering_set_rendering_configuration(_default_window);

#ifdef RDE_PHYSICS_MODULE
	rde_jolt_init(_engine_init_info.jolt_config, rde_critical_error, rde_log_level_inner);
	rde_log_level(RDE_LOG_LEVEL_INFO, "Jolt loaded correctly");
#endif

	srand(time(NULL));

	_instantiated = true;

	return _default_window;
}

void rde_setup_initial_info(rde_end_user_mandatory_callbacks _end_user_callbacks) {
	ENGINE.mandatory_callbacks.on_update = _end_user_callbacks.on_update;
	ENGINE.mandatory_callbacks.on_fixed_update = _end_user_callbacks.on_fixed_update;
	ENGINE.mandatory_callbacks.on_late_update = _end_user_callbacks.on_late_update;
	ENGINE.mandatory_callbacks.on_render = _end_user_callbacks.on_render;
}

#define GET_VALUE_FROM_CONFIG_FILE(_type, _func) 																										\
	_type rde_inner_engine_get_config_file_##_type(const char** _config_file_lines, size_t _number_of_lines, const char* _key, size_t _default_value) {  \
		size_t _final_value = RDE_UINT_MAX;																											  \
		for(size_t _i = 0; _i < _number_of_lines; _i++) {																								\
			if(rde_util_string_contains_substring(_config_file_lines[_i], _key, false)) {																\
				char* _value = strrchr(_config_file_lines[_i], '=');																					 \
				char* _trimed = rde_util_string_trim(_value);																							\
				char* _end = NULL;																													   \
				_final_value = _func(_trimed + 1, &_end, 10);																							 \
				rde_critical_error(_trimed + 1 == _end || errno != 0, "Value '%s' could not be converted to "#_type"\n");								 \
			}																																			\
		}																																				\
																																						 \
		return _final_value == RDE_UINT_MAX ? _default_value : _final_value;																			 \
	}

GET_VALUE_FROM_CONFIG_FILE(size_t, strtol)
GET_VALUE_FROM_CONFIG_FILE(int, strtol)

#define GET_SIZET(_key, _value) _value = rde_inner_engine_get_config_file_size_t((const char**)_lines, _line_amount, _key, _value);
#define GET_INT(_key, _value) _value = rde_inner_engine_get_config_file_int((const char**)_lines, _line_amount, _key, _value);

rde_engine_init_info rde_engine_load_config(const char* _config_path) {
	rde_engine_init_info _init_info = RDE_DEFAULT_INIT_INFO;
	
	rde_file_handle* _handle = rde_file_open(_config_path, RDE_FILE_MODE_READ);
	char* _config_file = rde_file_read_full_file(_handle, NULL);
	
	char** _lines = NULL;
	size_t _line_amount = rde_util_string_split(_config_file, &_lines, '\n');
	
	GET_SIZET("max_amount_of_windows", _init_info.heap_allocs_config.max_amount_of_windows)
	GET_SIZET("max_amount_of_vertices_per_batch", _init_info.heap_allocs_config.max_amount_of_vertices_per_batch)
	GET_SIZET("max_amount_of_textures", _init_info.heap_allocs_config.max_amount_of_textures)
	GET_SIZET("max_amount_of_atlases", _init_info.heap_allocs_config.max_amount_of_atlases)
	GET_SIZET("max_amount_of_fonts", _init_info.heap_allocs_config.max_amount_of_fonts)
	GET_SIZET("max_amount_of_models", _init_info.heap_allocs_config.max_amount_of_models)
	GET_SIZET("max_amount_of_models_textures", _init_info.heap_allocs_config.max_amount_of_models_textures)
	
	GET_SIZET("max_amount_of_point_lights", _init_info.illumination_config.max_amount_of_point_lights)
	GET_SIZET("max_amount_of_spot_lights", _init_info.illumination_config.max_amount_of_spot_lights)

	#ifdef RDE_AUDIO_MODULE
	GET_SIZET("max_amount_of_sounds", _init_info.heap_allocs_config.max_amount_of_sounds)
	// GET_SIZET("max_number_of_musics", _init_info.heap_allocs_config.max_number_of_musics)
	#endif

	#ifdef RDE_PHYSICS_MODULE
	GET_SIZET("temp_allocator_bytes", _init_info.jolt_config.temp_allocator_size)
	GET_SIZET("max_amount_of_allowed_jobs", _init_info.jolt_config.max_amount_of_physics_jobs)
	GET_SIZET("max_amount_of_physics_barriers", _init_info.jolt_config.max_amount_of_physics_barriers)
	GET_INT("max_amount_of_threads", _init_info.jolt_config.max_amount_of_threads)
	GET_SIZET("max_amount_of_bodies", _init_info.jolt_config.max_amount_of_bodies)
	GET_SIZET("max_amount_of_body_mutexes", _init_info.jolt_config.max_amount_of_body_mutexes)
	GET_SIZET("max_amount_of_contact_constraints", _init_info.jolt_config.max_amount_of_contact_constraints)
	GET_SIZET("collision_steps_per_update", _init_info.jolt_config.collision_steps_per_update)
	#endif

	for(size_t _i = 0; _i < _line_amount; _i++) {
		free(_lines[_i]);
	}
	
	free(_lines);

	return _init_info;
}

void rde_engine_set_event_user_callback(rde_event_func _user_event_callback) {
	ENGINE.user_event_callback = _user_event_callback;
}

bool rde_engine_logs_supressed() {
	return ENGINE.supress_engine_logs;	
}

void rde_engine_supress_logs(bool _supress) {
	ENGINE.supress_engine_logs = _supress;
}

RDE_PLATFORM_TYPE_ rde_engine_get_platform() {
	return ENGINE.platform_type;
}

float rde_engine_get_fixed_delta() {
	return ENGINE.fixed_delta_time;
}

void rde_engine_set_fixed_delta(float _delta_time) {
	UNUSED(_delta_time)
	UNIMPLEMENTED()
}

void rde_engine_on_run() {

	rde_critical_error(ENGINE.mandatory_callbacks.on_update == NULL, RDE_ERROR_NULL_MANDATORY_CALLBACK, "rde_engine_user_on_update");
	rde_critical_error(ENGINE.mandatory_callbacks.on_fixed_update == NULL, RDE_ERROR_NULL_MANDATORY_CALLBACK, "rde_engine_user_on_fixed_update");
	rde_critical_error(ENGINE.mandatory_callbacks.on_late_update == NULL, RDE_ERROR_NULL_MANDATORY_CALLBACK, "rde_engine_user_on_late_upadte");
	rde_critical_error(ENGINE.mandatory_callbacks.on_render == NULL, RDE_ERROR_NULL_MANDATORY_CALLBACK, "rde_engine_user_on_render");

	#if IS_MOBILE()
	SDL_SetEventFilter(rde_events_mobile_consume_events_callback_wrapper, NULL);
	#endif

	rde_inner_rendering_init_2d();
	rde_inner_rendering_init_3d();

	while(ENGINE.running) {
		Uint64 _start = SDL_GetPerformanceCounter();
		ENGINE.fixed_time_step_accumulator += ENGINE.delta_time;

		rde_inner_engine_on_event();
		if (!ENGINE.running) return;

		if(ENGINE.pause_loop) {
			continue;
		}

		rde_inner_engine_on_update(ENGINE.delta_time);
		ENGINE.mandatory_callbacks.on_update(ENGINE.delta_time);

		while (ENGINE.fixed_time_step_accumulator >= ENGINE.fixed_delta_time) {
			ENGINE.fixed_time_step_accumulator -= ENGINE.fixed_delta_time;
#if defined(RDE_PHYSICS_MODULE) || defined(RDE_PHYSICS_2D_MODULE)
			rde_inner_engine_on_fixed_update(ENGINE.fixed_delta_time);
#endif
			ENGINE.mandatory_callbacks.on_fixed_update(ENGINE.fixed_delta_time);
		}

		rde_inner_engine_on_late_update(ENGINE.delta_time);
		ENGINE.mandatory_callbacks.on_late_update(ENGINE.delta_time);
		
		rde_engine_transform_update();

		for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_windows; _i++) {
			rde_window* _window = &ENGINE.windows[_i];

			if(_window->sdl_window == NULL) {
				continue;
			}

			rde_inner_engine_on_render(ENGINE.delta_time, _window);
			ENGINE.mandatory_callbacks.on_render(ENGINE.delta_time, _window);
			rde_inner_rendering_flush_to_default_render_texture(_window);

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

void rde_engine_show_message_box(RDE_LOG_LEVEL_ _level, const char* _title, const char* _content, rde_window* _window) {
	Uint32 _sdl_level = SDL_MESSAGEBOX_INFORMATION;

	switch(_level) {
		case RDE_LOG_LEVEL_INFO: _sdl_level = SDL_MESSAGEBOX_INFORMATION; break;
		case RDE_LOG_LEVEL_WARNING: _sdl_level = SDL_MESSAGEBOX_WARNING; break;
		case RDE_LOG_LEVEL_ERROR: _sdl_level = SDL_MESSAGEBOX_ERROR; break;
		default: {
			rde_log_level(RDE_LOG_LEVEL_WARNING, "%s", RDE_ERROR_MESSAGE_BOX);
		}
	}

	SDL_ShowSimpleMessageBox(_sdl_level, _title, _content, _window->sdl_window);
}

void rde_engine_destroy_engine() {
	rde_inner_rendering_end_2d();
	rde_inner_rendering_end_3d();
	rde_inner_rendering_destroy_current_antialiasing_config();
	rde_rendering_render_texture_destroy(DEFAULT_RENDER_TEXTURE);
	glDeleteBuffers(1, &DEFAULT_RENDER_TEXTURE->vbo);
	glDeleteVertexArrays(1, &DEFAULT_RENDER_TEXTURE->vao);
	
	if(ENGINE.skybox.opengl_texture_id != -1) {
		rde_rendering_skybox_unload(ENGINE.skybox.opengl_texture_id);
	}

	if(ENGINE.skybox.vao != RDE_UINT_MAX) {
		glDeleteBuffers(1, &ENGINE.skybox.vbo);
		glDeleteVertexArrays(1, &ENGINE.skybox.vao);
	}

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_atlases; _i++) {
		if(ENGINE.atlases[_i].texture == NULL) {
			continue;
		}

		rde_rendering_atlas_unload(&ENGINE.atlases[_i]);
	}
	free(ENGINE.atlases);

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_fonts; _i++) {
		if(ENGINE.fonts[_i].texture == NULL) {
			continue;
		}

		rde_rendering_font_unload(&ENGINE.fonts[_i]);
	}
	free(ENGINE.fonts);

	for (size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if (ENGINE.textures[_i].opengl_texture_id == -1) {
			continue;
		}

		if(ENGINE.textures[_i].pixels != NULL) {
			rde_rendering_memory_texture_destroy(&ENGINE.textures[_i]);
		} else {
			rde_rendering_texture_unload(&ENGINE.textures[_i]);
		}
	}
	free(ENGINE.textures);

	for (size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_models; _i++) {
		if (ENGINE.models[_i].mesh_array == NULL) {
			continue;
		}

		rde_rendering_model_unload(&ENGINE.models[_i]);
	}
	free(ENGINE.models);

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_shaders; _i++) {
		if(ENGINE.shaders[_i].compiled_program_id == -1) {
			continue;
		}

		rde_rendering_shader_unload(&ENGINE.shaders[_i]);
	}
	free(ENGINE.shaders);

	if(ENGINE.illumination.point_lights != NULL) {
		free(ENGINE.illumination.point_lights);
	}

	if(ENGINE.illumination.spot_lights != NULL) {
		free(ENGINE.illumination.spot_lights);
	}

#ifdef RDE_AUDIO_MODULE
	rde_audio_end();
	free(ENGINE.sounds);
#endif

#ifdef RDE_PHYSICS_MODULE
	rde_jolt_end();
#endif

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_windows; _i++) {
		if(ENGINE.windows[_i].sdl_window == NULL) {
			continue;
		}

		rde_window_destroy_window(&ENGINE.windows[_i]);
	}
	free(ENGINE.windows);

	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
	rde_log_level(RDE_LOG_LEVEL_INFO, "%s", "Exited cleanly");
}

void rde_engine_switch_window_display(rde_window* _window, size_t _new_display) {
	UNUSED(_window)
	UNUSED(_new_display)
	UNIMPLEMENTED();
}

rde_window* rde_engine_get_focused_window() {
	SDL_Window* _window = SDL_GetMouseFocus();

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_windows; _i++) {
		rde_window* _w = &ENGINE.windows[_i];
		
		if(_w->sdl_window == _window) {
			return _w;
		}
	}

	return NULL;
}

rde_transform* rde_engine_transform_load() {
	rde_critical_error(ENGINE.last_transform_used == RDE_MAX_TRANSFORMS - 1, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "Transforms", RDE_MAX_TRANSFORMS);
	
	rde_transform* _t = NULL;

	if(stbds_arrlen(ENGINE.free_transforms) > 0) {
		_t = &ENGINE.transforms[ENGINE.free_transforms[0]];
		stbds_arrdel(ENGINE.free_transforms, 0);

		if(ENGINE.last_transform_used < _t->index) {
			ENGINE.last_transform_used = _t->index;
		}
	}

	if(_t == NULL) {
		_t = &ENGINE.transforms[ENGINE.last_transform_used + 1];
		ENGINE.last_transform_used = _t->index;
	}

	_t->parent = -1;
	
	return _t;
}

rde_vec_3F  rde_engine_transform_get_position(rde_transform* _transform) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on get position");
	return _transform->position;
}

void rde_engine_transform_set_position(rde_transform* _transform, rde_vec_3F _position) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on set position");
	_transform->position = _position;
	_transform->dirty = true;
}

rde_vec_3F  rde_engine_transform_get_rotation_degs(rde_transform* _transform) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on get rotation");
	return _transform->rotation;
}

void rde_engine_transform_set_rotation(rde_transform* _transform, rde_vec_3F _rotation_degs) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on set rotation");
	_transform->rotation = _rotation_degs;
	_transform->dirty = true;
}

rde_vec_3F  rde_engine_transform_get_scale(rde_transform* _transform) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on get scale");
	return _transform->scale;
}

void rde_engine_transform_set_scale(rde_transform* _transform, rde_vec_3F _scale) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on set scale");
	_transform->scale = _scale;
	_transform->dirty = true;
}

rde_transform* rde_engine_trasnform_get_parent(rde_transform* _transform) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on get parent");
	if(_transform->parent == -1) {
		return NULL;
	}

	return &ENGINE.transforms[_transform->parent];
}

void rde_engine_transform_set_parent(rde_transform* _transform, rde_transform* _parent) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on get position");

	if(_parent != NULL) {
		_transform->parent = _parent->index;
		bool _has_child_already = false;
		for(size_t _i = 0; _i < stbds_arrlenu(_parent->children); _i++) {
			if(_parent->children[_i] == _transform->index) {
				_has_child_already = true;
				break;
			}
		}
	
		if(!_has_child_already) {
			stbds_arrput(_parent->children, _transform->index);
		}
	} else {
		rde_engine_transform_remove_transform_from_parent_children(_transform);
	}
}

size_t rde_engine_transform_get_children_count(rde_transform* _transform) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Transform on get children count");
	return stbds_arrlenu(_transform->children);
}

void rde_engine_transform_unload(rde_transform* _transform) {
	rde_critical_error(_transform == NULL, RDE_ERROR_NO_NULL_ALLOWED, "rde_engine_transform_unload -> transform");
	stbds_arrput(ENGINE.free_transforms, _transform->index);

	if(_transform == &ENGINE.transforms[ENGINE.last_transform_used]) {
		int _last_transform_used = ENGINE.last_transform_used;
		for(int _i = ENGINE.last_transform_used - 1; _i > 0; _i--) {
			rde_transform* _t =  &ENGINE.transforms[_i];
			if(_t->parent != RDE_INT_MIN) {
				ENGINE.last_transform_used = _i;
			}
		}
		if(_last_transform_used == ENGINE.last_transform_used) {
			ENGINE.last_transform_used = -1;
		}
	}
	rde_engine_transform_remove_transform_from_parent_children(_transform);
	glm_mat4_identity(ENGINE.world_transforms[_transform->index]);
	int _index = _transform->index;
	*_transform = rde_struct_create_transform();
	_transform->index = _index;
}

#if IS_MAC()
#pragma clang diagnostic pop
#endif




// ==============================================================================
// =							PUBLIC API - MATH					 		  =
// ==============================================================================

void rde_math_set_random_seed(long _seed) {
	srand(_seed);
}

int rde_math_get_random_int(int _min_included, int _max_included) {
	return (rand() % (_max_included - _min_included + 1)) + _min_included;
}

float rde_math_get_random_float(float _min_included, float _max_included) {
	float _scale = rand() / (float) RAND_MAX;
	return _min_included + _scale * (_max_included - _min_included);
}

rde_probability rde_math_get_probability(float _normalized_chance_to_happen) {
	UNUSED(_normalized_chance_to_happen)
	if(_normalized_chance_to_happen > 1.0f) _normalized_chance_to_happen = 1.0f;
	if(_normalized_chance_to_happen < 0.0f) _normalized_chance_to_happen = 0.0f;

	float _chance = 1.f - _normalized_chance_to_happen;
	float _left_probability = rde_math_get_random_float(0.0f, 1.0f);
	rde_probability _p;
	_p.probability_rolled = _left_probability;
	_p.happened = _chance;
	return _p;
}

void rde_math_convert_world_position_to_screen_coordinates_3d(rde_window* _window, rde_vec_3F* _vec) {
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	_vec->x = (_vec->x / (_window_size.x * 0.5f));
	_vec->y = (_vec->y / (_window_size.y * 0.5f));
}

void rde_math_convert_world_position_to_screen_coordinates_2d(rde_window* _window, rde_vec_2F* _vec) {
	rde_vec_3F _vec_ = { _vec->x, _vec->y, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(_window, &_vec_);
	_vec->x = _vec_.x;
	_vec->y = _vec_.y;
}

void rde_math_convert_world_size_to_screen_size(rde_window* _window, rde_vec_2F* _vec) {
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	_vec->x = (_vec->x / (_window_size.x));
	_vec->y = (_vec->y / (_window_size.y));
}

#define RDE_IMPLEMENT_CLAMP_FUNCS(_type) 								\
_type rde_math_clamp_##_type(_type _value, _type _min, _type _max) { 	\
	return (_value < _min) ? _min : (_value > _max) ? _max : _value; 	\
}

RDE_IMPLEMENT_CLAMP_FUNCS(int)
RDE_IMPLEMENT_CLAMP_FUNCS(uint)
RDE_IMPLEMENT_CLAMP_FUNCS(float)
RDE_IMPLEMENT_CLAMP_FUNCS(double)
RDE_IMPLEMENT_CLAMP_FUNCS(long)
RDE_IMPLEMENT_CLAMP_FUNCS(ulong)

float rde_math_radians_to_degrees(float _radians) {
	return _radians * (180.f / RDE_PI);
}

float rde_math_degrees_to_radians(float _degrees) {
	return _degrees * (RDE_PI / 180.0f);
}

rde_vec_3F rde_math_quaternion_to_euler_degrees(rde_quaternion _quaternion) {
	double _sinr_cosp = 2 * (_quaternion.w * _quaternion.x + _quaternion.y * _quaternion.z);
	double _cosr_cosp = 1 - 2 * (_quaternion.x * _quaternion.x + _quaternion.y * _quaternion.y);

	float t2 = 2.0f * (_quaternion.w * _quaternion.y - _quaternion.z * _quaternion.x);
	t2 = t2 > 1.0f ? 1.0f : t2;
	t2 = t2 < -1.0f ? -1.0f : t2;

	double _siny_cosp = 2 * (_quaternion.w * _quaternion.z + _quaternion.x * _quaternion.y);
	double _cosy_cosp = 1 - 2 * (_quaternion.y * _quaternion.y + _quaternion.z * _quaternion.z);

	return (rde_vec_3F) { 
		rde_math_radians_to_degrees(atan2(_sinr_cosp, _cosr_cosp)),
		rde_math_radians_to_degrees(asin(t2)),
		rde_math_radians_to_degrees(atan2(_siny_cosp, _cosy_cosp))
	};
}

rde_quaternion rde_math_euler_degrees_to_quaternion(rde_vec_3F _euler_degrees) {
	float _r_x = rde_math_degrees_to_radians(_euler_degrees.x);
	float _r_y = rde_math_degrees_to_radians(_euler_degrees.y);
	float _r_z = rde_math_degrees_to_radians(_euler_degrees.z);
	double _cx = cos(_r_x * 0.5);
	double _sx = sin(_r_x * 0.5);
	double _cy = cos(_r_y * 0.5);
	double _sy = sin(_r_y * 0.5);
	double _cz = cos(_r_z * 0.5);
	double _sz = sin(_r_z * 0.5);

	return (rde_quaternion) {
		_cz * _sx * _cy - _sz * _cx * _sy,
		_cz * _cx * _sy + _sz * _sx * _cy,
		_sz * _cx * _cy - _cz * _sx * _sy,
		_cz * _cx * _cy + _sz * _sx * _sy 
	};
}


float rde_math_easing_in_linear(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return _change_in_value * _current_time / _duration + _start_value;
}

float rde_math_easing_in_quadratic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return _change_in_value * _current_time * _current_time + _start_value;
}

float rde_math_easing_out_quadratic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return -_change_in_value * _current_time * (_current_time - 2) + _start_value;
}

float rde_math_easing_in_out_quadratic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1) return _change_in_value / 2 * _current_time * _current_time + _start_value;
	_current_time--;
	return -_change_in_value / 2 * (_current_time * (_current_time - 2) - 1) + _start_value;
}

float rde_math_easing_in_cubic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return _change_in_value * _current_time * _current_time * _current_time + _start_value;
}

float rde_math_easing_out_cubic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	_current_time--;
	return _change_in_value * (_current_time * _current_time * _current_time + 1) + _start_value;
}

float rde_math_easing_in_out_cubic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1) return _change_in_value / 2 * _current_time * _current_time * _current_time + _start_value;
	_current_time -= 2;
	return _change_in_value / 2 * (_current_time * _current_time * _current_time + 2) + _start_value;
}

float rde_math_easing_in_quartic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return _change_in_value * _current_time * _current_time * _current_time * _current_time + _start_value;
}

float rde_math_easing_out_quartic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	_current_time--;
	return -_change_in_value * (_current_time * _current_time * _current_time * _current_time - 1) + _start_value;
}

float rde_math_easing_in_out_quartic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1)
		return _change_in_value / 2 * _current_time * _current_time * _current_time * _current_time + _start_value;
	_current_time -= 2;
	return -_change_in_value / 2 * (_current_time * _current_time * _current_time * _current_time - 2) + -_start_value;
}

float rde_math_easing_in_quintic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return _change_in_value * _current_time * _current_time * _current_time * _current_time * _current_time + _start_value;
}

float rde_math_easing_out_quintic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	_current_time--;
	return _change_in_value * (_current_time * _current_time * _current_time * _current_time * _current_time + 1) + _start_value;
}

float rde_math_easing_in_out_quintic(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1)
		return _change_in_value / 2 * _current_time * _current_time * _current_time * _current_time * _current_time + _start_value;
	_current_time -= 2;
	return _change_in_value / 2 * (_current_time * _current_time * _current_time * _current_time * _current_time + 2) + _start_value;
}

float rde_math_easing_in_sine(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return -_change_in_value * (float) cos(_current_time / _duration * (RDE_PI / 2)) + _change_in_value + _start_value;
}

float rde_math_easing_out_sine(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return _change_in_value * (float) sin(_current_time / _duration * (RDE_PI / 2)) + _start_value;
}

float rde_math_easing_in_out_sine(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return -_change_in_value / 2 * ((float) cos(RDE_PI * _current_time / _duration) - 1) + _start_value;
}

float rde_math_easing_in_exponential(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return _change_in_value * (float) pow(2, 10 * (_current_time / _duration - 1)) + _start_value;
}

float rde_math_easing_out_exponential(float _current_time, float _start_value, float _change_in_value, float _duration) {
	return _change_in_value * (-(float) pow(2, -10 * _current_time / _duration) + 1) + _start_value;
}

float rde_math_easing_in_out_exponential(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1) return _change_in_value / 2 * (float) pow(2, 10 * (_current_time - 1)) + _start_value;
	_current_time--;
	return _change_in_value / 2 * (-(float) pow(2, -10 * _current_time) + 2) + _start_value;
}

float rde_math_easing_in_circular(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	return -_change_in_value * ((float) sqrt(1 - _current_time * _current_time) - 1) + _start_value;
}

float rde_math_easing_out_circular(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration;
	_current_time--;
	return _change_in_value * (float) sqrt(1 - _current_time * _current_time) + _start_value;
}

float rde_math_easing_in_out_circular(float _current_time, float _start_value, float _change_in_value, float _duration) {
	_current_time /= _duration / 2;
	if (_current_time < 1)
		return -_change_in_value / 2 * ((float) sqrt(1 - _current_time * _current_time) - 1) + _start_value;
	_current_time -= 2;
	return _change_in_value / 2 * ((float) sqrt(1 - _current_time * _current_time) + 1) + _start_value;
}

rde_vec_3F rde_math_cross_product(rde_vec_3F _vec_0, rde_vec_3F _vec_1) {
	vec3 _cp = GLM_VEC3_ZERO;
	glm_cross((vec3) { _vec_0.x, _vec_0.y, _vec_0.z }, (vec3) { _vec_1.x, _vec_1.y, _vec_1.z }, _cp);
	return (rde_vec_3F) { _cp[0], _cp[1], _cp[2] };
}

void rde_math_normalize(rde_vec_3F* _vec) {
	vec3 _norm = (vec3) { _vec->x, _vec->y, _vec->z };
	glm_normalize(_norm);
	_vec->x = _norm[0];
	_vec->y = _norm[1];
	_vec->z = _norm[2];
}


// ==============================================================================
// =							PRIVATE API - UTIL					 	     =
// ==============================================================================

// replace substring is from https://github.com/ipserc/strrep/blob/master/string_replace/Source/strrep.c
char* rde_inner_append_str(char* _string, const char* _append) {
	char* _new_string = malloc(strlen(_string) + strlen(_append) + 1);

	sprintf(_new_string, "%s%s", _string, _append);
	free(_string);
	return _new_string;
}

char* rde_inner_strtokk(char* _string, const char* _strf) {
	static char* _ptr;
	static char* _ptr2;

	if (!*_strf) return _string;
	if (_string) _ptr = _string;
	else {
		if (!_ptr2) return _ptr2;
		_ptr = _ptr2 + strlen(_strf);
	}

	if (_ptr) {
		_ptr2 = strstr(_ptr, _strf);
		if (_ptr2) memset(_ptr2, 0, strlen(_strf));
	}
	return _ptr;
}


// ==============================================================================
// =							PUBLIC API - UTIL					 	      =
// ==============================================================================

void rde_util_file_sanitaize_path(const char* _path, char* _sanitized, size_t _sanitized_size) {
	rde_critical_error(strlen(_path) > _sanitized_size, RDE_ERROR_SANITIZATION_PATH, _path, _sanitized_size);
	memset(_sanitized, 0, _sanitized_size);

	char _fucking_windows_backslash = '\\';
	char _lovely_unix_slash = '/';
	
	for(size_t _i = 0; _i < strlen(_path); _i++) {
		_sanitized[_i] = _path[_i];
		if(_sanitized[_i] == _fucking_windows_backslash) {
			_sanitized[_i] = _lovely_unix_slash;
		}
	}
}

const char* rde_util_file_get_name_extension(const char* _file_name) {
	const char* _dot = strrchr(_file_name, '.');
	if(!_dot || _dot == _file_name) return "";
	return _dot + 1;
}

size_t rde_util_font_get_string_width(const char* _string, const rde_font* _font) {
	int _text_size = strlen(_string);

	size_t _size = 0;
	for (int _i = 0; _i < _text_size; _i++) {
		int _key = (int)_string[_i];
		rde_font_char_info _char_info = _font->chars[_key];
		_size += (_char_info.advance.x >> 6); // /64.f
	}

	return _size;
}

rde_vec_2I rde_util_font_get_string_size(const char* _string, const rde_font* _font) {
	int _text_size = strlen(_string);

	int _width = 0;
	int _height = 0;
	for (int _i = 0; _i < _text_size; _i++) {
		int _key = (int)_string[_i];
		rde_font_char_info _char_info = _font->chars[_key];
		_width += (_char_info.advance.x >> 6);

		if(_char_info.size.y > _height) {
			_height = _char_info.size.y;
		}
	}

	return (rde_vec_2I) { _width, _height };
}

char* rde_util_string_trim(char* _s) {
	char* _end;

	while(isspace((unsigned char)*_s)) _s++;

	if(*_s == 0) {
		return _s;
	}

	_end = _s + strlen(_s) - 1;
	while(_end > _s && isspace((unsigned char)*_end)) _end--;

	_end[1] = '\0';

	return _s;
}

bool rde_util_string_starts_with(const char* _string, const char* _prefix) {
	if(_string == NULL || _prefix == NULL) {
		return false;
	}
	return strncmp(_string, _prefix, strlen(_prefix)) == 0;
}

bool rde_util_string_ends_with(const char* _string, const char* _suffix) {
	if(_string == NULL || _suffix == NULL) {
		return false;
	}

	size_t _suffix_size = strlen(_suffix);
	size_t _string_size = strlen(_string);
	return strncmp(_string + (_string_size - _suffix_size), _suffix, _suffix_size) == 0;
}

bool rde_util_string_contains_substring(const char* _string, const char* _substring, bool _case_sensitive) {
	if(_string == NULL || _substring == NULL) {
		return false;
	}

	if(_case_sensitive) {
		return strstr(_string, _substring) != NULL;
	} else {
#if IS_WINDOWS()
		return StrStrI(_string, _substring) != NULL;
#else
		return strcasestr(_string, _substring) != NULL;
#endif
	}
	return false;
}

size_t rde_util_string_char_appearences(const char* _string, char _char) {
	if(_string == NULL) {
		return 0;
	}

	size_t _amount = 0;
	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++) {
		if(_string[_i] == _char) {
			_amount++;
		}
	}

	return _amount;
}

void rde_util_string_concat(char* _string, size_t _size, const char* _fmt, ...) {
	if(_string == NULL) {
		return;
	}

	va_list _args;
	va_start(_args, _fmt);
	vsnprintf(_string, _size, _fmt, _args);
	va_end(_args);
}

void rde_util_string_to_lower(char* _destination, const char* _string) {
	if(_string == NULL || _destination == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++){
		_destination[_i] = tolower(_string[_i]);
	}
}

void rde_util_string_to_lower_itself(char* _string) {
	if(_string == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++){
		_string[_i] = tolower(_string[_i]);
	}
}

void rde_util_string_to_upper(char* _destination, const char* _string) {
	if(_string == NULL || _destination == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++){
		_destination[_i] = toupper(_string[_i]);
	}
}

void rde_util_string_to_upper_itself(char* _string) {
	if(_string == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++){
		_string[_i] = toupper(_string[_i]);
	}
}

void rde_util_string_replace_char(char* _string, char _old, char _new) {
	if(_string == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++) {
		if(_string[_i] == _old) {
			_string[_i] = _new;
			break;
		}
	}
}

char* rde_util_string_replace_substring(char* _string, char* _old_string, char* _new_string, int* _output_appearences) {
	char* _str;
	char* _ptr;
	char* _strrep;

	_str = (char *)malloc(strlen(_string) + 1);
	sprintf(_str, "%s", _string);
	if (!*_old_string) return _str;
	_ptr = rde_inner_strtokk(_str, _old_string);
	_strrep = malloc(strlen(_ptr) + 1);
	memset(_strrep, 0, strlen(_ptr) + 1);
	while (_ptr) {
		_strrep = rde_inner_append_str(_strrep, _ptr);
		_ptr = rde_inner_strtokk(NULL, _old_string);
		if (_ptr) _strrep = rde_inner_append_str(_strrep, _new_string);
		if(_ptr && _output_appearences != NULL) {
			(*_output_appearences)++;
		}
	}

	free(_str);
	return _strrep;
}

void rde_util_string_replace_chars_all(char* _string, char _old, char _new) {
	if(_string == NULL) {
		return;
	}

	size_t _string_size = strlen(_string);
	for(size_t _i = 0; _i < _string_size; _i++) {
		if(_string[_i] == _old) {
			_string[_i] = _new;
		}
	}
}

size_t rde_util_string_split(char* _string, char*** _split_array, char _split_mark) {
	if(_string == NULL || _split_array == NULL) {
		return 0;
	}

	int* _tklen = NULL; 
	int* _t = NULL;
	int _count = 1;
    char** _arr;
	char* _p = (char*) _string;

    while (*_p != '\0') {
		if (*_p++ == _split_mark) {
			_count += 1;
		}
	}
	
    _t = _tklen = calloc(_count, sizeof(int));

    for (_p = (char*)_string; *_p != '\0'; _p++) {
		*_p == _split_mark ? *_t++ : (*_t)++;
	}
    
	*_split_array = _arr = malloc(_count * sizeof(char*));
    _t = _tklen;
    _p = *_arr++ = calloc(*(_t++) + 1, sizeof(char*));
    
	while (*_string != '\0') {
        if (*_string == _split_mark) {
            _p = *_arr++ = calloc (*(_t++) + 1, sizeof(char*));
            _string++;
        }
        else {
			*_p++ = *_string++;
		}
    }
	
    free (_tklen);
    return _count;
}

void rde_log_color_inner(RDE_LOG_COLOR_ _color, const char* _fmt, ...) {
	switch(_color) {
		case RDE_LOG_COLOR_RED: {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 12);
#else
			fprintf(stdout, "\033[0;31m");
#endif
		} break;
		
		case RDE_LOG_COLOR_GREEN:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 10);
#else
			fprintf(stdout, "\033[0;32m");	
#endif
		} break;
		
		case RDE_LOG_COLOR_YELLOW:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 6);
#else
			fprintf(stdout, "\033[0;33m");	
#endif
		} break;
		
		case RDE_LOG_COLOR_BLUE:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 9);
#else
			fprintf(stdout, "\033[0;34m");		
#endif
		} break;
		
		case RDE_LOG_COLOR_PURPLE:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 5);
#else
			fprintf(stdout, "\033[0;35m");
#endif
		} break;
		
		case RDE_LOG_COLOR_CYAN:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 11);
#else
			fprintf(stdout, "\033[0;36m");
#endif
		} break;
		
		case RDE_LOG_COLOR_WHITE:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 15);
#else
			fprintf(stdout, "\033[0;37m");
#endif
		} break;
	}

	va_list _args;
	va_start(_args, _fmt);
	vfprintf(stdout, _fmt, _args);
	va_end(_args);

#if IS_WINDOWS()
	SetConsoleTextAttribute(ENGINE.console_handle, 7);
#else
	fprintf(stdout, "\033[0m");
#endif
}

void rde_log_level_inner(RDE_LOG_LEVEL_ _level, const char* _fmt, ...) {
	switch(_level) {
		case RDE_LOG_LEVEL_ERROR: {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 12);
#else
			fprintf(stdout, "\033[0;31m");
#endif
			fprintf(stdout, "[ERROR] ");
		} break;
		
		case RDE_LOG_LEVEL_WARNING:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 6);
#else
			fprintf(stdout, "\033[0;33m");	
#endif
			fprintf(stdout, "[WARNING] ");
		} break;
		
		case RDE_LOG_LEVEL_DEBUG:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 11);
#else
			fprintf(stdout, "\033[0;36m");
#endif
			fprintf(stdout, "[DEBUG] ");
		} break;

		case RDE_LOG_LEVEL_INFO:  {
#if IS_WINDOWS()
			SetConsoleTextAttribute(ENGINE.console_handle, 7);
#endif
			fprintf(stdout, "[INFO] ");
		} break;
	}

	va_list _args;
	va_start(_args, _fmt);
	vfprintf(stdout, _fmt, _args);
	va_end(_args);
	
#if IS_WINDOWS()
	SetConsoleTextAttribute(ENGINE.console_handle, 7);
#else
	fprintf(stdout, "\033[0m");
#endif
}




// ==============================================================================
// =							PRIVATE API - FILE SYSTEM					   =
// ==============================================================================

rde_atlas_sub_textures* rde_inner_file_system_read_atlas_config(const char* _atlas_path, rde_texture* _atlas);
rde_font_char_info* rde_inner_file_system_read_font_config(const char* _font_path, rde_texture* _atlas);

rde_atlas_sub_textures* rde_inner_file_system_read_atlas_config(const char* _atlas_path, rde_texture* _atlas) {
	FILE* _file = NULL;
	char* _text = NULL;

	#if IS_WINDOWS()
		errno_t _err = fopen_s(&_file, _atlas_path, "r");
		rde_critical_error(_err != 0, RDE_ERROR_FILE_NOT_FOUND, _atlas_path);
	#else
		_file = fopen(_atlas_path, "r");
		rde_critical_error(_file == NULL, RDE_ERROR_FILE_NOT_FOUND, _atlas_path);
	#endif

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	rde_critical_error(_text == NULL, RDE_ERROR_NO_MEMORY, _num_bytes, "config file");
	fread(_text, sizeof(char), _num_bytes, _file);


	rde_atlas_sub_textures* hash = NULL;
	cJSON* _atlas_json = cJSON_Parse(_text);

	if(_atlas_json == NULL) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		rde_critical_error(true, RDE_ERROR_JSON, _atlas_path, _error_ptr);
	}

	cJSON* _sub_texture_json = NULL;
	cJSON_ArrayForEach(_sub_texture_json, _atlas_json) {
		cJSON* _position = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "position");
		cJSON* _size = cJSON_GetObjectItemCaseSensitive(_sub_texture_json, "size");

		rde_texture _texture;
		_texture.opengl_texture_id = _atlas->opengl_texture_id;
		_texture.size.x = (uint)cJSON_GetArrayItem(_size, 0)->valueint;
		_texture.size.y = (uint)cJSON_GetArrayItem(_size, 1)->valueint;
		_texture.position.x = (uint)cJSON_GetArrayItem(_position, 0)->valueint;
		_texture.position.y = (uint)cJSON_GetArrayItem(_position, 1)->valueint;
		_texture.channels = _atlas->channels;
		_texture.internal_format = _atlas->internal_format;
		_texture.data_format = _atlas->data_format;
		rde_strcpy(_texture.file_path, RDE_MAX_PATH, _atlas->file_path);
		_texture.atlas_texture = _atlas;
		stbds_shput(hash, _sub_texture_json->string, _texture);

		cJSON_free(_position);
		cJSON_free(_size);
	}

	cJSON_free(_atlas_json);

	fclose(_file);
	free(_text);

	return hash;
}

rde_font_char_info* rde_inner_file_system_read_font_config(const char* _font_path, rde_texture* _atlas) {
	FILE* _file = NULL;
	char* _text = NULL;

	#if IS_WINDOWS()
	errno_t _err = fopen_s(&_file, _font_path, "r");
	rde_critical_error(_err != 0, RDE_ERROR_FILE_NOT_FOUND, _font_path);
	#else
	_file = fopen(_font_path, "r");
	rde_critical_error(_file == NULL, RDE_ERROR_FILE_NOT_FOUND, _font_path);
	#endif

	long _num_bytes = 0;
	fseek(_file, 0L, SEEK_END);
	_num_bytes = ftell(_file);
	fseek(_file, 0L, SEEK_SET);

	_text = (char*)calloc(_num_bytes, sizeof(char));
	rde_critical_error(_text == NULL, RDE_ERROR_NO_MEMORY, _num_bytes, "config file");
	fread(_text, sizeof(char), _num_bytes, _file);


	rde_font_char_info* _chars = NULL;
	cJSON* _font_json = cJSON_Parse(_text);

	if(_font_json == NULL) {
		const char* _error_ptr = cJSON_GetErrorPtr();
		rde_critical_error(true, RDE_ERROR_JSON, _font_path, _error_ptr);
	}

	cJSON* _char_info_json = NULL;
	rde_font_char_info _fci_default = rde_struct_create_font_char_info();
	cJSON* _extra_data = cJSON_GetObjectItemCaseSensitive(_font_json, "extra_data");
	cJSON* _offset_from_start = cJSON_GetObjectItemCaseSensitive(_extra_data, "offset_from_start");
	for(int _i = 0; _i < _offset_from_start->valueint; _i++) {
		stbds_arrput(_chars, _fci_default);
	}

	cJSON* _characters = cJSON_GetObjectItemCaseSensitive(_font_json, "characters");

	cJSON_ArrayForEach(_char_info_json, _characters) {
		cJSON* _advance = cJSON_GetObjectItemCaseSensitive(_char_info_json, "advance");
		cJSON* _size = cJSON_GetObjectItemCaseSensitive(_char_info_json, "size");
		cJSON* _bearing = cJSON_GetObjectItemCaseSensitive(_char_info_json, "bearing");
		cJSON* _offset = cJSON_GetObjectItemCaseSensitive(_char_info_json, "offset");
		cJSON* _metrics = cJSON_GetObjectItemCaseSensitive(_char_info_json, "metrics");

		rde_font_char_info _char_info = rde_struct_create_font_char_info();
		_char_info.advance = (rde_vec_2I) { cJSON_GetArrayItem(_advance, 0)->valueint, cJSON_GetArrayItem(_advance, 1)->valueint };
		_char_info.size = (rde_vec_2I) { cJSON_GetArrayItem(_size, 0)->valueint, cJSON_GetArrayItem(_size, 1)->valueint };
		_char_info.bearing = (rde_vec_2I) { cJSON_GetArrayItem(_bearing, 0)->valueint, cJSON_GetArrayItem(_bearing, 1)->valueint };
		_char_info.offset = (rde_vec_2I) { cJSON_GetArrayItem(_offset, 0)->valueint, cJSON_GetArrayItem(_offset, 1)->valueint };
		_char_info.metrics = (rde_vec_2I) { cJSON_GetArrayItem(_metrics, 0)->valueint, cJSON_GetArrayItem(_metrics, 1)->valueint };

		rde_texture _texture;
		_texture.opengl_texture_id = _atlas->opengl_texture_id;
		_texture.size.x = _char_info.size.x;
		_texture.size.y = _char_info.size.y;
		_texture.position.x = _char_info.offset.x;
		_texture.position.y = _char_info.offset.y;
		_texture.channels = _atlas->channels;
		_texture.internal_format = _atlas->internal_format;
		_texture.data_format = _atlas->data_format;
		rde_strcpy(_texture.file_path, RDE_MAX_PATH, _atlas->file_path);
		_texture.atlas_texture = _atlas;
		_char_info.texture = _texture;

		stbds_arrput(_chars, _char_info);

		cJSON_free(_advance);
		cJSON_free(_size);
		cJSON_free(_bearing);
		cJSON_free(_offset);
	}

	cJSON_free(_font_json);

	fclose(_file);
	free(_text);

	return _chars;
}

const char* rde_inner_file_system_file_mode_to_char(const RDE_FILE_MODE_ _mode) {
	switch (_mode) {
		case RDE_FILE_MODE_READ: return "rb";
		case RDE_FILE_MODE_WRITE: return "w";
		case RDE_FILE_MODE_APPEND: return "a";
		case RDE_FILE_MODE_READ_AND_WRITE: return "r+";
		case RDE_FILE_MODE_READ_AND_APPEND: return "a+";
	}

	return "rb";
}

void rde_inner_file_system_free_text_allocation(rde_file_handle* _handler) {
	if(_handler->text_allocated != NULL) {
		free(_handler->text_allocated);
		_handler->text_allocated = NULL;
	}

	if(_handler->bytes_allocated != NULL) {
		free(_handler->bytes_allocated);
		_handler->bytes_allocated = NULL;
	}
}

void rde_inner_file_system_check_file_mode_or_convert(rde_file_handle* _handler, RDE_FILE_MODE_ _expected) {
	if(_handler->file_mode == _expected) return;
	SDL_RWclose(_handler->sdl_handle);
	rde_inner_file_system_free_text_allocation(_handler);
	_handler->sdl_handle = SDL_RWFromFile(_handler->file_path, rde_inner_file_system_file_mode_to_char(_expected));
	_handler->file_mode = _expected;
}


// ==============================================================================
// =							PUBLIC API - FILE SYSTEM					    =
// ==============================================================================


rde_file_handle* rde_file_open(const char* _file_path, RDE_FILE_MODE_ _file_mode) {
	const char* _mode = rde_inner_file_system_file_mode_to_char(_file_mode);
	SDL_RWops* _sdl_file = SDL_RWFromFile(_file_path, _mode);
	rde_critical_error(_sdl_file == NULL, RDE_ERROR_FILE_NOT_FOUND, _file_path);

	for(size_t _i = 0; _i < RDE_MAX_CONCURRENT_FILES_OPENED; _i++) {
		rde_file_handle* _file_handler = &concurrent_file_handlers[_i];

		if(_file_handler->sdl_handle == NULL) {
			_file_handler->sdl_handle = _sdl_file;
			_file_handler->file_mode = _file_mode;
			memset(_file_handler->file_path, 0, RDE_MAX_PATH);
			rde_strcat(_file_handler->file_path, RDE_MAX_PATH, _file_path);
			_file_handler->text_allocated = NULL;
			_file_handler->bytes_allocated = NULL;

			return _file_handler;
		}
	}

	rde_critical_error(true, RDE_ERROR_FILE_SYSTEM_MAX_OPENED_FILES_REACHED, RDE_MAX_CONCURRENT_FILES_OPENED);
	return NULL;
}

char* rde_file_read_full_file(rde_file_handle* _file_handler, size_t* _output_file_size) {
	rde_inner_file_system_check_file_mode_or_convert(_file_handler, RDE_FILE_MODE_READ);
	size_t _total_size = SDL_RWsize(_file_handler->sdl_handle);
	size_t _total_bytes_read = 0;
	size_t _bytes_to_read = 1;
	char* _text = (char*)malloc(sizeof(char) * _total_size + 1);
	char* _buf = _text;
	memset(_text, 0, _total_size);
	rde_critical_error(_text == NULL, RDE_ERROR_NO_MEMORY, sizeof(char) * _total_size, "Text Read Full File");

	while (_total_bytes_read < _total_size && _bytes_to_read != 0) {
		_bytes_to_read = SDL_RWread(_file_handler->sdl_handle, _buf, 1, (_total_size - _total_bytes_read));
		_total_bytes_read += _bytes_to_read;
		_buf += _bytes_to_read;
	}
	_text[_total_bytes_read] = '\0';

	rde_inner_file_system_free_text_allocation(_file_handler);
	_file_handler->text_allocated = _text;
	
	if(_output_file_size != NULL) {
		*_output_file_size = _total_size;
	}

	return _text;
}

void rde_file_free_read_text(rde_file_handle* _file_handle) {
	rde_critical_error(_file_handle == NULL, RDE_ERROR_NO_NULL_ALLOWED, "File Handle");
	if(_file_handle->text_allocated != NULL) {
		free(_file_handle->text_allocated);
		_file_handle->text_allocated = NULL;
	}
}

unsigned char* rde_file_read_full_file_bytes(rde_file_handle* _file_handler, size_t* _output_file_size) {
	rde_inner_file_system_check_file_mode_or_convert(_file_handler, RDE_FILE_MODE_READ);
	size_t _total_size = SDL_RWsize(_file_handler->sdl_handle);
	size_t _total_bytes_read = 0;
	size_t _bytes_to_read = 1;
	unsigned char* _text = (unsigned char*)malloc(sizeof(unsigned char) * _total_size + 1);
	unsigned char* _buf = _text;
	memset(_text, 0, _total_size);
	rde_critical_error(_text == NULL, RDE_ERROR_NO_MEMORY, sizeof(unsigned char) * _total_size, "Text Read Full File");

	while (_total_bytes_read < _total_size && _bytes_to_read != 0) {
		_bytes_to_read = SDL_RWread(_file_handler->sdl_handle, _buf, 1, (_total_size - _total_bytes_read));
		_total_bytes_read += _bytes_to_read;
		_buf += _bytes_to_read;
	}
	_text[_total_bytes_read] = '\0';

	rde_inner_file_system_free_text_allocation(_file_handler);
	_file_handler->bytes_allocated = _text;
	
	if(_output_file_size != NULL) {
		*_output_file_size = _total_size;
	}

	return _text;
}

void rde_file_free_read_bytes(rde_file_handle* _file_handle) {
	rde_critical_error(_file_handle == NULL, RDE_ERROR_NO_NULL_ALLOWED, "File Handle");
	if(_file_handle->bytes_allocated != NULL) {
		free(_file_handle->bytes_allocated);
		_file_handle->bytes_allocated = NULL;
	}
}

char* rde_file_read_line(rde_file_handle* _file_handler, size_t _line) {
	size_t _current_line = 0;
	size_t _final_line_ptr = 0;
	bool _line_found = false;
	size_t _line_first_byte = RDE_UINT_MAX;

	rde_inner_file_system_check_file_mode_or_convert(_file_handler, RDE_FILE_MODE_READ);
	SDL_RWseek(_file_handler->sdl_handle, 0, RW_SEEK_END);
	long _content_size = SDL_RWtell(_file_handler->sdl_handle);
	char* _file_content = (char*)malloc(sizeof(char) * _content_size);
	SDL_RWseek(_file_handler->sdl_handle, 0, RW_SEEK_SET);

	for(long _i = 0; _i < _content_size; _i++) {
		SDL_RWread(_file_handler->sdl_handle, &_file_content[_i], sizeof(char), 1);
		if(_current_line == _line) {
			
			if(_line_first_byte == RDE_UINT_MAX) {
				_line_first_byte = _i;
			}

			_line_found = true;
			if(_file_content[_i] == '\n' || (_file_content[_i] == '\r' && _file_content[_i + 1] == '\n')) break;
			_final_line_ptr++;
			continue;
		}

		if(_file_content[_i] == '\n') _current_line++;
	}

	if(!_line_found) {
		return NULL;
	}

	char* _line_ptr = (char*)malloc(sizeof(char) * _final_line_ptr);
	memset(_line_ptr, 0, _final_line_ptr);
#if IS_WINDOWS()
	strncpy_s(_line_ptr, _final_line_ptr, _file_content, _final_line_ptr);
#else
	strncpy(_line_ptr, _file_content, _final_line_ptr);
#endif

	rde_inner_file_system_free_text_allocation(_file_handler);
	_file_handler->text_allocated = _line_ptr;
	return _line_ptr;
}

char* rde_file_read_chunk(rde_file_handle* _file_handler, size_t _begin_byte, size_t _end_byte) {
	UNUSED(_file_handler)
	UNUSED(_begin_byte)
	UNUSED(_end_byte)
	UNIMPLEMENTED()
	return NULL;
}

void rde_file_write(rde_file_handle* _file_handler, size_t _bytes, const char* _data) {
	UNUSED(_file_handler)
	UNUSED(_bytes)
	UNUSED(_data)
	UNIMPLEMENTED()
}

void rde_file_write_to_line(rde_file_handle* _file_handler, size_t _bytes, const char* _data, size_t _line) {
	UNUSED(_file_handler)
	UNUSED(_bytes)
	UNUSED(_data)
	UNUSED(_line)
	UNIMPLEMENTED()
}

void rde_file_append(rde_file_handle* _file_handler, size_t _append_byte, size_t _bytes, const char* _data, size_t _line) {
	UNUSED(_file_handler)
	UNUSED(_append_byte)
	UNUSED(_bytes)
	UNUSED(_data)
	UNUSED(_line)
	UNIMPLEMENTED()
}

void rde_file_clear_content(rde_file_handle* _file_handler) {
	UNUSED(_file_handler)
	UNIMPLEMENTED()
}

bool rde_file_exists(const char* _file_path) {
	UNUSED(_file_path)
	UNIMPLEMENTED()
	return false;
}

void rde_file_delete(const char* _file_path) {
	UNUSED(_file_path)
	UNIMPLEMENTED()
}

void rde_file_move(const char* _file_path, const char* _new_file_path) {
	UNUSED(_file_path)
	UNUSED(_new_file_path)
	UNIMPLEMENTED()
}

void rde_file_close(rde_file_handle* _file_handler) {
	rde_inner_file_system_free_text_allocation(_file_handler);
	SDL_RWclose(_file_handler->sdl_handle);
	_file_handler->sdl_handle = NULL;
}




// ==============================================================================
// =							PRIVATE API - WINDOW					 	   =
// ==============================================================================

#if IS_WINDOWS()
rde_window* rde_inner_window_create_windows_window(size_t _free_window_index) {
	ENGINE.windows[_free_window_index] = rde_struct_create_window();
	rde_window* _window = &ENGINE.windows[_free_window_index];

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Use this to allow trackpad to use touch gestures
	//SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "1");

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);

	_window->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	rde_critical_error(_window->sdl_window == NULL, RDE_ERROR_SDL_WINDOW, SDL_GetError());

	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);
	rde_critical_error(_window->sdl_gl_context == NULL, RDE_ERROR_SDL_OPENGL, SDL_GetError());

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

	rde_critical_error(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), RDE_ERROR_GLAD_INIT);
	
	rde_log_level(RDE_LOG_LEVEL_INFO, "GLAD and SDL2 loaded successfully for windows");

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	return _window;
}
#endif

#if IS_MAC()
rde_window* rde_inner_window_create_mac_window(size_t _free_window_index) {
	ENGINE.windows[_free_window_index] = rde_struct_create_window();
	rde_window* _window = &ENGINE.windows[_free_window_index];

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);
	_window->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	rde_critical_error(_window->sdl_window == NULL, RDE_ERROR_SDL_WINDOW, SDL_GetError());

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	
	// Use this to allow trackpad to use touch gestures
	//SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "1");

	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);
	rde_critical_error(_window->sdl_gl_context == NULL, RDE_ERROR_SDL_OPENGL, SDL_GetError());

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);
	
	rde_log_level(RDE_LOG_LEVEL_INFO, "GLAD and SDL2 loaded successfully for mac");

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	int _major = 0;
	if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &_major) != 0) {
		exit(-1);
	}
	printf("Major: %d\n", _major);

	int _minor = 0;
	if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &_minor) != 0) {
		exit(-1);
	}
	printf("Minor: %d\n", _minor);

	int _context = 0;
	if(SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &_context) != 0) {
		exit(-1);
	}
	printf("Context: %d\n", _context);
	return _window;
}
#endif

#if IS_LINUX() && !IS_ANDROID()
rde_window* rde_inner_window_create_linux_window(size_t _free_window_index) {
	ENGINE.windows[_free_window_index] = rde_struct_create_window();
	rde_window* _window = &ENGINE.windows[_free_window_index];

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Use this to allow trackpad to use touch gestures
	//SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "1");

	char _title[16];
	snprintf(_title, 10, "%d", (int)_free_window_index);
	_window->sdl_window = SDL_CreateWindow(_title, 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	rde_critical_error(_window->sdl_window == NULL, RDE_ERROR_SDL_WINDOW, SDL_GetError());

	_window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);
	rde_critical_error(_window->sdl_gl_context == NULL, RDE_ERROR_SDL_OPENGL, SDL_GetError());

	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

	rde_critical_error(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), RDE_ERROR_GLAD_INIT);
	
	rde_log_level(RDE_LOG_LEVEL_INFO, "GLAD and SDL2 loaded successfully for linux");

	SDL_GL_SetSwapInterval(1);

	SDL_SetWindowPosition(_window->sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_SetWindowResizable(_window->sdl_window, SDL_TRUE);

	return _window;
}
#endif

#if IS_ANDROID()
rde_window* rde_inner_window_create_android_window(size_t _free_window_index) {
	ENGINE.windows[_free_window_index] = rde_struct_create_window();
	rde_window* _window = &ENGINE.windows[_free_window_index];

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);

	SDL_SetHint(SDL_HINT_ORIENTATIONS, "Portrait");

    SDL_DisplayMode _mode;
    rde_critical_error(SDL_GetDisplayMode(0, 0, &_mode) < 0, "Error getting SDL_DisplayMode -> %s\n", SDL_GetError());
    rde_log_level(RDE_LOG_LEVEL_INFO, "Screen Size: (%d, %d)", _mode.w, _mode.h);
    
    _window->sdl_window = SDL_CreateWindow(NULL, 0, 0, _mode.w, _mode.h ,SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	rde_critical_error(_window->sdl_window == NULL, RDE_ERROR_SDL_WINDOW, SDL_GetError());

    int _drawableSizeX = 0, _drawableSizeY = 0;
    SDL_GL_GetDrawableSize(_window->sdl_window, &_drawableSizeX, &_drawableSizeY);
    rde_log_level(RDE_LOG_LEVEL_INFO, "Drawable Size: (%d, %d)", _drawableSizeX, _drawableSizeY);

    _window->sdl_gl_context = SDL_GL_CreateContext(_window->sdl_window);
    rde_critical_error(_window->sdl_gl_context == NULL, RDE_ERROR_SDL_OPENGL, SDL_GetError());

    SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);

    SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE, "1");
    SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE_PAUSEAUDIO, "1");
    SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1");

    SDL_GL_SetSwapInterval(1);

    rde_log_level(RDE_LOG_LEVEL_INFO, "OpenGL and SDL2 loaded successfully for Android");

    return _window;
}
#endif

#if IS_IOS()
rde_window* rde_inner_window_create_ios_window(size_t _free_window_index) {
	UNIMPLEMENTED();
    return NULL;
}
#endif

#if 0
rde_window* rde_window_create_wasm_window(size_t _free_window_index) {
	UNIMPLEMENTED();
    return NULL;
}
#endif


// ==============================================================================
// =							PUBLIC API - WINDOW					 	    =
// ==============================================================================

rde_window* rde_window_create_window_os() {

	size_t _free_window_index = 0;
	rde_window* _window = NULL;

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_windows; _i++) {
		if(ENGINE.windows[_i].sdl_window != NULL) {

			if(_i == ENGINE.init_info.heap_allocs_config.max_amount_of_windows - 1) {
				rde_critical_error(true, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "windows", ENGINE.init_info.heap_allocs_config.max_amount_of_windows);
			}

			continue;
		}

		_free_window_index = _i;
		break;
	}

	// TODO: create window depending on platform
	#if IS_WINDOWS()
		_window = rde_inner_window_create_windows_window(_free_window_index);
	#elif IS_LINUX() && !IS_ANDROID()
		_window = rde_inner_window_create_linux_window(_free_window_index);
	#elif IS_MAC()
		_window = rde_inner_window_create_mac_window(_free_window_index);
	#elif IS_ANDROID()
		_window = rde_inner_window_create_android_window(_free_window_index);
	#elif IS_IOS()
		_window = rde_inner_window_create_ios_window(_free_window_index);
	#else
		rde_critical_error(true, RDE_ERROR_UNSUPPORTED_PLATFORM);
	#endif

	return _window;
}

rde_vec_2I rde_window_get_window_size(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _size;
	SDL_GetWindowSize(_window->sdl_window, &_size.x, &_size.y);
	return _size;
}
void rde_window_set_window_size(rde_window* _window, rde_vec_2I _size) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	SDL_SetWindowSize(_window->sdl_window, _size.x, _size.y);
}

rde_vec_2I rde_window_get_position(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _position = {  };
	SDL_GetWindowPosition(_window->sdl_window, &_position.x, &_position.y);
	return _position;
}

void rde_window_set_position(rde_window* _window, rde_vec_2I _position) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	SDL_SetWindowPosition(_window->sdl_window, _position.x, _position.y);
}

const char* rde_window_get_title(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	return SDL_GetWindowTitle(_window->sdl_window);
}

void rde_window_set_title(rde_window* _window, const char* _title) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	SDL_SetWindowTitle(_window->sdl_window, _title);
}

bool rde_window_orientation_is_horizontal(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	return _window_size.x >= _window_size.y;
}

void rde_window_take_screen_shot(rde_window* _window, rde_vec_2I _position, rde_vec_2I _size_of_rectangle, const char* _file_name_with_extension) {
	UNUSED(_window)
	UNUSED(_position)
	UNUSED(_size_of_rectangle)
	UNUSED(_file_name_with_extension)
	UNIMPLEMENTED()
}

unsigned char* getAreaOfScreenPixels(rde_window* _window, rde_vec_2I _position, rde_vec_2I _size) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	unsigned char* _pixels = (unsigned char*)malloc(sizeof(unsigned char) * (4 * _window_size.x * _window_size.y));
	rde_critical_error(_pixels == NULL, RDE_ERROR_NO_MEMORY, 4 * _window_size.x * _window_size.y, "pixels array for screenshot");
	memset(_pixels, 0, 4 * _window_size.x * _window_size.y);
	glReadPixels((int)(_window_size.x * 0.5f + _position.x - _size.x * 0.5f ), (int)(_window_size.y * 0.5f + _position.y - _size.y * 0.5f),
	             _size.x, _size.y, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);
	return _pixels;
}

float rde_window_get_aspect_ratio(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	return _window_size.x / (float)_window_size.y;
}

bool rde_window_is_mouse_out_of_window_allowed() {
	return SDL_GetRelativeMouseMode();
}

void rde_window_allow_mouse_out_of_window(bool _allow_mouse_out_of_window) {
	SDL_SetRelativeMouseMode(_allow_mouse_out_of_window ? SDL_FALSE : SDL_TRUE);
}

void rde_window_set_icon(rde_window* _window, const char* _path_to_icon) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	int _width, _height, _channels;

	stbi_set_flip_vertically_on_load(0);
	stbi_uc* _data = stbi_load(_path_to_icon, &_width, &_height, &_channels, 0);
	if(_data == NULL) {
		rde_critical_error(true, RDE_ERROR_FILE_NOT_FOUND, _path_to_icon);
		return;
	}
	
	Uint32 _rmask, _gmask, _bmask, _amask;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	int _shift = (_channels == 3) ? 8 : 0;
	_rmask = 0xff000000 >> _shift;
	_gmask = 0x00ff0000 >> _shift;
	_bmask = 0x0000ff00 >> _shift;
	_amask = 0x000000ff >> _shift;
	#else
	_rmask = 0x000000ff;
	_gmask = 0x0000ff00;
	_bmask = 0x00ff0000;
	_amask = (_channels == 3) ? 0 : 0xff000000;
	#endif

	SDL_Surface* _surface = SDL_CreateRGBSurfaceFrom((void*)_data, _width, _height, 
		_channels * 8, _channels * _width, _rmask, _gmask, _bmask, _amask);


	SDL_SetWindowIcon(_window->sdl_window, _surface);
	SDL_FreeSurface(_surface);
}

void* rde_window_get_native_sdl_window_handle(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	return _window->sdl_window;
}

void* rde_window_get_native_sdl_gl_context_handle(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");
	return _window->sdl_gl_context;
}

void rde_window_destroy_window(rde_window* _window) {
	rde_critical_error(_window == NULL, RDE_ERROR_NO_NULL_ALLOWED, "window");

	if(_window->sdl_gl_context != NULL) {
		SDL_GL_DeleteContext(_window->sdl_gl_context);
	}

	if(_window->sdl_window != NULL) {
		SDL_DestroyWindow(_window->sdl_window);
	}

	memset(_window->key_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_KEYS);
	memset(_window->mouse_states, RDE_INPUT_STATUS_UNINITIALIZED, RDE_AMOUNT_OF_MOUSE_BUTTONS);

	_window->sdl_window = NULL;
}





// ==============================================================================
// =							PRIVATE API - RENDERING					 	=
// ==============================================================================

rde_vec_2F rde_inner_rendering_get_aspect_ratio() {
	rde_vec_2I _window_size = rde_window_get_window_size(current_drawing_window);
	bool _is_horizontal = rde_window_orientation_is_horizontal(current_drawing_window);
	float _aspect_ratio = _window_size.x / (float)_window_size.y;
	rde_vec_2F _aspect_ratios;
	_aspect_ratios.x = _is_horizontal ? 1.f : _aspect_ratio;
	_aspect_ratios.y = _is_horizontal ? _aspect_ratio : 1.f;

	return _aspect_ratios;
}

void rde_inner_rendering_set_rendering_configuration(rde_window* _window) {
#if !IS_MOBILE()
	GLint profile;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
	if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
		rde_log_level(RDE_LOG_LEVEL_INFO, "Core profile");
	} else if(profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
		rde_log_level(RDE_LOG_LEVEL_INFO, "Compatibility profile");
	} else {
		rde_log_level(RDE_LOG_LEVEL_WARNING, "Unkown OpenGL context profile");
	}
#endif

#if IS_MOBILE()
	GLint _no_of_extensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &_no_of_extensions);
	for (int _i = 0; _i < _no_of_extensions; ++_i) {
		if(strcmp("GL_EXT_multisampled_render_to_texture", (const char*)glGetStringi(GL_EXTENSIONS, _i)) == 0) {
			rde_log_level(RDE_LOG_LEVEL_INFO, "MSAA is supported in this device");
			glFramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC)eglGetProcAddress("glFramebufferTexture2DMultisampleEXT");
			glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC)eglGetProcAddress("glRenderbufferStorageMultisampleEXT");
			continue;
		}

		if(strcmp("GL_EXT_discard_framebuffer", (const char*)glGetStringi(GL_EXTENSIONS, _i)) == 0) {
			rde_log_level(RDE_LOG_LEVEL_INFO, "Framebuffer discard is supported in this device");
			glDiscardFramebufferEXT = (PFNGLDISCARDFRAMEBUFFEREXTPROC)eglGetProcAddress("glDiscardFramebufferEXT");
			continue;
		}
	}

	if(glRenderbufferStorageMultisampleEXT == NULL || glFramebufferTexture2DMultisampleEXT == NULL) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, "MSAA is NOT supported on this device");
	}

	if(glDiscardFramebufferEXT == NULL) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, "Framebuffer discard is NOT supported on this device");
	}
#endif

	rde_log_level(RDE_LOG_LEVEL_INFO, "OpenGL Version: %s, Vendor: %s, GPU: %s, GLSL: %s", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_SHADING_LANGUAGE_VERSION));

	RDE_CHECK_GL(glEnable, GL_BLEND);
	RDE_CHECK_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//RDE_CHECK_GL(glBlendFuncSeparate, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

#if !IS_MOBILE() 
	#if !IS_WASM()
		RDE_CHECK_GL(glEnable, GL_PROGRAM_POINT_SIZE);
		RDE_CHECK_GL(glEnable, GL_LINE_SMOOTH);
		RDE_CHECK_GL(glHint, GL_LINE_SMOOTH_HINT,  GL_NICEST);
	#endif
			
	#if !IS_MAC() && !IS_LINUX()
		#if !IS_WASM()
			//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		#endif
	#endif
#endif

#if !IS_MOBILE() && !IS_WASM()
	rde_file_handle* _header_3d_vert_handle = rde_file_open("shaders/core/header_3d_vert.glsl", RDE_FILE_MODE_READ);
	rde_file_handle* _header_3d_frag_handle = rde_file_open("shaders/core/header_3d_frag.glsl", RDE_FILE_MODE_READ);
	char* _header_3d_vert = rde_file_read_full_file(_header_3d_vert_handle, NULL);
	char* _header_3d_frag = rde_file_read_full_file(_header_3d_frag_handle, NULL);
	UNUSED(_header_3d_frag)

	rde_file_handle* _header_2d_vert_handle = rde_file_open("shaders/core/header_2d_vert.glsl", RDE_FILE_MODE_READ);
	rde_file_handle* _header_2d_frag_handle = rde_file_open("shaders/core/header_2d_frag.glsl", RDE_FILE_MODE_READ);
	char* _header_2d_vert = rde_file_read_full_file(_header_2d_vert_handle, NULL);
	char* _header_2d_frag = rde_file_read_full_file(_header_2d_frag_handle, NULL);
	UNUSED(_header_2d_vert)
	UNUSED(_header_2d_frag)

	rde_file_handle* _shader_vertex_handle = rde_file_open("shaders/core/line_vert.glsl", RDE_FILE_MODE_READ);
	rde_file_handle* _shader_fragment_handle = rde_file_open("shaders/core/line_frag.glsl", RDE_FILE_MODE_READ);
	char* _vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	char* _fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);

	if(rde_util_string_contains_substring(_vertex_shader, "header_2d_vert", false)) {
		char* _new_vertex_shader = rde_util_string_replace_substring(_vertex_shader, "header_2d_vert", _header_2d_vert, NULL);
		rde_file_free_read_text(_shader_vertex_handle);
		_vertex_shader = _new_vertex_shader;
	}

	if(rde_util_string_contains_substring(_fragment_shader, "header_2d_frag", false)) {
		char* _new_fragment_shader = rde_util_string_replace_substring(_fragment_shader, "header_2d_frag", _header_2d_frag, NULL);
		rde_file_free_read_text(_shader_fragment_handle);
		_fragment_shader = _new_fragment_shader;
	}

	ENGINE.line_shader = rde_rendering_shader_load(RDE_SHADER_LINE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/color_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/color_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);

	if(rde_util_string_contains_substring(_vertex_shader, "header_2d_vert", false)) {
		char* _new_vertex_shader = rde_util_string_replace_substring(_vertex_shader, "header_2d_vert", _header_2d_vert, NULL);
		rde_file_free_read_text(_shader_vertex_handle);
		_vertex_shader = _new_vertex_shader;
	}

	if(rde_util_string_contains_substring(_fragment_shader, "header_2d_frag", false)) {
		char* _new_fragment_shader = rde_util_string_replace_substring(_fragment_shader, "header_2d_frag", _header_2d_frag, NULL);
		rde_file_free_read_text(_shader_fragment_handle);
		_fragment_shader = _new_fragment_shader;
	}

	ENGINE.color_shader_2d = rde_rendering_shader_load(RDE_SHADER_COLOR, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/core/texture_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/texture_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);

	if(rde_util_string_contains_substring(_vertex_shader, "header_2d_vert", false)) {
		char* _new_vertex_shader = rde_util_string_replace_substring(_vertex_shader, "header_2d_vert", _header_2d_vert, NULL);
		rde_file_free_read_text(_shader_vertex_handle);
		_vertex_shader = _new_vertex_shader;
	}

	if(rde_util_string_contains_substring(_fragment_shader, "header_2d_frag", false)) {
		char* _new_fragment_shader = rde_util_string_replace_substring(_fragment_shader, "header_2d_frag", _header_2d_frag, NULL);
		rde_file_free_read_text(_shader_fragment_handle);
		_fragment_shader = _new_fragment_shader;
	}
	
	ENGINE.texture_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXTURE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/core/text_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/text_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);

	if(rde_util_string_contains_substring(_vertex_shader, "header_2d_vert", false)) {
		char* _new_vertex_shader = rde_util_string_replace_substring(_vertex_shader, "header_2d_vert", _header_2d_vert, NULL);
		rde_file_free_read_text(_shader_vertex_handle);
		_vertex_shader = _new_vertex_shader;
	}

	if(rde_util_string_contains_substring(_fragment_shader, "header_2d_frag", false)) {
		char* _new_fragment_shader = rde_util_string_replace_substring(_fragment_shader, "header_2d_frag", _header_2d_frag, NULL);
		rde_file_free_read_text(_shader_fragment_handle);
		_fragment_shader = _new_fragment_shader;
	}

	ENGINE.text_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXT, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/framebuffer_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/framebuffer_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);

	if(rde_util_string_contains_substring(_vertex_shader, "header_2d_vert", false)) {
		char* _new_vertex_shader = rde_util_string_replace_substring(_vertex_shader, "header_2d_vert", _header_2d_vert, NULL);
		rde_file_free_read_text(_shader_vertex_handle);
		_vertex_shader = _new_vertex_shader;
	}
	
	if(rde_util_string_contains_substring(_fragment_shader, "header_2d_frag", false)) {
		char* _new_fragment_shader = rde_util_string_replace_substring(_fragment_shader, "header_2d_frag", _header_2d_frag, NULL);
		rde_file_free_read_text(_shader_fragment_handle);
		_fragment_shader = _new_fragment_shader;
	}

	ENGINE.framebuffer_shader = rde_rendering_shader_load(RDE_SHADER_FRAMEBUFFER, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/mesh_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/mesh_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);

	if(rde_util_string_contains_substring(_vertex_shader, "header_3d_vert", false)) {
		char* _new_vertex_shader = rde_util_string_replace_substring(_vertex_shader, "header_3d_vert", _header_3d_vert, NULL);
		rde_file_free_read_text(_shader_vertex_handle);
		_vertex_shader = _new_vertex_shader;
	}
	
	if(rde_util_string_contains_substring(_fragment_shader, "header_3d_frag", false)) {
		char* _new_fragment_shader = rde_util_string_replace_substring(_fragment_shader, "header_3d_frag", _header_3d_frag, NULL);
		rde_file_free_read_text(_shader_fragment_handle);
		_fragment_shader = _new_fragment_shader;
	}

	char* _fragment_with_values = (char*)malloc(sizeof(char) * 10000);
	memset(_fragment_with_values, 0, 10000);
	snprintf(_fragment_with_values, 10000, _fragment_shader, ENGINE.init_info.illumination_config.max_amount_of_point_lights, 
	         ENGINE.init_info.illumination_config.max_amount_of_spot_lights);

	ENGINE.mesh_shader = rde_rendering_shader_load(RDE_SHADER_MESH, _vertex_shader, _fragment_with_values);

	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
	
	_shader_vertex_handle = rde_file_open("shaders/core/skybox_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/core/skybox_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.skybox_shader = rde_rendering_shader_load(RDE_SHADER_SKYBOX, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);
#else
	rde_file_handle* _shader_vertex_handle = rde_file_open("shaders/es/line_vert.glsl", RDE_FILE_MODE_READ);
	rde_file_handle* _shader_fragment_handle = rde_file_open("shaders/es/line_frag.glsl", RDE_FILE_MODE_READ);
	char* _vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	char* _fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.line_shader = rde_rendering_shader_load(RDE_SHADER_LINE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/color_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/color_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.color_shader_2d = rde_rendering_shader_load(RDE_SHADER_COLOR, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/texture_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/texture_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.texture_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXTURE, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/text_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/text_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.text_shader_2d = rde_rendering_shader_load(RDE_SHADER_TEXT, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/framebuffer_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/framebuffer_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.framebuffer_shader = rde_rendering_shader_load(RDE_SHADER_FRAMEBUFFER, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/mesh_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/mesh_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);

	char* _fragment_with_values = (char*)malloc(sizeof(char) * 10000);
	memset(_fragment_with_values, 0, 10000);
	snprintf(_fragment_with_values, 10000, _fragment_shader, ENGINE.init_info.illumination_config.max_amount_of_point_lights, 
	         ENGINE.init_info.illumination_config.max_amount_of_spot_lights);

	ENGINE.mesh_shader = rde_rendering_shader_load(RDE_SHADER_MESH, _vertex_shader, _fragment_with_values);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	_shader_vertex_handle = rde_file_open("shaders/es/skybox_vert.glsl", RDE_FILE_MODE_READ);
	_shader_fragment_handle = rde_file_open("shaders/es/skybox_frag.glsl", RDE_FILE_MODE_READ);
	_vertex_shader = rde_file_read_full_file(_shader_vertex_handle, NULL);
	_fragment_shader = rde_file_read_full_file(_shader_fragment_handle, NULL);
	ENGINE.skybox_shader = rde_rendering_shader_load(RDE_SHADER_SKYBOX, _vertex_shader, _fragment_shader);
	rde_file_close(_shader_vertex_handle);
	rde_file_close(_shader_fragment_handle);

	free(_fragment_with_values);
#endif
	
	rde_vec_2I _window_size = rde_window_get_window_size(_window);

	GLuint _vao, _vbo;
	RDE_CHECK_GL(glGenVertexArrays, 1, &_vao);
	RDE_CHECK_GL(glGenBuffers, 1, &_vbo);
	RDE_CHECK_GL(glBindVertexArray, _vao);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _vbo);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(FRAMEBUFFER_QUAD_DATA), &FRAMEBUFFER_QUAD_DATA, GL_STATIC_DRAW);
	RDE_CHECK_GL(glEnableVertexAttribArray, 0);
	RDE_CHECK_GL(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	// 1 is reserved for color
	RDE_CHECK_GL(glEnableVertexAttribArray, 2);
	RDE_CHECK_GL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	DEFAULT_RENDER_TEXTURE = rde_rendering_render_texture_create(_window_size.x, _window_size.y);
	DEFAULT_RENDER_TEXTURE->vao = _vao;
	DEFAULT_RENDER_TEXTURE->vbo = _vbo;
	
#if IS_MOBILE()
	rde_rendering_set_antialiasing(_window, RDE_ANTIALIASING_NONE);
#else
	rde_rendering_set_antialiasing(_window, RDE_ANTIALIASING_X4);
#endif
}

void rde_inner_rendering_draw_to_framebuffer(rde_render_texture* _render_texture) {
	GLuint _framebuffer_id = _render_texture == DEFAULT_RENDER_TEXTURE ? 0 : _render_texture->opengl_framebuffer_id;
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, _framebuffer_id);
	RDE_CHECK_GL(glDisable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
	RDE_CHECK_GL(glClear, GL_COLOR_BUFFER_BIT);

	RDE_CHECK_GL(glUseProgram, ENGINE.framebuffer_shader->compiled_program_id);

	RDE_CHECK_GL(glBindVertexArray, _render_texture->vao);
	RDE_CHECK_GL(glActiveTexture, GL_TEXTURE0);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _render_texture->opengl_texture_id);
	RDE_CHECK_GL(glDrawArrays, GL_TRIANGLES, 0, 6);
}

void rde_inner_rendering_flush_to_default_render_texture(rde_window* _window) {
	if(ENGINE.antialiasing.samples > 0) {
#if !IS_MOBILE()
		RDE_CHECK_GL(glBindFramebuffer, GL_READ_FRAMEBUFFER, ENGINE.antialiasing.frame_buffer_id);
		RDE_CHECK_GL(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, DEFAULT_RENDER_TEXTURE->opengl_framebuffer_id);
		rde_vec_2I _screen_size = rde_window_get_window_size(_window);
		RDE_CHECK_GL(glBlitFramebuffer, 0, 0, _screen_size.x, _screen_size.y, 0, 0, _screen_size.x, _screen_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		rde_inner_rendering_draw_to_framebuffer(DEFAULT_RENDER_TEXTURE);
#else
		GLenum _discard_attachments[] = { GL_DEPTH_ATTACHMENT };
		glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, _discard_attachments);
		rde_render_texture _rt;
		_rt.opengl_framebuffer_id = 0;
		_rt.vao = DEFAULT_RENDER_TEXTURE->vao;
		_rt.opengl_texture_id = ENGINE.antialiasing.opengl_texture_id;
		rde_inner_rendering_draw_to_framebuffer(&_rt);
#endif
	} else {
		rde_inner_rendering_draw_to_framebuffer(DEFAULT_RENDER_TEXTURE);
	}
}

void rde_inner_rendering_destroy_current_antialiasing_config() {
	if(ENGINE.antialiasing.opengl_texture_id == -1 || ENGINE.antialiasing.frame_buffer_id == RDE_UINT_MAX) {
		return;
	}

	uint _texture_id = ENGINE.antialiasing.opengl_texture_id;
	RDE_CHECK_GL(glDeleteTextures, 1, &_texture_id);
	RDE_CHECK_GL(glDeleteRenderbuffers, 1, &ENGINE.antialiasing.render_buffer_id);
	RDE_CHECK_GL(glDeleteFramebuffers, 1, &ENGINE.antialiasing.frame_buffer_id);

	ENGINE.antialiasing.opengl_texture_id = -1;
	ENGINE.antialiasing.frame_buffer_id = RDE_UINT_MAX;
	ENGINE.antialiasing.render_buffer_id = RDE_UINT_MAX;
}

void rde_inner_rendering_create_shadows() {
	glGenFramebuffers(1, &ENGINE.shadows.frame_buffer_id);
	
	uint _depth_texture;
	RDE_CHECK_GL(glGenTextures, 1, &_depth_texture);
	ENGINE.shadows.opengl_texture_id = _depth_texture;
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, ENGINE.shadows.opengl_texture_id);
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, RDE_SHADOW_MAP_SIZE, RDE_SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.shadows.frame_buffer_id);
	RDE_CHECK_GL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ENGINE.shadows.opengl_texture_id, 0);
	
#if IS_MOBILE()
	GLenum _a[1] = { GL_NONE };
	RDE_CHECK_GL(glDrawBuffers, 1, _a);
#else
	RDE_CHECK_GL(glDrawBuffer, GL_NONE);
#endif

	RDE_CHECK_GL(glReadBuffer, GL_NONE);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void rde_inner_rendering_destroy_shadows() {
	uint _depth_texture = ENGINE.shadows.opengl_texture_id;
	RDE_CHECK_GL(glDeleteTextures, 1, &_depth_texture);
	RDE_CHECK_GL(glDeleteFramebuffers, 1, &ENGINE.shadows.frame_buffer_id);

	ENGINE.shadows.opengl_texture_id = -1;
	ENGINE.shadows.frame_buffer_id = RDE_UINT_MAX;
}

void rde_inner_rendering_draw_scene_shadows(rde_window* _window, rde_camera* _camera) {
	(void)_window;
	RDE_CHECK_GL(glClearColor, 0.1f, 0.1f, 0.1f, 1.0f);
	RDE_CHECK_GL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 _light_projection;
	mat4 _light_view;
	mat4 _light_space_matrix;
	glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, _camera->near_far.x, _camera->near_far.x, _light_projection);
	glm_lookat((vec3) { ENGINE.illumination.directional_light.direction.x, ENGINE.illumination.directional_light.direction.x, ENGINE.illumination.directional_light.direction.z },
				(vec3) { 0.0f, 0.0f, 0.0f },
				(vec3) { 0.0f, 1.0f, 0.0f },
				_light_view
	);
	 glm_mul(_light_projection, _light_view, _light_space_matrix);
	// render scene from light's point of view
	//simpleDepthShader.use();
	//simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	RDE_CHECK_GL(glViewport, 0, 0, RDE_SHADOW_MAP_SIZE, RDE_SHADOW_MAP_SIZE);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.shadows.frame_buffer_id);
	RDE_CHECK_GL(glClear, GL_DEPTH_BUFFER_BIT);
	
	// Render scene

	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void rde_inner_engine_on_render(float _dt, rde_window* _window) {
	UNUSED(_dt)
	SDL_GL_MakeCurrent(_window->sdl_window, _window->sdl_gl_context);
	rde_vec_2I _window_size = rde_window_get_window_size(_window);
	RDE_CHECK_GL(glViewport, 0, 0, _window_size.x, _window_size.y);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.antialiasing.samples > 0 ? ENGINE.antialiasing.frame_buffer_id : DEFAULT_RENDER_TEXTURE->opengl_framebuffer_id);
}

void rde_inner_rendering_init_2d() {
	current_batch_2d = rde_struct_create_2d_batch();
	rde_inner_rendering_generate_gl_vertex_config_for_quad_2d(&current_batch_2d);

	current_batch_2d.vertices = (rde_vertex_2d*)malloc(sizeof(rde_vertex_2d) * ENGINE.init_info.heap_allocs_config.max_amount_of_vertices_per_batch);
	rde_critical_error(current_batch_2d.vertices == NULL, RDE_ERROR_NO_MEMORY, sizeof(rde_vertex_2d) * ENGINE.init_info.heap_allocs_config.max_amount_of_vertices_per_batch, "2d batch vertices");
}

void rde_inner_rendering_end_2d() {
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindVertexArray, 0);
	RDE_CHECK_GL(glDeleteBuffers, 1, &current_batch_2d.vertex_buffer_object);
	RDE_CHECK_GL(glDeleteBuffers, 1, &current_batch_2d.index_buffer_object);
	RDE_CHECK_GL(glDeleteVertexArrays, 1, &current_batch_2d.vertex_array_object);
	free(current_batch_2d.vertices);
}

void rde_inner_rendering_transform_to_glm_mat4_2d(const rde_transform* _transform, mat4 _mat) {
	rde_vec_2I _window_size = rde_window_get_window_size(current_drawing_window);
	float _aspect_ratio = (float)_window_size.x / (float)_window_size.y;
	rde_vec_2F _screen_pos;
	
	if(current_drawing_camera->camera_type == RDE_CAMERA_TYPE_ORTHOGRAPHIC) {
		_screen_pos.x = _transform->position.x;
		_screen_pos.y = _transform->position.y / _aspect_ratio;
		rde_math_convert_world_position_to_screen_coordinates_2d(current_drawing_window, &_screen_pos);
	} else {
		bool _is_hud = current_batch_2d.is_hud;
		_screen_pos.x = _transform->position.x - (_is_hud ? 0.f : current_drawing_camera->transform->position.x);
		_screen_pos.y = _transform->position.y - (_is_hud ? 0.f : current_drawing_camera->transform->position.y);
	}

	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	glm_translate(_transformation_matrix, (vec3) { _screen_pos.x, _screen_pos.y, 0.f });
	glm_rotate(_transformation_matrix, glm_rad(_transform->rotation.z), (vec3){ 0.0f, 0.0f, 1.0f });
	
	if(current_drawing_camera->camera_type == RDE_CAMERA_TYPE_ORTHOGRAPHIC) {
		glm_scale(_transformation_matrix, (vec3) { _transform->scale.x, _transform->scale.y / _aspect_ratio, _transform->scale.z });
	} else {
		glm_scale(_transformation_matrix, (vec3) { _transform->scale.x, _transform->scale.y, _transform->scale.z });
	}

	glm_mat4_copy(_transformation_matrix, _mat);
}

void rde_inner_rendering_generate_gl_vertex_config_for_quad_2d(rde_batch_2d* _batch) {
	RDE_CHECK_GL(glGenVertexArrays, 1, &_batch->vertex_array_object);
	RDE_CHECK_GL(glBindVertexArray, _batch->vertex_array_object);
	
	RDE_CHECK_GL(glGenBuffers, 1, &_batch->vertex_buffer_object);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _batch->vertex_buffer_object);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(rde_vertex_2d) * ENGINE.init_info.heap_allocs_config.max_amount_of_vertices_per_batch, NULL, GL_DYNAMIC_DRAW);

	RDE_CHECK_GL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, sizeof(rde_vertex_2d), (const void*)0);
	RDE_CHECK_GL(glEnableVertexAttribArray, 0);
	
	RDE_CHECK_GL(glVertexAttribPointer, 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(rde_vertex_2d), (const void*)(sizeof(float) * 3));
	RDE_CHECK_GL(glEnableVertexAttribArray, 1);
	
	RDE_CHECK_GL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE, sizeof(rde_vertex_2d), (const void*)(sizeof(float) * 3 + sizeof(unsigned char) * 4));
	RDE_CHECK_GL(glEnableVertexAttribArray, 2);

	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindVertexArray, 0);
}

void rde_inner_rendering_reset_batch_2d() {
	current_batch_2d.shader = NULL;
	current_batch_2d.texture = rde_struct_create_texture();
	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_vertices_per_batch; _i++) {
		current_batch_2d.vertices[_i] = rde_struct_create_vertex_2d();
	}
	current_batch_2d.amount_of_vertices = 0;
}

void rde_inner_rendering_try_create_batch_2d(rde_shader* _shader, const rde_texture* _texture) {
	if(current_batch_2d.shader == NULL) {
		current_batch_2d.shader = _shader;
	}

	if(current_batch_2d.texture.opengl_texture_id == -1 && _texture != NULL) {
		current_batch_2d.texture = *_texture;
	}
}

void rde_inner_rendering_flush_batch_2d() {
	if(current_batch_2d.shader == NULL) {
		return;
	}

	RDE_CHECK_GL(glUseProgram, current_batch_2d.shader->compiled_program_id);

	RDE_CHECK_GL(glUniformMatrix4fv, glGetUniformLocation(current_batch_2d.shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)current_batch_2d.mvp);
	rde_vec_2I _mouse_pos = current_drawing_window->mouse_position;
	RDE_CHECK_GL(glUniform2f, glGetUniformLocation(current_batch_2d.shader->compiled_program_id, "mouse_position"), _mouse_pos.x, _mouse_pos.y);
	RDE_CHECK_GL(glUniform1f, glGetUniformLocation(current_batch_2d.shader->compiled_program_id, "mouse_position"), ENGINE.delta_time);
	RDE_CHECK_GL(glBindVertexArray, current_batch_2d.vertex_array_object);

	if(current_batch_2d.texture.opengl_texture_id >= 0) {
		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE0);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, current_batch_2d.texture.opengl_texture_id);
	}

	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, current_batch_2d.vertex_buffer_object);
	RDE_CHECK_GL(glBufferSubData, GL_ARRAY_BUFFER, 0, (long)(sizeof(rde_vertex_2d) * current_batch_2d.amount_of_vertices), current_batch_2d.vertices);

	RDE_CHECK_GL(glDrawArrays, GL_TRIANGLES, 0, current_batch_2d.amount_of_vertices);

	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindVertexArray, 0);

	rde_inner_rendering_reset_batch_2d();

	statistics.number_of_drawcalls++;
}

void rde_inner_rendering_try_flush_batch_2d(rde_shader* _shader, const rde_texture* _texture, size_t _extra_vertices) {
	bool _vertex_ok = current_batch_2d.amount_of_vertices + _extra_vertices <= ENGINE.init_info.heap_allocs_config.max_amount_of_vertices_per_batch;
	bool _shader_ok = current_batch_2d.shader == _shader;
	bool _texture_ok = _texture == NULL || current_batch_2d.texture.opengl_texture_id == _texture->opengl_texture_id;
	if(_vertex_ok && _shader_ok && _texture_ok) {
		return;
	}

	rde_inner_rendering_flush_batch_2d();
	rde_inner_rendering_reset_batch_2d();
	rde_inner_rendering_try_create_batch_2d(_shader, _texture);
}

#ifdef RDE_OBJ_MODULE
void rde_inner_fill_obj_mesh_data(rde_obj_mesh_data* _data, fastObjGroup* _group, fastObjMaterial* _material, bool _has_t, bool _has_n) {
	_data->name = _group->name;

	_data->material = _material;

	_data->positions_size = _data->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION;
	_data->positions = (float*)malloc(sizeof(float) * _data->positions_size);

	if(_has_t) {
		_data->texcoords_size = _data->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD;
		_data->texcoords = (float*)malloc(sizeof(float) * _data->texcoords_size);
	}

	if(_has_n) {
		_data->normals_size = _data->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL;
		_data->normals = (float*)malloc(sizeof(float) * _data->normals_size);
	}

	_data->material = _material;

	if(_material != NULL) {
		
	}
	
	//rde_engine_supress_logs(true);
	if(_material != NULL && _material->map_Ka.path != NULL && strlen(_material->map_Ka.path) > 0) {
		_data->map_ka = rde_rendering_texture_load(_material->map_Ka.path, NULL);
	}

	if(_material != NULL && _material->map_Kd.path != NULL && strlen(_material->map_Kd.path) > 0) {
		_data->map_kd = rde_rendering_texture_load(_material->map_Kd.path, NULL);
	}

	if(_material != NULL && _material->map_Ks.path != NULL && strlen(_material->map_Ks.path) > 0) {
		_data->map_ks = rde_rendering_texture_load(_material->map_Ks.path, NULL);
	}

	if(_material != NULL && _material->map_bump.path != NULL && strlen(_material->map_bump.path) > 0) {
		_data->map_bump = rde_rendering_texture_load(_material->map_bump.path, NULL);
	}
	//rde_engine_supress_logs(false);
}

void rde_inner_parse_3_vertices_face_obj(uint _v, uint _offset, fastObjMesh* _mesh, rde_obj_mesh_data* _obj_mesh_data) {
	fastObjIndex _face_index = rde_arr_s_get_fastObjIndex(_offset + 0, _mesh->indices, _mesh->index_count * 3, "Face Index");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 0, rde_arr_s_get_float(_face_index.p * 3 + 0, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 1, rde_arr_s_get_float(_face_index.p * 3 + 1, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 2, rde_arr_s_get_float(_face_index.p * 3 + 2, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");

	_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 1), _mesh->indices, _mesh->index_count * 3, "Face Index");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 3, rde_arr_s_get_float(_face_index.p * 3 + 0, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 4, rde_arr_s_get_float(_face_index.p * 3 + 1, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 5, rde_arr_s_get_float(_face_index.p * 3 + 2, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");

	_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 2), _mesh->indices, _mesh->index_count * 3, "Face Index");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 6, rde_arr_s_get_float(_face_index.p * 3 + 0, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 7, rde_arr_s_get_float(_face_index.p * 3 + 1, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");
	rde_arr_s_set_float(_obj_mesh_data->positions_pointer + 8, rde_arr_s_get_float(_face_index.p * 3 + 2, _mesh->positions, _mesh->position_count * 3, "Positions Obj -> (%u, %u, %u)", _face_index.p, _face_index.t, _face_index.n), _obj_mesh_data->positions, _obj_mesh_data->positions_size, "Positions Mesh");

	_face_index = rde_arr_s_get_fastObjIndex(_offset + 0, _mesh->indices, _mesh->index_count * 3, "Face Index");
	if(_obj_mesh_data->texcoords != NULL && _obj_mesh_data->texcoords_size > 0) {
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 0, rde_arr_s_get_float(_face_index.t * 2 + 0, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 1, rde_arr_s_get_float(_face_index.t * 2 + 1, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");

		_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 1), _mesh->indices, _mesh->index_count * 3, "Face Index");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 2, rde_arr_s_get_float(_face_index.t * 2 + 0, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 3, rde_arr_s_get_float(_face_index.t * 2 + 1, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");

		_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 2), _mesh->indices, _mesh->index_count * 3, "Face Index");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 4, rde_arr_s_get_float(_face_index.t * 2 + 0, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
		rde_arr_s_set_float(_obj_mesh_data->texcoords_pointer + 5, rde_arr_s_get_float(_face_index.t * 2 + 1, _mesh->texcoords, _mesh->texcoord_count * 2, "Texcoords Obj"), _obj_mesh_data->texcoords, _obj_mesh_data->texcoords_size, "Texcoords Mesh");
	}

	_face_index = rde_arr_s_get_fastObjIndex(_offset + 0, _mesh->indices, _mesh->index_count * 3, "Face Index");
	if(_obj_mesh_data->normals != NULL && _obj_mesh_data->normals_size > 0) {
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 0, rde_arr_s_get_float(_face_index.n * 3 + 0, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 1, rde_arr_s_get_float(_face_index.n * 3 + 1, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 2, rde_arr_s_get_float(_face_index.n * 3 + 2, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");

		_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 1), _mesh->indices, _mesh->index_count * 3, "Face Index");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 3, rde_arr_s_get_float(_face_index.n * 3 + 0, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 4, rde_arr_s_get_float(_face_index.n * 3 + 1, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 5, rde_arr_s_get_float(_face_index.n * 3 + 2, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");

		_face_index = rde_arr_s_get_fastObjIndex(_offset + (_v + 2), _mesh->indices, _mesh->index_count * 3, "Face Index");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 6, rde_arr_s_get_float(_face_index.n * 3 + 0, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 7, rde_arr_s_get_float(_face_index.n * 3 + 1, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
		rde_arr_s_set_float(_obj_mesh_data->normals_pointer + 8, rde_arr_s_get_float(_face_index.n * 3 + 2, _mesh->normals, _mesh->normal_count * 3, "Normals Obj"), _obj_mesh_data->normals, _obj_mesh_data->normals_size, "Normals Mesh");
	}

	_obj_mesh_data->positions_pointer += 9;
	_obj_mesh_data->texcoords_pointer += 6;
	_obj_mesh_data->normals_pointer += 9;
}

void* rde_inner_obj_file_open(const char* _file_path, void* _user_data) {
	(void)_user_data;
	SDL_RWops* _sdl_file = SDL_RWFromFile(_file_path, "rb");
	return (void*)_sdl_file;
}

void rde_inner_obj_file_close(void* _file, void* _user_data) {
	(void)_user_data;
	SDL_RWops* _sdl_file = (SDL_RWops*)_file;
	SDL_FreeRW(_sdl_file);
}

size_t rde_inner_obj_file_read(void* _file, void* _dst, size_t _bytes, void* _user_data) {
	(void)_user_data;
	SDL_RWops* _sdl_file = (SDL_RWops*)_file;
	return SDL_RWread(_sdl_file, _dst, 1, _bytes);
}

unsigned long rde_inner_obj_file_size(void* _file, void* _user_data) {
	(void)_user_data;
	SDL_RWops* _sdl_file = (SDL_RWops*)_file;
	return SDL_RWsize(_sdl_file);
}

rde_model* rde_inner_obj_load_model(const char* _obj_path) {
	clock_t _t_start, _t_end;
	_t_start = clock();

	rde_log_color(RDE_LOG_COLOR_GREEN, "Loading OBJ '%s':", _obj_path);

	rde_model* _model = NULL;
	for (size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_models; _i++) {
		rde_model* _m = &ENGINE.models[_i];

		if (_m->mesh_array != NULL) {
			continue;
		}

		_model = _m;
		break;
	}

	rde_critical_error(_model == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "models", ENGINE.init_info.heap_allocs_config.max_amount_of_models);

	fastObjCallbacks _callbacks = {
		.file_open  = rde_inner_obj_file_open,
		.file_close = rde_inner_obj_file_close,
		.file_read  = rde_inner_obj_file_read,
		.file_size  = rde_inner_obj_file_size
	};
	fastObjMesh* _mesh = fast_obj_read_with_callbacks(_obj_path, &_callbacks, NULL);

	rde_critical_error(_mesh == NULL, "OBJ Mesh read from fast_obj is NULL, '%s' \n", _obj_path);

	size_t _material_count = _mesh->material_count == 0 ? 1 : _mesh->material_count;
	rde_obj_mesh_data* _obj = (rde_obj_mesh_data*)malloc(sizeof(rde_obj_mesh_data) * _material_count);
	for(size_t _i = 0; _i < _material_count; _i++) {
		_obj[_i] = rde_inner_struct_create_obj_mesh_data();
	}

	fastObjGroup* _group_or_obj = _mesh->objects;
	uint _group_or_object_count = _mesh->object_count;
	if(_group_or_obj[0].name == NULL && _mesh->groups[0].name != NULL) {
		_group_or_obj = _mesh->groups;
		_group_or_object_count = _mesh->group_count;
	}

	for(size_t _i = 0; _i < _group_or_object_count; _i++) {
		rde_obj_mesh_data* _obj_mesh_data = NULL;
		fastObjGroup* _o_or_g_inner = &_group_or_obj[_i];
		for (size_t _j = 0; _j < _o_or_g_inner->face_count; _j++) {
			uint _material_key = _mesh->face_materials[_o_or_g_inner->face_offset + _j];
			_obj_mesh_data = &_obj[_material_key];
			
			uint _amount_of_elements_on_face_vertex = _mesh->face_vertices[_o_or_g_inner->face_offset + _j];
			if (_amount_of_elements_on_face_vertex == 3) {
				_obj_mesh_data->vertex_count += 3;
			} else {
				for (size_t _v = 0; _v < _amount_of_elements_on_face_vertex - 2; _v++) {
					_obj_mesh_data->vertex_count += 3;
				}
			}
		}
	}

	uint _offset = 0;
	for(size_t _i = 0; _i < _group_or_object_count; _i++) {
		fastObjGroup* _o_or_g = &_group_or_obj[_i];

		for (size_t _j = 0; _j < _o_or_g->face_count; _j++) {
			uint _material_key = _mesh->face_materials[_o_or_g->face_offset + _j];
			rde_obj_mesh_data* _obj_mesh_data = NULL;

			_obj_mesh_data = &_obj[_material_key];
			if(_obj_mesh_data->positions == NULL) {
				fastObjIndex _index = _mesh->indices[_o_or_g->index_offset + _j];
				rde_inner_fill_obj_mesh_data(_obj_mesh_data, 
				                       _o_or_g,
				                       &_mesh->materials[_material_key], 
				                       _index.t != 0,
				                       _index.n != 0);
			}

			uint _amount_of_elements_on_face_vertex = _mesh->face_vertices[_o_or_g->face_offset + _j];
			if (_amount_of_elements_on_face_vertex == 3) {
				rde_inner_parse_3_vertices_face_obj(0, _offset, _mesh, &_obj[_material_key]);
			} else {
				for (size_t _v = 0; _v < _amount_of_elements_on_face_vertex - 2; _v++) {
					rde_inner_parse_3_vertices_face_obj(_v, _offset, _mesh, &_obj[_material_key]);
				}
			}
			_offset += _amount_of_elements_on_face_vertex;
		}
	}

	for(size_t _i = 0; _i < _material_count; _i++) {
		rde_obj_mesh_data* _obj_mesh_data = &_obj[_i];

		if(_obj_mesh_data->vertex_count <= 0) {
			continue;
		}

		rde_mesh_gen_data _data = {
			.name = _obj_mesh_data->name,
			.vertex_count = _obj_mesh_data->vertex_count,
			.positions = _obj_mesh_data->positions,
			.positions_size = _obj_mesh_data->positions_size,
			.texcoords = _obj_mesh_data->texcoords,
			.texcoords_size = _obj_mesh_data->texcoords_size,
			.normals = _obj_mesh_data->normals,
			.normals_size = _obj_mesh_data->normals_size,
			.material = {
				.map_ka = _obj_mesh_data->map_ka,
				.map_kd = _obj_mesh_data->map_kd,
				.map_ks = _obj_mesh_data->map_ks,
				.map_bump = _obj_mesh_data->map_bump,
			}
		};

		if(_obj_mesh_data->material != NULL) {
			_data.material.material_light_data = (rde_material_light_data) {
				.shininess = _obj_mesh_data->material->Ns,
				.ka = { _obj_mesh_data->material->Ka[0], _obj_mesh_data->material->Ka[1], _obj_mesh_data->material->Ka[2] },
				.kd = { _obj_mesh_data->material->Kd[0], _obj_mesh_data->material->Kd[1], _obj_mesh_data->material->Kd[2] },
				.ks = { _obj_mesh_data->material->Ks[0], _obj_mesh_data->material->Ks[1], _obj_mesh_data->material->Ks[2] }
			};
		} else {
			_data.material.material_light_data = rde_struct_create_material_light_data();
		}
		
		rde_engine_supress_logs(true);
		rde_mesh _mesh = rde_inner_struct_create_mesh(&_data);
		rde_engine_supress_logs(false);
		stbds_arrput(_model->mesh_array, _mesh);
		_model->mesh_array_size++;
	}

	rde_strcpy(_model->file_path, RDE_MAX_PATH, _obj_path);

	fast_obj_destroy(_mesh);
	free(_obj);

	_t_end = clock();

	rde_log_color(RDE_LOG_COLOR_GREEN, "Model loaded in %f""s", (_t_end - _t_start) / 1000.f);

	return _model;
}
#endif

#ifdef RDE_FBX_MODULE
void parse_3_vertices_face_fbx(uint _i, uint _v, uint* _mesh_indices, 
                               float* _mesh_positions, float* _mesh_texcoords, ufbx_face* _face, ufbx_mesh* _mesh,
                           	   uint* _indices_pointer, uint* _positions_pointer, 
                           	   uint* _texcoords_pointer) {
	_mesh_indices[*_indices_pointer + 0] = 3 * _i + 0;
	_mesh_indices[*_indices_pointer + 1] = 3 * _i + 1;
	_mesh_indices[*_indices_pointer + 2] = 3 * _i + 2;

	_mesh_positions[*_positions_pointer + 0] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + 0]].x;
	_mesh_positions[*_positions_pointer + 1] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + 0]].y;
	_mesh_positions[*_positions_pointer + 2] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + 0]].z;

	_mesh_positions[*_positions_pointer + 3] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 1)]].x;
	_mesh_positions[*_positions_pointer + 4] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 1)]].y;
	_mesh_positions[*_positions_pointer + 5] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 1)]].z;
	
	_mesh_positions[*_positions_pointer + 6] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 2)]].x;
	_mesh_positions[*_positions_pointer + 7] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 2)]].y;
	_mesh_positions[*_positions_pointer + 8] = _mesh->vertex_position.values.data[_mesh->vertex_position.indices.data[_face->index_begin + (_v + 2)]].z;
	
	if(_mesh_texcoords != NULL) {
		_mesh_texcoords[*_texcoords_pointer + 0] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + 0]].x;
		_mesh_texcoords[*_texcoords_pointer + 1] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + 0]].y;

		_mesh_texcoords[*_texcoords_pointer + 2] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + (_v + 1)]].x;
		_mesh_texcoords[*_texcoords_pointer + 3] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + (_v + 1)]].y;

		_mesh_texcoords[*_texcoords_pointer + 4] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + (_v + 2)]].x;
		_mesh_texcoords[*_texcoords_pointer + 5] = _mesh->vertex_uv.values.data[_mesh->vertex_uv.indices.data[_face->index_begin + (_v + 2)]].y;
	}

	*_indices_pointer += 3;
	*_positions_pointer += 9;
	*_texcoords_pointer += 6;
}

rde_model* rde_rendering_model_fbx_load(const char* _fbx_path, const char* _texture_path) {
	rde_model* _model = NULL;
	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_models; _i++) {
		rde_model* _m = &ENGINE.models[_i];

		if(_m->mesh_array != NULL) {
			continue;
		}

		_model = _m;
		break;
	}

    ufbx_load_opts _opts = { 0 };
    ufbx_error _error;
    ufbx_scene* _scene = ufbx_load_file(_fbx_path, &_opts, &_error);
    rde_critical_error(_scene == NULL, RDE_ERROR_FBX_COULD_NOT_LOAD, _fbx_path, _error.description.data);

	uint _nodes_size = _scene->nodes.count;

	_model->mesh_array = NULL;
	_model->mesh_array_size = 0;
	
	rde_log_color(RDE_LOG_COLOR_GREEN, "Loading FBX '%s' with %u nodes:", _fbx_path, _nodes_size);

    for (size_t i = 0; i < _scene->nodes.count; i++) {
        ufbx_node *node = _scene->nodes.data[i];
		if (node->is_root || node->mesh == NULL) {
			continue;
		}

        if (node->mesh) {
			ufbx_mesh* _mesh = node->mesh;
			size_t _faces_size = _mesh->faces.count;
			
			uint _mesh_indices_size = 0;
			size_t _mesh_positions_size = 0;
			uint _mesh_texcoords_size = 0;

			for(size_t _i = 0; _i < _faces_size; _i++) {
				ufbx_face* _face = &_mesh->faces.data[_i];

				if (_face->num_indices == 3) {
					_mesh_indices_size += 3;
					_mesh_positions_size += 3;
					_mesh_texcoords_size += 3;
				} else {
					int _indices_in_face = _face->num_indices;
					int _moving_pointer = 1;
					
					while(_moving_pointer != _indices_in_face - 1) {
						_mesh_indices_size += 3;
						_mesh_positions_size += 3;
						_mesh_texcoords_size += 3;
						_moving_pointer++;
					}
				}
			}

			if(_mesh_indices_size == 0 || _mesh->vertex_position.values.count == 0) {
				continue;
			}

			uint* _mesh_indices = (uint*)malloc(sizeof(uint) * _mesh_indices_size * 1);
			float* _mesh_positions = (float*)malloc(sizeof(float) * _mesh_positions_size * 3);
			float* _mesh_texcoords = (float*)malloc(sizeof(float) * _mesh_texcoords_size * 2);;

			uint _indices_pointer = 0;
			uint _positions_pointer = 0;
			uint _texcoords_pointer = 0;

			for(size_t _i = 0; _i < _faces_size; _i++) {
				ufbx_face* _face = &_mesh->faces.data[_i];

				if (_face->num_indices == 3) {
					parse_3_vertices_face_fbx(_indices_pointer / 3, 0,
					                          _mesh_indices, _mesh_positions, _mesh_texcoords, 
					                          _face, _mesh,
					                          &_indices_pointer, &_positions_pointer, &_texcoords_pointer);
				} else {
					for(size_t _v = 0; _v < _face->num_indices - 2; _v++) {
						parse_3_vertices_face_fbx(_indices_pointer / 3, _v,
						                          _mesh_indices, _mesh_positions, _mesh_texcoords, 
						                          _face, _mesh,
						                          &_indices_pointer, &_positions_pointer, &_texcoords_pointer);
					}
				}
			}
			
			rde_mesh _m = rde_inner_struct_create_mesh(_mesh_positions_size, _mesh_indices_size);
			rde_rendering_mesh_set_vertex_positions(&_m, _mesh_positions, true);
			rde_rendering_mesh_set_indices(&_m, _mesh_indices, true);

			rde_texture* _texture = rde_rendering_texture_load(_texture_path, NULL);
			rde_rendering_mesh_set_vertex_texture_data(&_m, _mesh_texcoords_size, _mesh_texcoords, _texture, true);

			rde_log_color(RDE_LOG_COLOR_GREEN, "	- vertices: %u, indices: %u, texcoords: %u \n", _mesh_positions_size, _mesh_indices_size, _mesh_texcoords_size);

			stbds_arrput(_model->mesh_array, _m);
			_model->mesh_array_size++;
		}
    }

    ufbx_free_scene(_scene);
	return _model;
}
#endif

void rde_inner_create_line_batch_buffers() {
	RDE_CHECK_GL(glGenVertexArrays, 1, &current_batch_3d.line_batch.vertex_array_object);
	RDE_CHECK_GL(glBindVertexArray, current_batch_3d.line_batch.vertex_array_object);
	
	RDE_CHECK_GL(glGenBuffers, 1, &current_batch_3d.line_batch.vertex_buffer_object);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, current_batch_3d.line_batch.vertex_buffer_object);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(rde_line_vertex) * RDE_MAX_LINES_PER_DRAW, NULL, GL_DYNAMIC_DRAW);

	RDE_CHECK_GL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, sizeof(rde_line_vertex), (const void*)0);
	RDE_CHECK_GL(glEnableVertexAttribArray, 0);
	
	RDE_CHECK_GL(glVertexAttribPointer, 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(rde_line_vertex), (const void*)(sizeof(float) * 3));
	RDE_CHECK_GL(glEnableVertexAttribArray, 1);

	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindVertexArray, 0);
}

void rde_inner_destroy_line_batch_buffers() {
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindVertexArray, 0);
	RDE_CHECK_GL(glDeleteBuffers, 1, &current_batch_3d.line_batch.vertex_buffer_object);
	RDE_CHECK_GL(glDeleteVertexArrays, 1, &current_batch_3d.line_batch.vertex_array_object);

	free(current_batch_3d.line_batch.vertices);
	current_batch_3d.line_batch.vertices = NULL;
}

void rde_inner_rendering_init_3d() {
	DEFAULT_TEXTURE = rde_rendering_memory_texture_create(RDE_DEFAULT_TEXTURE_SIZE, RDE_DEFAULT_TEXTURE_SIZE, RDE_DEFAULT_TEXTURE_CHANNELS);
	
	for(int _y = 0; _y < RDE_DEFAULT_TEXTURE_SIZE; _y++) {
		for(int _x = 0; _x < RDE_DEFAULT_TEXTURE_SIZE; _x++) {
			rde_rendering_memory_texture_set_pixel(DEFAULT_TEXTURE, (rde_vec_2I) {_x, _y}, RDE_COLOR_WHITE);
		}
	}

	rde_rendering_memory_texture_gpu_upload(DEFAULT_TEXTURE);
	current_batch_3d = rde_struct_create_batch_3d();
	current_batch_3d.line_batch.vertices = (rde_line_vertex*)malloc(sizeof(rde_line_vertex) * RDE_MAX_LINES_PER_DRAW);
	rde_inner_create_line_batch_buffers();
}

void rde_inner_rendering_end_3d() {
	if(DEFAULT_TEXTURE != NULL) {
		rde_rendering_memory_texture_destroy(DEFAULT_TEXTURE);
	}
	rde_inner_destroy_line_batch_buffers();
}

void rde_inner_rendering_transform_to_glm_mat4_3d(const rde_transform* _transform, mat4 _mat) {
	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	glm_translate(_transformation_matrix, (vec3) { _transform->position.x, _transform->position.y, _transform->position.z });
	
	if(_transform->rotation.x != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_transform->rotation.x), (vec3){ 1.0f, 0.0f, 0.0f });
	}

	if(_transform->rotation.y != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_transform->rotation.y), (vec3){ 0.0f, 1.0f, 0.0f });
	}

	if(_transform->rotation.z != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_transform->rotation.z), (vec3){ 0.0f, 0.0f, 1.0f });
	}

	glm_scale(_transformation_matrix, (vec3) { _transform->scale.x, _transform->scale.y, _transform->scale.z });
	glm_mat4_copy(_transformation_matrix, _mat);
}

bool rde_inner_rendering_is_mesh_ok_to_render(rde_mesh* _mesh) {
	if(_mesh->vertex_positions == NULL) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to render a mesh without positions, skipping rendering for this mesh");
		return false;
	}

	return true;
}

float* rde_inner_rendering_mesh_calculate_normals(float* _vertex_positions, size_t _indices_count, size_t _vertex_count, uint* _indices) {
	size_t _normals_size = _vertex_count * 3;
	float* _normals = (float*)malloc(sizeof(float) * _normals_size);
	for(size_t _i = 0; _i < _normals_size; _i++) {
		_normals[_i] = 0;
	}

	for(uint _i = 0; _i < _indices_count; _i += 3) {
		vec3 _a = (vec3) { _vertex_positions[_indices[_i + 0] + 0], _vertex_positions[_indices[_i + 0] + 1], _vertex_positions[_indices[_i + 0] + 2] };
		vec3 _b = (vec3) { _vertex_positions[_indices[_i + 1] + 0], _vertex_positions[_indices[_i + 1] + 1], _vertex_positions[_indices[_i + 1] + 2] };
		vec3 _c = (vec3) { _vertex_positions[_indices[_i + 2] + 0], _vertex_positions[_indices[_i + 2] + 1], _vertex_positions[_indices[_i + 2] + 2] };

		vec3 _c_b = (vec3) { _c[0] - _b[0], _c[1] - _b[1], _c[2] - _b[2] };
		vec3 _a_b = (vec3) { _a[0] - _b[0], _a[1] - _b[1], _a[2] - _b[2] };

		vec3 _cross_product = GLM_VEC3_ZERO;
		glm_cross(_c_b, _a_b, _cross_product);
		glm_normalize(_cross_product);

		_normals[_indices[_i + 0] + 0] += _cross_product[0];
		_normals[_indices[_i + 0] + 1] += _cross_product[1];
		_normals[_indices[_i + 0] + 2] += _cross_product[2];
		// rde_log_level(RDE_LOG_LEVEL_INFO, "Normal for %d (%f, %f, %f)", _indices[_i + 0], _normals[_indices[_i + 0] + 0], _normals[_indices[_i + 0] + 1], _normals[_indices[_i + 0] + 2]);

		_normals[_indices[_i + 1] + 0] += _cross_product[0];
		_normals[_indices[_i + 1] + 1] += _cross_product[1];
		_normals[_indices[_i + 1] + 2] += _cross_product[2];
		// rde_log_level(RDE_LOG_LEVEL_INFO, "Normal for %d (%f, %f, %f)", _indices[_i + 1], _normals[_indices[_i + 1] + 0], _normals[_indices[_i + 1] + 1], _normals[_indices[_i + 1] + 2]);

		_normals[_indices[_i + 2] + 0] += _cross_product[0];
		_normals[_indices[_i + 2] + 1] += _cross_product[1];
		_normals[_indices[_i + 2] + 2] += _cross_product[2];
		// rde_log_level(RDE_LOG_LEVEL_INFO, "Normal for %d (%f, %f, %f)", _indices[_i + 2], _normals[_indices[_i + 2] + 0], _normals[_indices[_i + 2] + 1], _normals[_indices[_i + 2] + 2]);

		// rde_log_level(RDE_LOG_LEVEL_INFO, "Doing triangle of indices (%d, %d, %d)", _indices[_i + 0], _indices[_i + 1], _indices[_i + 2]);
	}

	// rde_log_level(RDE_LOG_LEVEL_INFO, "Indices: %d", _indices_count);
	for(uint _i = 0; _i < _indices_count; _i++) {
		vec3 _normal = (vec3) { _normals[_indices[_i] + 0], _normals[_indices[_i] + 1], _normals[_indices[_i] + 2] };
		glm_normalize(_normal);
		_normals[_indices[_i] + 0] = _normal[0];
		_normals[_indices[_i] + 1] = _normal[1];
		_normals[_indices[_i] + 2] = _normal[2];

		// rde_log_level(RDE_LOG_LEVEL_INFO, "vertex index: %d, normal(%f, %f, %f)", _i, _normal[0], _normal[1], _normal[2]);
	}

	return _normals;
}

rde_mesh rde_inner_struct_create_mesh(rde_mesh_gen_data* _data) {
	rde_mesh _mesh;
	_mesh.vao = 0;

	memset(_mesh.name, 0, RDE_MESH_NAME_MAX);
	rde_strcat(_mesh.name, RDE_MESH_NAME_MAX, _data->name);
	
	GLuint _vao = 0;
	RDE_CHECK_GL(glGenVertexArrays, 1, &_vao);

	_mesh.vao = _vao;
	_mesh.vertex_count = _data->vertex_count;
	_mesh.vertex_positions = _data->positions;
	_mesh.vertex_normals = _data->normals;
	_mesh.vertex_texcoords = _data->texcoords;
	
	_mesh.transformation_matrices = NULL;
	_mesh.transformation_matrices = (mat4*)malloc(sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW );
	for(size_t _i = 0; _i < RDE_MAX_MODELS_PER_DRAW; _i++) {
		glm_mat4_zero(_mesh.transformation_matrices[_i]);
	}

	RDE_CHECK_GL(glBindVertexArray, _mesh.vao);
	
	size_t _positions_size = sizeof(float) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION;
	RDE_CHECK_GL(glGenBuffers, 1, &_mesh.vbo[0]);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh.vbo[0]);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _positions_size, _mesh.vertex_positions, GL_STATIC_DRAW);
	RDE_CHECK_GL(glEnableVertexAttribArray, 0);
	RDE_CHECK_GL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);

	if(_data->normals != NULL && _data->normals_size > 0) {
		size_t _normals_size = sizeof(float) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL;
		RDE_CHECK_GL(glGenBuffers, 1, &_mesh.vbo[1]);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh.vbo[1]);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _normals_size, _mesh.vertex_normals, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 1);
		RDE_CHECK_GL(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	}

	if(_data->texcoords != NULL && _data->texcoords_size > 0) {
		size_t _texcoords_size = sizeof(float) * _mesh.vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD;
		RDE_CHECK_GL(glGenBuffers, 1, &_mesh.vbo[2]);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh.vbo[2]);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _texcoords_size, _mesh.vertex_texcoords, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 2);
		RDE_CHECK_GL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	}

	RDE_CHECK_GL(glGenBuffers, 1, &_mesh.vbo[3]);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh.vbo[3]);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW, NULL, GL_DYNAMIC_DRAW);
	
	RDE_CHECK_GL(glEnableVertexAttribArray, 3);
	RDE_CHECK_GL(glVertexAttribPointer, 3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);

	RDE_CHECK_GL(glEnableVertexAttribArray, 4);
	RDE_CHECK_GL(glVertexAttribPointer, 4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));

	RDE_CHECK_GL(glEnableVertexAttribArray, 5);
	RDE_CHECK_GL(glVertexAttribPointer, 5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));

	RDE_CHECK_GL(glEnableVertexAttribArray, 6);
	RDE_CHECK_GL(glVertexAttribPointer, 6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));

	RDE_CHECK_GL(glVertexAttribDivisor, 3, 1);
    RDE_CHECK_GL(glVertexAttribDivisor, 4, 1);
    RDE_CHECK_GL(glVertexAttribDivisor, 5, 1);
    RDE_CHECK_GL(glVertexAttribDivisor, 6, 1);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);

	RDE_CHECK_GL(glBindVertexArray, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);

	_mesh.material = rde_struct_create_material();
	_mesh.material.map_ka = _data->material.map_ka;
	_mesh.material.map_kd = _data->material.map_kd;
	_mesh.material.map_ks = _data->material.map_ks;
	_mesh.material.map_bump = _data->material.map_bump;
	_mesh.material.material_light_data = _data->material.material_light_data;


	rde_log_level(RDE_LOG_LEVEL_INFO, "Created mesh with %zu vertices. VAO: %u\n", _mesh.vertex_count, _vao);

	return _mesh;
}

void rde_inner_rendering_reset_line_batch() {
	for(size_t _i = 0; _i < RDE_MAX_LINES_PER_DRAW; _i++) {
		current_batch_3d.line_batch.vertices[_i] = rde_struct_create_line_vertex();
	}

	current_batch_3d.line_batch.amount_of_vertices = 0;
	current_batch_3d.line_batch.shader = NULL;
	current_batch_3d.line_batch.thickness = 0;
}

void rde_inner_rendering_reset_batch_3d() {
	current_batch_3d.shader = NULL;

	if(current_batch_3d.mesh != NULL) {
		for(size_t _i = 0; _i < current_batch_3d.amount_of_models_per_draw; _i++) {
			glm_mat4_zero(current_batch_3d.mesh->transformation_matrices[_i]);
		}
	}

	current_batch_3d.mesh = NULL;
	current_batch_3d.amount_of_models_per_draw = 0;
}

void rde_inner_rendering_try_create_line_batch(rde_shader* _shader, unsigned short _thickness) {
	if(current_batch_3d.line_batch.shader == NULL) {
		current_batch_3d.line_batch.shader = _shader;
	}

	if(current_batch_3d.line_batch.thickness == 0 ) {
		current_batch_3d.line_batch.thickness = _thickness;
	}
}

void rde_inner_rendering_try_create_batch_3d(rde_shader* _shader, rde_mesh* _mesh) {
	if(current_batch_3d.shader == NULL) {
		current_batch_3d.shader = _shader;
	}

	if(current_batch_3d.mesh == NULL && _mesh != NULL) {
		current_batch_3d.mesh = _mesh;
	}
}

void rde_inner_rendering_flush_line_batch() {
	if(current_batch_3d.line_batch.shader == NULL || current_batch_3d.line_batch.amount_of_vertices == 0) {
		return;
	}

	if(current_batch_3d.line_batch.thickness <= 0) {
		current_batch_3d.line_batch.thickness = 1;
	}
	RDE_CHECK_GL(glLineWidth, current_batch_3d.line_batch.thickness);

	rde_shader* _shader = current_batch_3d.line_batch.shader;
	RDE_CHECK_GL(glUseProgram, _shader->compiled_program_id);

	mat4 _view_projection_matrix = GLM_MAT4_IDENTITY_INIT;

	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_vec_3F _cam_pos = current_drawing_camera->transform->position;
	rde_vec_3F _cam_direction = current_drawing_camera->direction;
	rde_vec_3F _cam_up = current_drawing_camera->up;
	glm_lookat( (vec3) { _cam_pos.x, _cam_pos.y, _cam_pos.z },
				(vec3) { _cam_pos.x + _cam_direction.x, _cam_pos.y + _cam_direction.y, _cam_pos.z + _cam_direction.z },
				(vec3) { _cam_up.x, _cam_up.y, _cam_up.z },
				_view_matrix
	);

	glm_mat4_mul(projection_matrix, _view_matrix, _view_projection_matrix);
	RDE_CHECK_GL(glUniformMatrix4fv, glGetUniformLocation(_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_view_projection_matrix);
	rde_vec_2I _mouse_pos = current_drawing_window->mouse_position;
	RDE_CHECK_GL(glUniform2f, glGetUniformLocation(_shader->compiled_program_id, "mouse_position"), _mouse_pos.x, _mouse_pos.y);
	RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, "mouse_position"), ENGINE.delta_time);
	
	RDE_CHECK_GL(glBindVertexArray, current_batch_3d.line_batch.vertex_array_object);
	
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, current_batch_3d.line_batch.vertex_buffer_object);
	RDE_CHECK_GL(glBufferSubData, GL_ARRAY_BUFFER, 0, current_batch_3d.line_batch.amount_of_vertices * sizeof(rde_line_vertex), current_batch_3d.line_batch.vertices);

	RDE_CHECK_GL(glDrawArrays, GL_LINES, 0, current_batch_3d.line_batch.amount_of_vertices);
}

void rde_inner_rendering_flush_batch_3d() {
	if(current_batch_3d.shader == NULL || current_batch_3d.mesh == NULL || current_batch_3d.amount_of_models_per_draw == 0) {
		return;
	}

	rde_mesh* _mesh = current_batch_3d.mesh;
	rde_shader* _shader = current_batch_3d.shader;

	if(!rde_inner_rendering_is_mesh_ok_to_render(_mesh)) {
		return;
	}

	RDE_CHECK_GL(glUseProgram, current_batch_3d.shader->compiled_program_id);

	mat4 _view_projection_matrix = GLM_MAT4_IDENTITY_INIT;

	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_vec_3F _cam_pos = current_drawing_camera->transform->position;
	rde_vec_3F _cam_direction = current_drawing_camera->direction;
	rde_vec_3F _cam_up = current_drawing_camera->up;
	glm_lookat( (vec3) { _cam_pos.x, _cam_pos.y, _cam_pos.z },
				(vec3) { _cam_pos.x + _cam_direction.x, _cam_pos.y + _cam_direction.y, _cam_pos.z + _cam_direction.z },
				(vec3) { _cam_up.x, _cam_up.y, _cam_up.z },
				_view_matrix
	);

	glm_mat4_mul(projection_matrix, _view_matrix, _view_projection_matrix);

	GLint _using_render_texture_location = glGetUniformLocation(_shader->compiled_program_id, "material.using_render_texture");
	if(_mesh->material.render_texture != NULL) {
		RDE_CHECK_GL(glUseProgram, current_batch_3d.shader->compiled_program_id);

		GLint _rt_location = glGetUniformLocation(_shader->compiled_program_id, "render_texture");
		RDE_CHECK_GL(glUniform1i, _rt_location, 0);

		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE0);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _mesh->material.render_texture->opengl_texture_id);

		RDE_CHECK_GL(glUniform1i, _using_render_texture_location, 1);
	} else {
		rde_texture* _ka_texture = _mesh->material.map_ka != NULL ? _mesh->material.map_ka : DEFAULT_TEXTURE;
		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE0);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _ka_texture->opengl_texture_id);

		rde_texture* _kd_texture = _mesh->material.map_kd != NULL ? _mesh->material.map_kd : DEFAULT_TEXTURE;
		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE1);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _kd_texture->opengl_texture_id);

		rde_texture* _ks_texture = _mesh->material.map_ks != NULL ? _mesh->material.map_ks : DEFAULT_TEXTURE;
		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE2);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _ks_texture->opengl_texture_id);

		rde_texture* _bump_texture = _mesh->material.map_bump != NULL ? _mesh->material.map_bump : DEFAULT_TEXTURE;
		RDE_CHECK_GL(glActiveTexture, GL_TEXTURE3);
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _bump_texture->opengl_texture_id);

		GLint _ka_location = glGetUniformLocation(_shader->compiled_program_id, "tex_ka");
		RDE_CHECK_GL(glUniform1i, _ka_location, 0);
		GLint _kd_location = glGetUniformLocation(_shader->compiled_program_id, "tex_kd");
		RDE_CHECK_GL(glUniform1i, _kd_location, 1);
		GLint _ks_location = glGetUniformLocation(_shader->compiled_program_id, "tex_ks");
		RDE_CHECK_GL(glUniform1i, _ks_location, 2);
		GLint _bump_location = glGetUniformLocation(_shader->compiled_program_id, "tex_bump");
		RDE_CHECK_GL(glUniform1i, _bump_location, 3);
		RDE_CHECK_GL(glUniform1i, _using_render_texture_location, 0);
	}

	RDE_CHECK_GL(glUniformMatrix4fv, glGetUniformLocation(_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_view_projection_matrix);
	rde_vec_2I _mouse_pos = current_drawing_window->mouse_position;
	RDE_CHECK_GL(glUniform2f, glGetUniformLocation(_shader->compiled_program_id, "mouse_position"), _mouse_pos.x, _mouse_pos.y);
	RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, "mouse_position"), ENGINE.delta_time);

	rde_vec_3F _camera_pos = current_drawing_camera->transform->position;
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "camera_pos"), _camera_pos.x, _camera_pos.y, _camera_pos.z);
	
	rde_vec_3F _dl_direction = ENGINE.illumination.directional_light.direction;
	rde_vec_3F _dl_ambient = ENGINE.illumination.directional_light.ambient_color;
	rde_vec_3F _dl_diffuse = ENGINE.illumination.directional_light.diffuse_color;
	rde_vec_3F _dl_specular = ENGINE.illumination.directional_light.specular_color;

	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "directional_light.direction"), _dl_direction.x, _dl_direction.y, _dl_direction.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "directional_light.ambient_color"), _dl_ambient.x, _dl_ambient.y, _dl_ambient.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "directional_light.diffuse_color"), _dl_diffuse.x, _dl_diffuse.y, _dl_diffuse.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "directional_light.specular_color"), _dl_specular.x, _dl_specular.y, _dl_specular.z);

	rde_material_light_data _material = _mesh->material.material_light_data;
	RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, "material.shininess"), _material.shininess);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "material.Ka"), _material.ka.x, _material.ka.y, _material.ka.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "material.Kd"), _material.kd.x, _material.kd.y, _material.kd.z);
	RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, "material.Ks"), _material.ks.x, _material.ks.y, _material.ks.z);

	for(size_t _i = 0; _i < ENGINE.init_info.illumination_config.max_amount_of_point_lights; _i++) {
		rde_point_light* _p = ENGINE.illumination.point_lights[_i];
		char _point_light_var[256];
		memset(_point_light_var, 0, 256);

		if(_p != NULL) {
			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].position", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->position.x, _p->position.y, _p->position.z);
			memset(_point_light_var, 0, 256);
		
			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].ambient_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->ambient_color.x, _p->ambient_color.y, _p->ambient_color.z);
			memset(_point_light_var, 0, 256);
		
			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].diffuse_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->diffuse_color.x, _p->diffuse_color.y, _p->diffuse_color.z);
			memset(_point_light_var, 0, 256);

			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].specular_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->specular_color.x, _p->specular_color.y, _p->specular_color.z);
			memset(_point_light_var, 0, 256);

			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].constant", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->constant);
			memset(_point_light_var, 0, 256);

			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].linear", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->linear);
			memset(_point_light_var, 0, 256);

			rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].quadratic", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _p->quadratic);
			memset(_point_light_var, 0, 256);
		}

		rde_util_string_concat(_point_light_var, 256, "point_lights[%zu].used", _i);
		RDE_CHECK_GL(glUniform1i, glGetUniformLocation(_shader->compiled_program_id, _point_light_var), _i < ENGINE.init_info.illumination_config.max_amount_of_point_lights && _p != NULL ? 1 : -1);

		memset(_point_light_var, 0, 256);
	}

	for(size_t _i = 0; _i < ENGINE.init_info.illumination_config.max_amount_of_spot_lights; _i++) {
		rde_spot_light* _s = ENGINE.illumination.spot_lights[_i];
		char _spot_light_var[256];
		memset(_spot_light_var, 0, 256);

		if(_s != NULL) {
			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].position", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->position.x, _s->position.y, _s->position.z);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].direction", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->direction.x, _s->direction.y, _s->direction.z);
			memset(_spot_light_var, 0, 256);
		
			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].ambient_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->ambient_color.x, _s->ambient_color.y, _s->ambient_color.z);
			memset(_spot_light_var, 0, 256);
		
			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].diffuse_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->diffuse_color.x, _s->diffuse_color.y, _s->diffuse_color.z);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].specular_color", _i);
			RDE_CHECK_GL(glUniform3f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->specular_color.x, _s->specular_color.y, _s->specular_color.z);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].constant", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->constant);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].linear", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->linear);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].quadratic", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _s->quadratic);
			memset(_spot_light_var, 0, 256);

			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].cut_off", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), cos(glm_rad(_s->cut_off)));
			memset(_spot_light_var, 0, 256);

			
			rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].outer_cut_off", _i);
			RDE_CHECK_GL(glUniform1f, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), cos(glm_rad(_s->outer_cut_off)));
			memset(_spot_light_var, 0, 256);
		}

		rde_util_string_concat(_spot_light_var, 256, "spot_lights[%zu].used", _i);
		RDE_CHECK_GL(glUniform1i, glGetUniformLocation(_shader->compiled_program_id, _spot_light_var), _i < ENGINE.init_info.illumination_config.max_amount_of_spot_lights && _s != NULL ? 1 : -1);

		memset(_spot_light_var, 0, 256);
	}

	RDE_CHECK_GL(glBindVertexArray, _mesh->vao);

	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[3]);
	RDE_CHECK_GL(glBufferSubData, GL_ARRAY_BUFFER, 0, current_batch_3d.amount_of_models_per_draw * (sizeof(float) * 16), _mesh->transformation_matrices);

	RDE_CHECK_GL(glDrawArraysInstanced, GL_TRIANGLES, 0, _mesh->vertex_count, current_batch_3d.amount_of_models_per_draw);
}

void rde_inner_rendering_try_flush_line_batch(rde_shader* _shader, unsigned short _thickness, size_t _extra_floats) {
	bool _lines_ok = current_batch_3d.line_batch.amount_of_vertices + _extra_floats <= RDE_MAX_LINES_PER_DRAW;
	bool _shader_ok = current_batch_3d.line_batch.shader == _shader;
	bool _thickness_ok = current_batch_3d.line_batch.thickness == _thickness;
	if(_shader_ok && _lines_ok && _thickness_ok) {
		return;
	}

	rde_inner_rendering_flush_line_batch();
	rde_inner_rendering_reset_line_batch();
	rde_inner_rendering_try_create_line_batch(_shader, _thickness);
}

void rde_inner_rendering_try_flush_batch_3d(rde_shader* _shader, rde_mesh* _mesh, size_t _extra_floats) {
	bool _models_ok = current_batch_3d.amount_of_models_per_draw + 1 <= RDE_MAX_MODELS_PER_DRAW;
	bool _shader_ok = current_batch_3d.shader == _shader;
	bool _mesh_ok = current_batch_3d.mesh == _mesh;
	bool _transforms_ok = current_batch_3d.amount_of_models_per_draw * _extra_floats + _extra_floats <= RDE_MAX_MODELS_PER_DRAW * _extra_floats;
	if(_models_ok && _shader_ok && _transforms_ok && _mesh_ok) {
		return;
	}

#if !IS_ANDROID()
    if(current_batch_3d.draw_mesh_wireframe) {
		RDE_CHECK_GL(glPolygonMode, GL_FRONT_AND_BACK, GL_LINE);
		rde_inner_rendering_flush_batch_3d();
		RDE_CHECK_GL(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL);
	}
#endif

	rde_inner_rendering_flush_batch_3d();
	rde_inner_rendering_reset_batch_3d();
	rde_inner_rendering_try_create_batch_3d(_shader, _mesh);
}

void rde_inner_rendering_flush_render_texture_3d() {
	rde_inner_rendering_flush_batch_3d();
	rde_inner_rendering_reset_batch_3d();
	rde_inner_rendering_flush_line_batch();
	rde_inner_rendering_reset_line_batch();
}


// ==============================================================================
// =							PUBLIC API - RENDERING					 	 =
// ==============================================================================

rde_shader* rde_rendering_shader_load(const char* _name, const char* _vertex_code, const char* _fragment_code) {
	bool _error = false;

	GLuint _vertex_program_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint _fragment_program_id = glCreateShader(GL_FRAGMENT_SHADER);

	GLint _vertex_source_size = strlen(_vertex_code);
	GLint _fragment_source_size = strlen(_fragment_code);

	RDE_CHECK_GL(glShaderSource, _vertex_program_id, 1, &_vertex_code, &_vertex_source_size);
	RDE_CHECK_GL(glShaderSource, _fragment_program_id, 1, &_fragment_code, &_fragment_source_size);

	RDE_CHECK_GL(glCompileShader, _vertex_program_id);
	RDE_CHECK_GL(glCompileShader, _fragment_program_id);

	GLint _is_vertex_compiled, _is_fragment_compiled;
	RDE_CHECK_GL(glGetShaderiv, _vertex_program_id, GL_COMPILE_STATUS, &_is_vertex_compiled);
	RDE_CHECK_GL(glGetShaderiv, _fragment_program_id, GL_COMPILE_STATUS, &_is_fragment_compiled);

	RDE_CHECK_SHADER_COMPILATION_STATUS( _vertex_program_id, _is_vertex_compiled, _vertex_code)
	RDE_CHECK_SHADER_COMPILATION_STATUS(_fragment_program_id, _is_fragment_compiled, _fragment_code)

	GLuint _program_id = glCreateProgram();
	RDE_CHECK_GL(glAttachShader, _program_id, _vertex_program_id);
	RDE_CHECK_GL(glAttachShader, _program_id, _fragment_program_id);

	RDE_CHECK_GL(glLinkProgram, _program_id);

	if (_error) {
		printf("%s \n\n %s", _vertex_code, _fragment_code);
		return NULL;
	}

	for (size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_shaders; _i++) {
		if (ENGINE.shaders[_i].compiled_program_id != -1) {
			continue;
		}

		rde_shader* _shader = &ENGINE.shaders[_i];
		_shader->vertex_program_id = _vertex_program_id;
		_shader->fragment_program_id = _fragment_program_id;
		_shader->compiled_program_id = _program_id;
		rde_strcat(_shader->name, RDE_SHADER_MAX_NAME, _name);

		rde_log_level(RDE_LOG_LEVEL_INFO, "Loaded shader with index '%d' successfully", _shader->compiled_program_id);
		return _shader;
	}

	rde_critical_error(true, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "shaders", ENGINE.init_info.heap_allocs_config.max_amount_of_shaders);
	return NULL;
}

void rde_rendering_shader_set_uniform_value_float(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_FV_ _type, float* _data, bool _transpose) {
	GLint _location = glGetUniformLocation((GLuint)_shader->compiled_program_id, _uniform_name);
	if(_location >= 0) {
		switch(_type) {
			case RDE_UNIFORM_FV_1 		   : glUniform1fv(_location, 1, _data); break;
			case RDE_UNIFORM_FV_2 		   : glUniform2fv(_location, 1, _data); break;
			case RDE_UNIFORM_FV_3 		   : glUniform3fv(_location, 1, _data); break;
			case RDE_UNIFORM_FV_4 		   : glUniform4fv(_location, 1, _data); break;
			case RDE_UNIFORM_FV_MATRIX_2   : glUniformMatrix2fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_3   : glUniformMatrix3fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_4   : glUniformMatrix4fv(_location, 1, _transpose, _data); break;
			
#if !IS_WASM()
			case RDE_UNIFORM_FV_MATRIX_2x3 : glUniformMatrix2x3fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_3x2 : glUniformMatrix3x2fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_3x4 : glUniformMatrix3x4fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_4x3 : glUniformMatrix4x3fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_2x4 : glUniformMatrix2x4fv(_location, 1, _transpose, _data); break;
			case RDE_UNIFORM_FV_MATRIX_4x2 : glUniformMatrix4x2fv(_location, 1, _transpose, _data); break;
#else
			default: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to set uniform '%s' of type '%d' in WASM and is not supported", _uniform_name, (int)_type);
			}
#endif
		}
	} else {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to set uniform '%s', but it could not be located", _uniform_name);
	}
}

void rde_rendering_shader_set_uniform_value_int(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_IV_ _type, int* _data) {
	GLint _location = glGetUniformLocation((GLuint)_shader->compiled_program_id, _uniform_name);
	if(_location >= 0) {
		switch(_type) {
			case RDE_UNIFORM_IV_1 : glUniform1iv(_location, 1, _data); break;
			case RDE_UNIFORM_IV_2 : glUniform2iv(_location, 1, _data); break;
			case RDE_UNIFORM_IV_3 : glUniform3iv(_location, 1, _data); break;
			case RDE_UNIFORM_IV_4 : glUniform4iv(_location, 1, _data); break;
		}
	} else {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to set uniform '%s', but it could not be located", _uniform_name);
	}
}

void rde_rendering_shader_set_uniform_value_uint(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_UIV_ _type, uint* _data) {
	GLint _location = glGetUniformLocation((GLuint)_shader->compiled_program_id, _uniform_name);
	if(_location >= 0) {
		switch(_type) {
			case RDE_UNIFORM_UIV_1 : glUniform1uiv(_location, 1, _data); break;
			case RDE_UNIFORM_UIV_2 : glUniform2uiv(_location, 1, _data); break;
			case RDE_UNIFORM_UIV_3 : glUniform3uiv(_location, 1, _data); break;
			case RDE_UNIFORM_UIV_4 : glUniform4uiv(_location, 1, _data); break;
		}
	} else {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "Tried to set uniform '%s', but it could not be located", _uniform_name);
	}
}

rde_shader_data rde_rendering_shader_get_data(rde_shader* _shader) {
	rde_critical_error(_shader == NULL, RDE_ERROR_NO_NULL_ALLOWED, "shader");
	return (rde_shader_data) {
		.vertex_program_id = _shader->vertex_program_id,
		.fragment_program_id = _shader->fragment_program_id,
		.compiled_program_id = _shader->compiled_program_id
	};
}

rde_shader* rde_rendering_shader_get_by_name(const char* _name) {
	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_shaders; _i++) {
		rde_shader* _shader = &ENGINE.shaders[_i];
		if(_shader != NULL && strcmp(_shader->name, _name) == 0) {
			return _shader;
		}
	}

	return NULL;
}

void rde_rendering_shader_unload(rde_shader* _shader) {
	rde_critical_error(_shader == NULL, RDE_ERROR_NO_NULL_ALLOWED, "shader");

	RDE_CHECK_GL(glDeleteShader, _shader->vertex_program_id);
	RDE_CHECK_GL(glDeleteShader, _shader->fragment_program_id);
	RDE_CHECK_GL(glDeleteProgram, _shader->compiled_program_id);

	_shader->compiled_program_id = -1;
}

rde_texture_parameters rde_innner_rendering_validate_texture_parameters(const rde_texture_parameters* _params) {
	rde_texture_parameters _tex_params = _params == NULL ? RDE_DEFAULT_TEXTURE_PARAMETERS : *_params;
#define RDE_TEXTURE_PARAM_CHECKER(_field, _min, _max, _warn_text) 																								\
	if(_tex_params._field < _min || _tex_params._field > _max) {																								\
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_RENDERING_WRONG_TEXTURE_PARAM, _tex_params._field, _warn_text, RDE_DEFAULT_TEXTURE_PARAMETERS._field);	\
		_tex_params._field = RDE_DEFAULT_TEXTURE_PARAMETERS._field;																								\
	}

	RDE_TEXTURE_PARAM_CHECKER(min_filter, RDE_TEXTURE_PARAMETER_TYPE_FILTER_NEAREST, RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR, "MIN_FILTER")
	RDE_TEXTURE_PARAM_CHECKER(mag_filter, RDE_TEXTURE_PARAMETER_TYPE_FILTER_NEAREST, RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR, "MAG_FILTER")
	RDE_TEXTURE_PARAM_CHECKER(wrap_s, RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT, RDE_TEXTURE_PARAMETER_TYPE_WRAP_MIRRORED_REPEAT, "WRAP_S")
	RDE_TEXTURE_PARAM_CHECKER(wrap_t, RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT, RDE_TEXTURE_PARAMETER_TYPE_WRAP_MIRRORED_REPEAT, "WRAP_T")
	RDE_TEXTURE_PARAM_CHECKER(mipmap_min_filter, RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NEAREST_MIN_FILTER_NEAREST, RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NONE, "MIPMAP_MIN_FILTER")

	return _tex_params;
}

rde_texture* rde_rendering_texture_load(const char* _file_path, const rde_texture_parameters* _params) {
	const char* _extension = rde_util_file_get_name_extension(_file_path);
	char _extension_lower[10];
	memset(_extension_lower, 0, 10);
	rde_util_string_to_lower(_extension_lower, _extension);
	rde_critical_error(strcmp(_extension_lower, "jpeg") != 0 && strcmp(_extension_lower, "jpg") != 0 &&
	                   strcmp(_extension_lower, "png") != 0, RDE_ERROR_RENDERING_TEXTURE_UNSUPPORTED_FORMAT, _file_path, _extension);

	rde_texture* _texture = NULL;

	char _sanitized_path[RDE_MAX_PATH];
	memset(_sanitized_path, 0, RDE_MAX_PATH);
	rde_util_file_sanitaize_path(_file_path, _sanitized_path, RDE_MAX_PATH);

	for (size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if (strlen(ENGINE.textures[_i].file_path) != 0 && strcmp(ENGINE.textures[_i].file_path, _sanitized_path) == 0) {
			rde_log_level(RDE_LOG_LEVEL_WARNING, "Path '%s' already loaded in texture", _sanitized_path);
			return &ENGINE.textures[_i];
		}
	}

	for (size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if (ENGINE.textures[_i].opengl_texture_id >= 0) {
			continue;
		}

		_texture = &ENGINE.textures[_i];
		break;
	}

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "textures", ENGINE.total_amount_of_textures);

	int _width, _height, _channels;
	stbi_set_flip_vertically_on_load(1);

#if IS_IOS()
	stbi_convert_iphone_png_to_rgb(1);
	stbi_set_unpremultiply_on_load(1);
#endif

	stbi_uc* _data = NULL;
	
#if IS_MOBILE()
	while(rde_util_string_contains_substring(_sanitized_path, "../", false)) {
		size_t _sp_size = strlen(_sanitized_path);
		size_t _init_point = 0;
		size_t _end_point = 0;

		for(size_t _i = 0; _i < _sp_size; _i++) {
			if(_sanitized_path[_i] == '/' &&
			   _i + 1 < _sp_size && _sanitized_path[_i + 1] != '.' &&
			   _i + 2 < _sp_size && _sanitized_path[_i + 2] != '.' ) {
				_init_point = _i;
			}

			if(_sanitized_path[_i] == '.' &&
			   _i + 1 < _sp_size && _sanitized_path[_i + 1] == '.' &&
			   _i + 2 < _sp_size && _sanitized_path[_i + 2] == '/') {
				_end_point = _i + 2;
				break;
			}
		}

		rde_critical_error(_init_point == _end_point, "Error converting path");

		char _new_path[RDE_MAX_PATH];
		memset(_new_path, 0, RDE_MAX_PATH);
		rde_strncat(_new_path, RDE_MAX_PATH, _sanitized_path, _init_point);
		rde_strncat(_new_path, RDE_MAX_PATH, _sanitized_path + _end_point, _sp_size - _end_point);
		memset(_sanitized_path, 0, RDE_MAX_PATH);
		rde_strcpy(_sanitized_path, RDE_MAX_PATH, _new_path);
		rde_log_level(RDE_LOG_LEVEL_INFO, "Removed a .. from path: %s", _sanitized_path);
	}

	rde_log_level(RDE_LOG_LEVEL_INFO, "This is the new path: %s", _sanitized_path);
	size_t _total_size = 0;
	rde_file_handle* _file_handle = rde_file_open(_sanitized_path, RDE_FILE_MODE_READ);
	const unsigned char* _texture_data = rde_file_read_full_file_bytes(_file_handle, &_total_size);
	_data = stbi_load_from_memory(_texture_data, _total_size, &_width, &_height, &_channels, (strcmp(_extension, "png") == 0 ? 4 : 3));
#else
	_data = stbi_load(_sanitized_path, &_width, &_height, &_channels, (strcmp(_extension, "png") == 0 ? 4 : 3));
#endif

	rde_critical_error(_data == NULL, "Could not load texture at '%s'", _sanitized_path);

	RDE_CHECK_GL(glPixelStorei, GL_UNPACK_ALIGNMENT, 4);
	GLenum _internal_format = 0;
	GLenum _data_format = 0;
	if (strcmp(_extension, "png") == 0) {
		_internal_format = GL_RGBA8;
		_data_format = GL_RGBA;
	} else {
		RDE_CHECK_GL(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);
		_internal_format = GL_RGB8;
		_data_format = GL_RGB;
	}

	rde_texture_parameters _tex_params = rde_innner_rendering_validate_texture_parameters(_params);
	
	GLuint _texture_id;
	RDE_CHECK_GL(glGenTextures, 1, &_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _texture_id);

	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _tex_params.min_filter);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _tex_params.mag_filter);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _tex_params.wrap_s);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _tex_params.wrap_t);

	if(_tex_params.mipmap_min_filter != RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NONE) {
		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _tex_params.mipmap_min_filter);
	}

	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, _internal_format, _width, _height, 0, _data_format, GL_UNSIGNED_BYTE, _data);

	if(_tex_params.mipmap_min_filter != RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NONE) {
		RDE_CHECK_GL(glGenerateMipmap, GL_TEXTURE_2D);
	}
	
	stbi_image_free(_data);

	_texture->opengl_texture_id = _texture_id;
	_texture->size = (rde_vec_2UI){ (uint)_width, (uint)_height };
	_texture->channels = _channels;
	_texture->internal_format = _internal_format;
	_texture->data_format = _data_format;
	rde_strcpy(_texture->file_path, RDE_MAX_PATH, _sanitized_path);

	rde_log_color(RDE_LOG_COLOR_GREEN, "Texture at '%s' loaded correctly: ", _sanitized_path);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Size: %dx%d: ", _width, _height);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Channels: %d: ", _channels);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- OpenGL ID: %u: ", _texture_id);
	
	if(!rde_engine_logs_supressed()) {
		printf("\n");
	}

#if IS_MOBILE()
	rde_file_close(_file_handle);
#endif

	return _texture;
}

rde_texture* rde_rendering_texture_text_load(const char* _file_path) {
	rde_texture* _texture = NULL;

	for (size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if (strlen(ENGINE.textures[_i].file_path) != 0 && strcmp(ENGINE.textures[_i].file_path, _file_path) == 0) {
			return &ENGINE.textures[_i];
		}
	}

	for(size_t _i = 0; _i < ENGINE.total_amount_of_textures; _i++) {
		if(ENGINE.textures[_i].opengl_texture_id >= 0) {
			continue;
		}

		_texture = &ENGINE.textures[_i];
		break;
	}

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "textures", ENGINE.total_amount_of_textures);

	int _width, _height, _channels;
	stbi_set_flip_vertically_on_load(1);

#if IS_IOS()
	stbi_convert_iphone_png_to_rgb(1); 
	stbi_set_unpremultiply_on_load(1);
#endif

	stbi_uc* _data = NULL;
	_data = stbi_load(_file_path, &_width, &_height, &_channels, 0);

	if(_data == NULL) {
		printf("Error while loading texture at '%s' \n", _file_path);
		return NULL;
	}

	GLuint _texture_id;
	RDE_CHECK_GL(glGenTextures, 1, &_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _texture_id);
#if IS_MOBILE() || defined(__EMSCRIPTEN__)
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_ALPHA, _width, _height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _data);
	_texture->internal_format = GL_ALPHA;
	_texture->data_format = GL_ALPHA;
#else
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, _data);
	_texture->internal_format = GL_RED;
	_texture->data_format = GL_RED;
#endif

	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(_data);

	_texture->opengl_texture_id = _texture_id;
	_texture->size = (rde_vec_2UI){ (uint)_width, (uint)_height };
	_texture->channels = _channels;
	rde_strcpy(_texture->file_path, RDE_MAX_PATH, _file_path);

	rde_log_color(RDE_LOG_COLOR_GREEN, "Texture at '%s' loaded correctly: ", _file_path);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Size: %dx%d: ", _width, _height);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- Channels: %d: ", _channels);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	- OpenGL ID: %u: ", _texture_id);
	if(!rde_engine_logs_supressed()) {
		printf("\n");
	}

	return _texture;
}

rde_texture_data rde_rendering_texture_get_data(rde_texture* _texture) {
	rde_critical_error(_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "texture");
	return (rde_texture_data) {
		.opengl_texture_id = _texture->opengl_texture_id,
		.size = _texture->size,
		.position = _texture->position,
		.channels = _texture->channels,
		.file_path = _texture->file_path,
		.atlas_texture_id = _texture->atlas_texture->opengl_texture_id
	};
}

rde_texture* rde_rendering_memory_texture_create(size_t _width, size_t _height, int _channels) {
	rde_texture* _texture = (rde_texture*)malloc(sizeof(rde_texture));
	rde_struct_init_alloc_ptr_texture(_texture);

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "textures", ENGINE.total_amount_of_textures);

	_texture->pixels = (unsigned char*)malloc(_width * _height * _channels);
	rde_critical_error(_texture->pixels == NULL, RDE_ERROR_NO_MEMORY, _width * _height * _channels, "texture pixels array");
	memset(_texture->pixels, 0, _width * _height * _channels);
	_texture->size = (rde_vec_2UI) { _width, _height };
	_texture->channels = _channels;

	return _texture;
}

void rde_rendering_memory_texture_set_pixel(rde_texture* _memory_texture, rde_vec_2I _position, rde_color _color) {
	rde_critical_error(_memory_texture == NULL || _memory_texture->pixels == NULL, RDE_ERROR_NO_NULL_ALLOWED, "memory texture or memory texture pixels");

	rde_vec_2UI _t_size = _memory_texture->size;
	int _channels = _memory_texture->channels;
	_memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 0] = _color.r;
	_memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 1] = _color.g;
	_memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 2] = _color.b;
	
	if(_channels == 4) {
		_memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 3] = _color.a;
	}

	_memory_texture->pixels_changed = true;
}

rde_color rde_rendering_memory_texture_get_pixel(rde_texture* _memory_texture, rde_vec_2I _position) {
	rde_critical_error(_memory_texture == NULL || _memory_texture->pixels == NULL, RDE_ERROR_NO_NULL_ALLOWED, "memory texture or memory texture pixels");

	rde_vec_2UI _t_size = _memory_texture->size;
	int _channels = _memory_texture->channels;
	rde_color _color;
	_color.r = _memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 0];
	_color.g = _memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 1];
	_color.b = _memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 2];
	
	if(_channels == 4) {
		_color.a = _memory_texture->pixels[_position.x * _channels + _position.y * _t_size.x * _channels + 3];
	}

	return _color;
}

void rde_rendering_memory_texture_gpu_upload(rde_texture* _memory_texture) {
	if(_memory_texture->opengl_texture_id == -1) {
		GLenum _internal_format = 0, _data_format = 0;
		if (_memory_texture->channels == 4) {
			_internal_format = GL_RGBA8;
			_data_format = GL_RGBA;
		} else if (_memory_texture->channels == 3) {
			_internal_format = GL_RGB8;
			_data_format = GL_RGB;
		}

		_memory_texture->internal_format = _internal_format;
		_memory_texture->data_format = _data_format;
		
		GLuint _id;
		RDE_CHECK_GL(glGenTextures, 1, &_id);

		_memory_texture->opengl_texture_id = _id;
		RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _memory_texture->opengl_texture_id);

		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, (int)_memory_texture->internal_format, _memory_texture->size.x, _memory_texture->size.y, 0, _memory_texture->data_format, GL_UNSIGNED_BYTE, _memory_texture->pixels);
	} else {
		if(_memory_texture->pixels_changed) {
			RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _memory_texture->opengl_texture_id);
			RDE_CHECK_GL(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, _memory_texture->size.x, _memory_texture->size.y, _memory_texture->data_format, GL_UNSIGNED_BYTE, _memory_texture->pixels);
			_memory_texture->pixels_changed = false;
		}
	}
}

unsigned char* rde_rendering_memory_texture_get_pixels(rde_texture* _memory_texture) {
	rde_critical_error(_memory_texture == NULL || _memory_texture->pixels == NULL, RDE_ERROR_NO_NULL_ALLOWED, "memory texture or memory texture pixels");
	return _memory_texture->pixels;
}

void rde_rendering_texture_unload(rde_texture* _texture) {
	rde_critical_error(_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "texture");
	GLuint _id = (GLuint)_texture->opengl_texture_id;

	if(_id < 0) {
		return;
	}

	glDeleteTextures(1, &_id);
	_texture->opengl_texture_id = -1;
	_texture->size = (rde_vec_2UI){ 0, 0 };
	_texture->channels = 0;
	_texture->internal_format = 0;
	_texture->data_format = 0;
	memset(_texture->file_path, 0, RDE_MAX_PATH);
}

rde_atlas* rde_rendering_atlas_load(const char* _texture_path) {
	char _config_path_ext[RDE_MAX_PATH];
	memset(_config_path_ext, 0, RDE_MAX_PATH);
	
	char _texture_path_ext[RDE_MAX_PATH];
	memset(_texture_path_ext, 0, RDE_MAX_PATH);

	rde_snprintf(_config_path_ext, RDE_MAX_PATH, "%s.rde_atlas", _texture_path);
	rde_snprintf(_texture_path_ext, RDE_MAX_PATH, "%s.png", _texture_path);

	rde_texture* _texture = rde_rendering_texture_load(_texture_path_ext, NULL);
	rde_atlas_sub_textures* _atlas_sub_textures = rde_inner_file_system_read_atlas_config(_config_path_ext, _texture);

	for (size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_atlases; _i++) {
		if (ENGINE.atlases[_i].texture != NULL && strcmp(ENGINE.atlases[_i].texture->file_path, _texture_path) == 0) {
			return &ENGINE.atlases[_i];
		}
	}

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_atlases; _i++) {
		rde_atlas* _atlas = &ENGINE.atlases[_i];
		if(_atlas->texture != NULL) {
			continue;
		}

		_atlas->texture = _texture;
		_atlas->sub_textures = _atlas_sub_textures;
		return _atlas;
	}

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "atlases", ENGINE.total_amount_of_textures);
	return NULL;
}

rde_texture* rde_rendering_atlas_get_subtexture(rde_atlas* _atlas, const char* _texture_name) {
	bool _exists = stbds_shgeti(_atlas->sub_textures, _texture_name) != -1;
	if(!_exists) {
		rde_critical_error(true, RDE_ERROR_ATLAS_SUB_TEXTURE, _texture_name, _atlas->texture->file_path);
	}

	return &stbds_shget(_atlas->sub_textures, _texture_name);
}

rde_atlas_data rde_rendering_atlas_get_data(rde_atlas* _atlas) {
	rde_critical_error(_atlas == NULL, RDE_ERROR_NO_NULL_ALLOWED, "atlas");
	return (rde_atlas_data) {
		.opengl_texture_id = _atlas->texture->opengl_texture_id,
		.amount_of_subtextures = stbds_arrlenu(_atlas->sub_textures)
	};
}

void rde_rendering_atlas_unload(rde_atlas* _atlas) {
	rde_critical_error(_atlas == NULL, RDE_ERROR_NO_NULL_ALLOWED, "atlas");
	stbds_shfree(_atlas->sub_textures);
	_atlas->sub_textures = NULL;
	rde_rendering_texture_unload(_atlas->texture);
	_atlas->texture = NULL;
}

void rde_rendering_memory_texture_destroy(rde_texture* _memory_texture) {
	rde_critical_error(_memory_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "memory texture");
	free(_memory_texture->pixels);
	_memory_texture->pixels = NULL;
	_memory_texture->pixels_changed = false;
	if(_memory_texture->opengl_texture_id != -1) {
		rde_rendering_texture_unload(_memory_texture);
	}
}

rde_font* rde_rendering_font_load(const char* _font_path) {
	char _config_path_ext[RDE_MAX_PATH];
	memset(_config_path_ext, 0, RDE_MAX_PATH);
	
	char _texture_path_ext[RDE_MAX_PATH];
	memset(_texture_path_ext, 0, RDE_MAX_PATH);

	rde_snprintf(_config_path_ext, RDE_MAX_PATH, "%s.rde_font", _font_path);
	rde_snprintf(_texture_path_ext, RDE_MAX_PATH, "%s.png", _font_path);

	rde_texture* _texture = rde_rendering_texture_text_load(_texture_path_ext);
	rde_font_char_info* _chars = rde_inner_file_system_read_font_config(_config_path_ext, _texture);

	for (size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_fonts; _i++) {
		if (ENGINE.fonts[_i].texture != NULL && strcmp(ENGINE.fonts[_i].texture->file_path, _font_path) == 0) {
			return &ENGINE.fonts[_i];
		}
	}

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_fonts; _i++) {
		rde_font* _font = &ENGINE.fonts[_i];
		if(_font->texture != NULL) {
			continue;
		}

		_font->texture = _texture;
		_font->chars = _chars;
		return _font;
	}

	rde_critical_error(_texture == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "fonts", ENGINE.init_info.heap_allocs_config.max_amount_of_fonts);
	return NULL;
}

rde_font_data rde_rendering_font_get_data(rde_font* _font) {
	rde_critical_error(_font == NULL, RDE_ERROR_NO_NULL_ALLOWED, "font");
	return (rde_font_data) {
		.opengl_texture_id = _font->texture->opengl_texture_id,
		.amount_of_chars = stbds_arrlenu(_font->chars)
	};
}

void rde_rendering_font_unload(rde_font* _font) {
	rde_critical_error(_font == NULL, RDE_ERROR_NO_NULL_ALLOWED, "font");
	stbds_arrfree(_font->chars);
	_font->chars = NULL;
	rde_rendering_texture_unload(_font->texture);
	_font->texture = NULL;
}

void rde_rendering_set_background_color(rde_color _color) {
	glClearColor((float)_color.r / 255.f, (float)_color.g / 255.f, (float)_color.b / 255.f, (float)_color.a / 255.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

rde_skybox_id rde_rendering_skybox_load(const char* _texture_paths[6]) {
	if(ENGINE.skybox.vao == RDE_UINT_MAX) {
		float _vertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		RDE_CHECK_GL(glGenVertexArrays, 1, &ENGINE.skybox.vao);
		RDE_CHECK_GL(glGenBuffers, 1, &ENGINE.skybox.vbo);
		RDE_CHECK_GL(glBindVertexArray, ENGINE.skybox.vao);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, ENGINE.skybox.vbo);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(_vertices), &_vertices, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 0);
		RDE_CHECK_GL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	GLuint _texture_id;
	RDE_CHECK_GL(glGenTextures, 1, &_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_CUBE_MAP, _texture_id);

	for(size_t _i = 0; _i < 6; _i++) {
		const char* _extension = rde_util_file_get_name_extension(_texture_paths[_i]);
		char _extension_lower[10];
		memset(_extension_lower, 0, 10);
		rde_util_string_to_lower(_extension_lower, _extension);
		rde_critical_error(strcmp(_extension_lower, "jpeg") != 0 && strcmp(_extension_lower, "jpg") != 0 &&
		                   strcmp(_extension_lower, "png") != 0, RDE_ERROR_RENDERING_TEXTURE_UNSUPPORTED_FORMAT, _texture_paths[_i], _extension);

		int _width, _height, _channels;
		//stbi_set_flip_vertically_on_load(1);

#if IS_IOS()
		stbi_convert_iphone_png_to_rgb(1);
		stbi_set_unpremultiply_on_load(1);
#endif

		stbi_uc* _data = NULL;
		_data = stbi_load(_texture_paths[_i], &_width, &_height, &_channels, (strcmp(_extension, "png") == 0 ? 4 : 3));

		if (_data == NULL) {
			rde_critical_error(true, RDE_ERROR_FILE_NOT_FOUND, _texture_paths[_i]);
			return -1;
		}

		RDE_CHECK_GL(glPixelStorei, GL_UNPACK_ALIGNMENT, 4);
		GLenum _internal_format = 0;
		GLenum _data_format = 0;
		if (strcmp(_extension, "png") == 0) {
			_internal_format = GL_RGBA8;
			_data_format = GL_RGBA;
		} else {
			RDE_CHECK_GL(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);
			_internal_format = GL_RGB8;
			_data_format = GL_RGB;
		}

		RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + _i, 0, _internal_format, _width, _height, 0, _data_format, GL_UNSIGNED_BYTE, _data);

		stbi_image_free(_data);
	}

	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	rde_log_color(RDE_LOG_COLOR_GREEN, "Loaded correctly skybox with textures:");
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Left: %s", _texture_paths[0]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Right: %s", _texture_paths[1]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Top: %s", _texture_paths[2]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Bottom: %s", _texture_paths[3]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Front: %s", _texture_paths[4]);
	rde_log_color(RDE_LOG_COLOR_GREEN, "	Back: %s", _texture_paths[5]);

	return _texture_id;
}

void rde_rendering_skybox_use(rde_skybox_id _skybox_id) {
	ENGINE.skybox.opengl_texture_id = _skybox_id;
}

void rde_rendering_skybox_unload(rde_skybox_id _skybox_id) {
	rde_critical_error(!glIsTexture(_skybox_id), "Tried to delete a texture with negative id");
	RDE_CHECK_GL(glDeleteTextures, 1, &_skybox_id);

	if(ENGINE.skybox.opengl_texture_id == (int)_skybox_id) {
		ENGINE.skybox.opengl_texture_id = -1;
		ENGINE.skybox.vao = RDE_UINT_MAX;
	}
}

rde_render_texture* rde_rendering_render_texture_create(size_t _width, size_t _height) {
	rde_render_texture* _render_texture = (rde_render_texture*)malloc(sizeof(rde_render_texture));
	_render_texture->size = (rde_vec_2UI) { _width, _height };

	RDE_CHECK_GL(glGenFramebuffers, 1, &_render_texture->opengl_framebuffer_id);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, _render_texture->opengl_framebuffer_id);

	RDE_CHECK_GL(glGenTextures, 1, &_render_texture->opengl_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _render_texture->opengl_texture_id);
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, 0);
	RDE_CHECK_GL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture->opengl_texture_id, 0);

#if !IS_MOBILE()
	RDE_CHECK_GL(glDrawBuffer, GL_COLOR_ATTACHMENT0);
#else
	GLenum _a[1] = { GL_COLOR_ATTACHMENT0 };
	RDE_CHECK_GL(glDrawBuffers, 1, _a);
#endif

	RDE_CHECK_GL(glGenRenderbuffers, 1, &_render_texture->opengl_renderbuffer_id);
	RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id); 
	RDE_CHECK_GL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);  
	RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
	RDE_CHECK_GL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id);

	rde_critical_error(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER);

	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);

	return _render_texture;
}

void rde_rendering_render_texture_enable(rde_render_texture* _render_texture) {
	rde_critical_error(_render_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Render Texture");
	current_render_texture = _render_texture;
	if(current_render_texture != NULL) {
		RDE_CHECK_GL(glViewport, 0, 0, current_render_texture->size.x, current_render_texture->size.y);
		RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, current_render_texture->opengl_framebuffer_id);
	}

	RDE_CHECK_GL(glEnable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glClearColor, 0.0f, 0.0f, 0.0f, 0.0f);
	RDE_CHECK_GL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void rde_rendering_render_texture_disable() {
	rde_inner_rendering_flush_render_texture_3d();

	current_render_texture = DEFAULT_RENDER_TEXTURE;
	if(current_render_texture != NULL) {
		RDE_CHECK_GL(glViewport, 0, 0, current_render_texture->size.x, current_render_texture->size.y);
		RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.antialiasing.samples > 0 ? ENGINE.antialiasing.frame_buffer_id : current_render_texture->opengl_framebuffer_id);
	}

	RDE_CHECK_GL(glEnable, GL_DEPTH_TEST);
}


void rde_rendering_render_texture_update(rde_render_texture* _render_texture, size_t _width, size_t _height) {
	rde_critical_error(_render_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Render Texture");
	rde_rendering_render_texture_destroy(_render_texture);

	_render_texture->size = (rde_vec_2UI) { _width, _height };

	RDE_CHECK_GL(glGenFramebuffers, 1, &_render_texture->opengl_framebuffer_id);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, _render_texture->opengl_framebuffer_id);

	RDE_CHECK_GL(glGenTextures, 1, &_render_texture->opengl_texture_id);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, _render_texture->opengl_texture_id);
	RDE_CHECK_GL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	RDE_CHECK_GL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D, 0);
	RDE_CHECK_GL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture->opengl_texture_id, 0);

#if !IS_MOBILE()
	RDE_CHECK_GL(glDrawBuffer, GL_COLOR_ATTACHMENT0);
#else
	GLenum _a[1] = { GL_COLOR_ATTACHMENT0 };
	RDE_CHECK_GL(glDrawBuffers, 1, _a);
#endif

	RDE_CHECK_GL(glGenRenderbuffers, 1, &_render_texture->opengl_renderbuffer_id);
	RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id); 
	RDE_CHECK_GL(glRenderbufferStorage, GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);  
	RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
	RDE_CHECK_GL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _render_texture->opengl_renderbuffer_id);

	rde_critical_error(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER);

	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void rde_rendering_render_texture_destroy(rde_render_texture* _render_texture) {
	rde_critical_error(_render_texture == NULL, RDE_ERROR_NO_NULL_ALLOWED, "Render Texture");

	RDE_CHECK_GL(glDeleteTextures, 1, &_render_texture->opengl_texture_id);
	RDE_CHECK_GL(glDeleteRenderbuffers, 1, &_render_texture->opengl_renderbuffer_id);
	RDE_CHECK_GL(glDeleteFramebuffers, 1, &_render_texture->opengl_framebuffer_id);
}

void rde_rendering_set_antialiasing(rde_window* _window, RDE_ANTIALIASING_ _antialiasing) {

 #if IS_MOBILE()
	if(glFramebufferTexture2DMultisampleEXT == NULL || glRenderbufferStorageMultisampleEXT == NULL || glDiscardFramebufferEXT == NULL) {
		return;
	}
 #endif

	if((int)_antialiasing == ENGINE.antialiasing.samples && ENGINE.antialiasing.frame_buffer_id != RDE_UINT_MAX) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_RENDERING_ANTIALIASING_LEVEL_ALREADY_SET, ENGINE.antialiasing.samples);
		return;
	}

	switch(_antialiasing) {
		case RDE_ANTIALIASING_NONE:
		case RDE_ANTIALIASING_X2:
		case RDE_ANTIALIASING_X4:
		case RDE_ANTIALIASING_X8:
		case RDE_ANTIALIASING_X16:
		case RDE_ANTIALIASING_X32: break;
		default: rde_critical_error(true, RDE_ERROR_RENDERING_WRONG_ANTIALIASING_VALUE, (int)_antialiasing);
	}

	int _max_opengl_supported_samples = 0;
	RDE_CHECK_GL(glGetIntegerv, GL_MAX_SAMPLES, &_max_opengl_supported_samples);
	int _antialiasing_samples = (int)_antialiasing;

	rde_inner_rendering_destroy_current_antialiasing_config();

	while(_antialiasing_samples > _max_opengl_supported_samples) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_RENDERING_ANTIALIASING_LEVEL_NOT_SUPPORTED, _antialiasing_samples, _antialiasing_samples >> 1);
		_antialiasing_samples = _antialiasing_samples >> 1;
	}

	ENGINE.antialiasing.samples = _antialiasing_samples;
	if(ENGINE.antialiasing.samples == 0) {
		rde_log_level(RDE_LOG_LEVEL_INFO, "Disabled antialiasing");
		return;
	}

	rde_vec_2I _screen_size = rde_window_get_window_size(_window);
#if !IS_MOBILE()
    RDE_CHECK_GL(glGenFramebuffers, 1, &ENGINE.antialiasing.frame_buffer_id);
    RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, ENGINE.antialiasing.frame_buffer_id);
    
    unsigned int _texture_multisampled_id;
	RDE_CHECK_GL(glGenTextures, 1, &_texture_multisampled_id);
	ENGINE.antialiasing.opengl_texture_id = _texture_multisampled_id;
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D_MULTISAMPLE, ENGINE.antialiasing.opengl_texture_id);
	RDE_CHECK_GL(glTexImage2DMultisample, GL_TEXTURE_2D_MULTISAMPLE, _antialiasing_samples, GL_RGB, _screen_size.x, _screen_size.y, GL_TRUE);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_2D_MULTISAMPLE, 0);
    RDE_CHECK_GL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ENGINE.antialiasing.opengl_texture_id, 0);
    
    RDE_CHECK_GL(glGenRenderbuffers, 1, &ENGINE.antialiasing.render_buffer_id);
    RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, ENGINE.antialiasing.render_buffer_id);
    RDE_CHECK_GL(glRenderbufferStorageMultisample, GL_RENDERBUFFER, _antialiasing_samples, GL_DEPTH24_STENCIL8, _screen_size.x, _screen_size.y);
    RDE_CHECK_GL(glBindRenderbuffer, GL_RENDERBUFFER, 0);
    RDE_CHECK_GL(glFramebufferRenderbuffer, GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ENGINE.antialiasing.render_buffer_id);
#else
	// From https://registry.khronos.org/OpenGL/extensions/EXT/EXT_multisampled_render_to_texture.txt
	glGenRenderbuffers(1, &ENGINE.antialiasing.render_buffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, ENGINE.antialiasing.render_buffer_id);
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, ENGINE.antialiasing.samples, GL_DEPTH_COMPONENT16, _screen_size.x, _screen_size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	GLuint _texture_id;
	glGenTextures(1, &_texture_id);
	ENGINE.antialiasing.opengl_texture_id = _texture_id;
	glBindTexture(GL_TEXTURE_2D, ENGINE.antialiasing.opengl_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _screen_size.x, _screen_size.y, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &ENGINE.antialiasing.frame_buffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, ENGINE.antialiasing.frame_buffer_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ENGINE.antialiasing.render_buffer_id);
	glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ENGINE.antialiasing.opengl_texture_id, 0, ENGINE.antialiasing.samples);
#endif

	rde_log_level(RDE_LOG_LEVEL_INFO, "Set antialiasing level to %d samples", ENGINE.antialiasing.samples);

	rde_critical_error(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE, RDE_ERROR_RENDERING_INCOMPLETE_FRAMEBUFFER_MSAA);
	RDE_CHECK_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

RDE_ANTIALIASING_ rde_rendering_get_current_antialiasing() {
	return (RDE_ANTIALIASING_)ENGINE.antialiasing.samples;
}

void rde_rendering_2d_begin_drawing(rde_camera* _camera, rde_window* _window, bool _is_hud) {
	rde_critical_error(_camera == NULL || _window == NULL, RDE_ERROR_BEGIN_RENDER);
	current_drawing_camera = _camera;
	current_drawing_window = _window;
	current_batch_2d.texture = rde_struct_create_texture();

	rde_vec_2F _aspect_ratios = rde_inner_rendering_get_aspect_ratio();
	float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.x : _aspect_ratios.y;
	float _zoom = _camera->zoom;
	glm_ortho(-_aspect_ratio * _zoom, _aspect_ratio * _zoom, -_zoom, _zoom, -_zoom, _zoom, projection_matrix);

	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	mat4 _view_matrix_inv = GLM_MAT4_IDENTITY_INIT;

	current_batch_2d.is_hud = _is_hud;

	rde_inner_rendering_transform_to_glm_mat4_2d(current_drawing_camera->transform, _view_matrix);
	if(_is_hud) {
		_view_matrix[3][0] = 0;
		_view_matrix[3][1] = 0;
	}

	glm_mat4_inv(_view_matrix, _view_matrix_inv);
	glm_mat4_mul(projection_matrix, _view_matrix_inv, current_batch_2d.mvp);
}

void rde_rendering_2d_draw_point(rde_vec_2F _position, rde_color _color, rde_shader* _shader) {
	UNUSED(_position);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED()
}

void rde_rendering_3d_draw_point(rde_vec_3F _position, rde_color _color, rde_shader* _shader) {
	UNUSED(_position);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED()
}

void rde_rendering_2d_draw_line(rde_vec_2F _init, rde_vec_2F _end, rde_color _color, rde_shader* _shader) {
	UNUSED(_init);
	UNUSED(_end);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED()
}

void rde_rendering_2d_draw_triangle(rde_vec_2F _vertex_a, rde_vec_2F _vertex_b, rde_vec_2F _vertex_c, rde_color _color, rde_shader* _shader) {
	const size_t _triangle_vertex_count = 3;
	
	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.color_shader_2d : _shader;
	rde_inner_rendering_try_create_batch_2d(_drawing_shader, NULL);
	rde_inner_rendering_try_flush_batch_2d(_drawing_shader, NULL, _triangle_vertex_count);

	int _c = RDE_COLOR_TO_HEX_COLOR(_color);
	rde_vec_2I _window_size = rde_window_get_window_size(current_drawing_window);
	float _aspect_ratio = (float)_window_size.x / (float)_window_size.y;

	// * 2
	// |\
	// | \
	// *--*
	// 0   1
	rde_vertex_2d _vertex_0;
	_vertex_0.position = (rde_vec_3F){ _vertex_a.x, _vertex_a.y / _aspect_ratio, 0.f };
	_vertex_0.color = _c;
	_vertex_0.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_0.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0;

	rde_vertex_2d _vertex_1;
	_vertex_1.position = (rde_vec_3F){ _vertex_b.x, _vertex_b.y / _aspect_ratio, 0.f };
	_vertex_1.color = _c;
	_vertex_1.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_1.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1;

	rde_vertex_2d _vertex_2;
	_vertex_2.position = (rde_vec_3F){ _vertex_c.x, _vertex_c.y / _aspect_ratio, 0.f };
	_vertex_2.color = _c;
	_vertex_2.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_2.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_2;
}

void rde_rendering_2d_draw_rectangle(rde_vec_2F _bottom_left, rde_vec_2F _top_right, rde_color _color, rde_shader* _shader) {
	const size_t _triangle_vertex_count = 6;
	
	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.color_shader_2d : _shader;
	rde_inner_rendering_try_create_batch_2d(_drawing_shader, NULL);
	rde_inner_rendering_try_flush_batch_2d(_drawing_shader, NULL, _triangle_vertex_count);

	int _c = RDE_COLOR_TO_HEX_COLOR(_color);
	rde_vec_2I _window_size = rde_window_get_window_size(current_drawing_window);
	float _aspect_ratio = (float)_window_size.x / (float)_window_size.y;

	// * 2
	// |\
	// | \
	// *--*
	// 0   1
	rde_vertex_2d _vertex_0_0;
	_vertex_0_0.position = (rde_vec_3F){ _bottom_left.x, _bottom_left.y / _aspect_ratio, 0.f };
	_vertex_0_0.color = _c;
	_vertex_0_0.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_0_0.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_0;

	rde_vertex_2d _vertex_0_1;
	_vertex_0_1.position = (rde_vec_3F){ _top_right.x, _bottom_left.y / _aspect_ratio, 0.f };
	_vertex_0_1.color = _c;
	_vertex_0_1.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_0_1.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_1;

	rde_vertex_2d _vertex_0_2;
	_vertex_0_2.position = (rde_vec_3F){ _bottom_left.x, _top_right.y / _aspect_ratio, 0.f };
	_vertex_0_2.color = _c;
	_vertex_0_2.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_0_2.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_2;

	// 2   1
	// *--*
	//  \ |
	//   \|
	//    *
	//    0

	rde_vertex_2d _vertex_1_0;
	_vertex_1_0.position = (rde_vec_3F){ _top_right.x, _bottom_left.y / _aspect_ratio, 0.f };
	_vertex_1_0.color = _c;
	_vertex_1_0.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_1_0.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_0;

	rde_vertex_2d _vertex_1_1;
	_vertex_1_1.position = (rde_vec_3F){ _top_right.x, _top_right.y / _aspect_ratio, 0.f };
	_vertex_1_1.color = _c;
	_vertex_1_1.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_1_1.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_1;

	rde_vertex_2d _vertex_1_2;
	_vertex_1_2.position = (rde_vec_3F){ _bottom_left.x, _top_right.y / _aspect_ratio, 0.f };
	_vertex_1_2.color = _c;
	_vertex_1_2.texture_coordinates = (rde_vec_2F){ 0.f, 0.f };
	rde_math_convert_world_position_to_screen_coordinates_3d(current_drawing_window, &_vertex_1_2.position);
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_2;
}

void rde_rendering_2d_draw_circle(rde_vec_2F _position, float _radius, rde_color _color, rde_shader* _shader) {
	UNUSED(_position);
	UNUSED(_radius);
	UNUSED(_color);
	UNUSED(_shader);
	UNIMPLEMENTED()
}

void rde_rendering_2d_draw_polygon(const rde_transform* _transform, const rde_polygon* _polygon, rde_color _color, const rde_shader* _shader) {
	UNUSED(_transform)
	UNUSED(_polygon)
	UNUSED(_color)
	UNUSED(_color)
	UNUSED(_shader)
	UNIMPLEMENTED()
}

void rde_rendering_2d_draw_texture(const rde_transform* _transform, const rde_texture* _texture, rde_color _tint_color, rde_shader* _shader) {
	const size_t _triangle_vertex_count = 6;

	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_inner_rendering_transform_to_glm_mat4_2d(_transform, _transformation_matrix);

	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.texture_shader_2d : _shader;
	rde_inner_rendering_try_create_batch_2d(_drawing_shader, _texture);
	rde_inner_rendering_try_flush_batch_2d(_drawing_shader, _texture, _triangle_vertex_count);

	rde_vec_2F _texture_origin_norm = (rde_vec_2F){ 0.f, 0.f };
	rde_vec_2F _texture_tile_size_norm = (rde_vec_2F){ 1.f, 1.f };
	rde_vec_2F _texture_tile_size = (rde_vec_2F){ (float)_texture->size.x, (float)_texture->size.y };

	if(_texture->atlas_texture != NULL) {
		_texture_origin_norm.x = _texture->position.x / (float)_texture->atlas_texture->size.x;
		_texture_origin_norm.y = _texture->position.y / (float)_texture->atlas_texture->size.y;
	
		_texture_tile_size_norm.x = _texture_tile_size.x / (float)_texture->atlas_texture->size.x;
		_texture_tile_size_norm.y = _texture_tile_size.y / (float)_texture->atlas_texture->size.y;
	}

	rde_vec_2F _texture_tile_size_on_screen = _texture_tile_size;
	rde_math_convert_world_size_to_screen_size(current_drawing_window, &_texture_tile_size_on_screen);

	vec4 _bottom_left_texture_position = GLM_VEC4_ONE_INIT;
	vec4 _bottom_right_texture_position = GLM_VEC4_ONE_INIT;
	vec4 _top_right_texture_position = GLM_VEC4_ONE_INIT;
	vec4 _top_left_texture_position = GLM_VEC4_ONE_INIT;

	glm_mat4_mulv(_transformation_matrix, (vec4) { -_texture_tile_size_on_screen.x, -_texture_tile_size_on_screen.y, 0.0f, 1.0f }, _bottom_left_texture_position);
	glm_mat4_mulv(_transformation_matrix, (vec4) { _texture_tile_size_on_screen.x, -_texture_tile_size_on_screen.y, 0.0f, 1.0f }, _bottom_right_texture_position);
	glm_mat4_mulv(_transformation_matrix, (vec4) { _texture_tile_size_on_screen.x,  _texture_tile_size_on_screen.y, 0.0f, 1.0f }, _top_right_texture_position);
	glm_mat4_mulv(_transformation_matrix, (vec4) { -_texture_tile_size_on_screen.x,  _texture_tile_size_on_screen.y, 0.0f, 1.0f }, _top_left_texture_position);

	// The (1.f - _texture_origin_norm.y) is needed as we need to invert the coordinates on the Y axis, the exported atlas (by RDE tool) assumes (0, 0)
	// on top-left, but OpenGL coords for UV origin is bottom-left.
	vec2 _top_left_texture_uv_coord   	 = (vec2){ _texture_origin_norm.x                            , 1.f - _texture_origin_norm.y };
	vec2 _top_right_texture_uv_coord  	 = (vec2){ _texture_origin_norm.x + _texture_tile_size_norm.x, 1.f - _texture_origin_norm.y };
	vec2 _bottom_right_texture_uv_coord  = (vec2){ _texture_origin_norm.x + _texture_tile_size_norm.x, 1.f - (_texture_origin_norm.y + _texture_tile_size_norm.y) };
	vec2 _bottom_left_texture_uv_coord   = (vec2){ _texture_origin_norm.x                            , 1.f - (_texture_origin_norm.y + _texture_tile_size_norm.y) };

	int _color = RDE_COLOR_TO_HEX_COLOR(_tint_color);

	// * 2
	// |\
	// | \
	// *--*
	// 0   1
	rde_vertex_2d _vertex_0_0;
	_vertex_0_0.position = (rde_vec_3F){ _bottom_left_texture_position[0], _bottom_left_texture_position[1], 0.f };
	_vertex_0_0.color = _color;
	_vertex_0_0.texture_coordinates = (rde_vec_2F){ _bottom_left_texture_uv_coord[0], _bottom_left_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_0;

	rde_vertex_2d _vertex_0_1;
	_vertex_0_1.position = (rde_vec_3F){ _bottom_right_texture_position[0], _bottom_right_texture_position[1], 0.f };
	_vertex_0_1.color = _color;
	_vertex_0_1.texture_coordinates = (rde_vec_2F){ _bottom_right_texture_uv_coord[0], _bottom_right_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_1;

	rde_vertex_2d _vertex_0_2;
	_vertex_0_2.position = (rde_vec_3F){ _top_left_texture_position[0], _top_left_texture_position[1], 0.f };
	_vertex_0_2.color = _color;
	_vertex_0_2.texture_coordinates = (rde_vec_2F){ _top_left_texture_uv_coord[0], _top_left_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_2;

	// 2   1
	// *--*
	//  \ |
	//   \|
	//    *
	//    0

	rde_vertex_2d _vertex_1_0;
	_vertex_1_0.position = (rde_vec_3F){ _bottom_right_texture_position[0], _bottom_right_texture_position[1], 0.f };
	_vertex_1_0.color = _color;
	_vertex_1_0.texture_coordinates = (rde_vec_2F){ _bottom_right_texture_uv_coord[0], _bottom_right_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_0;

	rde_vertex_2d _vertex_1_1;
	_vertex_1_1.position = (rde_vec_3F){ _top_right_texture_position[0], _top_right_texture_position[1], 0.f };
	_vertex_1_1.color = _color;
	_vertex_1_1.texture_coordinates = (rde_vec_2F){ _top_right_texture_uv_coord[0], _top_right_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_1;

	rde_vertex_2d _vertex_1_2;
	_vertex_1_2.position = (rde_vec_3F){ _top_left_texture_position[0], _top_left_texture_position[1], 0.f };
	_vertex_1_2.color = _color;
	_vertex_1_2.texture_coordinates = (rde_vec_2F){ _top_left_texture_uv_coord[0], _top_left_texture_uv_coord[1] };
	current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_2;
}

void rde_rendering_2d_draw_memory_texture(const rde_transform* _transform, rde_texture* _texture, rde_color _tint_color, rde_shader* _shader) {
	rde_log_level(RDE_LOG_LEVEL_WARNING, "THIS FUNCTION IS CORRPUTING 3D MESHES TEXTURE. To reproduce just draw a memory texture and then a 3d mesh");
	rde_rendering_memory_texture_gpu_upload(_texture);
	rde_rendering_2d_draw_texture(_transform, _texture, _tint_color, _shader);
}

void rde_rendering_2d_draw_text(const rde_transform* _transform, const rde_font* _font, const char* _text, rde_color _tint_color, rde_shader* _shader) {
	int _text_size = strlen(_text);

	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.text_shader_2d : _shader;

	int _next_pos_x = 0;
	for (int _i = 0; _i < _text_size; _i++) {
		rde_transform _t = *_transform;
		int _key = (int)_text[_i];
		rde_font_char_info _char_info = _font->chars[_key];
		_t.position.x += _next_pos_x + _char_info.bearing.x * _t.scale.x;
		_t.position.y -= (_char_info.size.y * 0.5f - _char_info.bearing.y) * _t.scale.y;

		const size_t _triangle_vertex_count = 6;

		mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;
		rde_inner_rendering_transform_to_glm_mat4_2d(&_t, _transformation_matrix);

		rde_inner_rendering_try_create_batch_2d(_drawing_shader, _char_info.texture.atlas_texture);
		rde_inner_rendering_try_flush_batch_2d(_drawing_shader, _char_info.texture.atlas_texture, _triangle_vertex_count);

		rde_vec_2F _texture_origin_norm = (rde_vec_2F){ 0.f, 0.f };
		rde_vec_2F _texture_tile_size_norm = (rde_vec_2F){ 1.f, 1.f };
		rde_vec_2F _texture_tile_size = (rde_vec_2F){ (float)_char_info.size.x, (float)_char_info.size.y };

		if(_char_info.texture.atlas_texture != NULL) {
			_texture_origin_norm.x = _char_info.offset.x / (float)_char_info.texture.atlas_texture->size.x;
			_texture_origin_norm.y = _char_info.offset.y / (float)_char_info.texture.atlas_texture->size.y;
	
			_texture_tile_size_norm.x = _char_info.size.x / (float)_char_info.texture.atlas_texture->size.x;
			_texture_tile_size_norm.y = _char_info.size.y / (float)_char_info.texture.atlas_texture->size.y;
		}

		rde_vec_2F _texture_tile_size_on_screen = _texture_tile_size;
		rde_math_convert_world_size_to_screen_size(current_drawing_window, &_texture_tile_size_on_screen);

		rde_vec_2F _size_in_screen = (rde_vec_2F) { (float)(_char_info.advance.x >> 6) - _char_info.bearing.x, 0.f};
		rde_math_convert_world_size_to_screen_size(current_drawing_window, &_size_in_screen);

		vec4 _bottom_left_texture_position = GLM_VEC4_ONE_INIT;
		vec4 _bottom_right_texture_position = GLM_VEC4_ONE_INIT;
		vec4 _top_right_texture_position = GLM_VEC4_ONE_INIT;
		vec4 _top_left_texture_position = GLM_VEC4_ONE_INIT;

		glm_mat4_mulv(_transformation_matrix, (vec4) { -_texture_tile_size_on_screen.x + _size_in_screen.x, -_texture_tile_size_on_screen.y, 0.0f, 1.0f }, _bottom_left_texture_position);
		glm_mat4_mulv(_transformation_matrix, (vec4) { _texture_tile_size_on_screen.x  + _size_in_screen.x, -_texture_tile_size_on_screen.y, 0.0f, 1.0f }, _bottom_right_texture_position);
		glm_mat4_mulv(_transformation_matrix, (vec4) { _texture_tile_size_on_screen.x  + _size_in_screen.x,  _texture_tile_size_on_screen.y, 0.0f, 1.0f }, _top_right_texture_position);
		glm_mat4_mulv(_transformation_matrix, (vec4) { -_texture_tile_size_on_screen.x + _size_in_screen.x,  _texture_tile_size_on_screen.y, 0.0f, 1.0f }, _top_left_texture_position);

		// The (1.f - _texture_origin_norm.y) is needed as we need to invert the coordinates on the Y axis, the exported atlas (by RDE tool) assumes (0, 0)
		// on top-left, but OpenGL coords for UV origin is bottom-left.
		vec2 _top_left_texture_uv_coord   	 = (vec2){ _texture_origin_norm.x                            , 1.f - _texture_origin_norm.y };
		vec2 _top_right_texture_uv_coord  	 = (vec2){ _texture_origin_norm.x + _texture_tile_size_norm.x, 1.f - _texture_origin_norm.y };
		vec2 _bottom_right_texture_uv_coord  = (vec2){ _texture_origin_norm.x + _texture_tile_size_norm.x, 1.f - (_texture_origin_norm.y + _texture_tile_size_norm.y) };
		vec2 _bottom_left_texture_uv_coord   = (vec2){ _texture_origin_norm.x                            , 1.f - (_texture_origin_norm.y + _texture_tile_size_norm.y) };

		int _color = RDE_COLOR_TO_HEX_COLOR(_tint_color);

		// * 2
		// |\
		// | \
		// *--*
		// 0   1
		rde_vertex_2d _vertex_0_0;
		_vertex_0_0.position = (rde_vec_3F){ _bottom_left_texture_position[0], _bottom_left_texture_position[1], 0.f };
		_vertex_0_0.color = _color;
		_vertex_0_0.texture_coordinates = (rde_vec_2F){ _bottom_left_texture_uv_coord[0], _bottom_left_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_0;

		rde_vertex_2d _vertex_0_1;
		_vertex_0_1.position = (rde_vec_3F){ _bottom_right_texture_position[0], _bottom_right_texture_position[1], 0.f };
		_vertex_0_1.color = _color;
		_vertex_0_1.texture_coordinates = (rde_vec_2F){ _bottom_right_texture_uv_coord[0], _bottom_right_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_1;

		rde_vertex_2d _vertex_0_2;
		_vertex_0_2.position = (rde_vec_3F){ _top_left_texture_position[0], _top_left_texture_position[1], 0.f };
		_vertex_0_2.color = _color;
		_vertex_0_2.texture_coordinates = (rde_vec_2F){ _top_left_texture_uv_coord[0], _top_left_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_0_2;

		// 2   1
		// *--*
		//  \ |
		//   \|
		//    *
		//    0

		rde_vertex_2d _vertex_1_0;
		_vertex_1_0.position = (rde_vec_3F){ _bottom_right_texture_position[0], _bottom_right_texture_position[1], 0.f };
		_vertex_1_0.color = _color;
		_vertex_1_0.texture_coordinates = (rde_vec_2F){ _bottom_right_texture_uv_coord[0], _bottom_right_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_0;

		rde_vertex_2d _vertex_1_1;
		_vertex_1_1.position = (rde_vec_3F){ _top_right_texture_position[0], _top_right_texture_position[1], 0.f };
		_vertex_1_1.color = _color;
		_vertex_1_1.texture_coordinates = (rde_vec_2F){ _top_right_texture_uv_coord[0], _top_right_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_1;

		rde_vertex_2d _vertex_1_2;
		_vertex_1_2.position = (rde_vec_3F){ _top_left_texture_position[0], _top_left_texture_position[1], 0.f };
		_vertex_1_2.color = _color;
		_vertex_1_2.texture_coordinates = (rde_vec_2F){ _top_left_texture_uv_coord[0], _top_left_texture_uv_coord[1] };
		current_batch_2d.vertices[current_batch_2d.amount_of_vertices++] = _vertex_1_2;

		_next_pos_x += (_char_info.advance.x >> 6) * _t.scale.x; // /64.f
	}
}

void rde_rendering_2d_end_drawing() {
	rde_inner_rendering_flush_batch_2d();
	rde_inner_rendering_reset_batch_2d();
	current_drawing_camera = NULL;
	current_drawing_window = NULL;
	statistics.number_of_drawcalls = 0;
}


rde_mesh* rde_struct_memory_mesh_create(rde_mesh_gen_data* _data) {
	rde_mesh* _mesh = (rde_mesh*)malloc(sizeof(rde_mesh));
	rde_critical_error(_mesh == NULL, "Could not create a memory mesh");
	_mesh->vao = 0;

	memset(_mesh->name, 0, RDE_MESH_NAME_MAX);
	
	if(_data->name != NULL) {
		rde_strcat(_mesh->name, RDE_MESH_NAME_MAX, _data->name);
	} else {
		rde_strcat(_mesh->name, RDE_MESH_NAME_MAX, "mesh");
	}
	
	GLuint _vao = 0;
	RDE_CHECK_GL(glGenVertexArrays, 1, &_vao);
	_mesh->vao = _vao;
	_mesh->vertex_count = _data->vertex_count;
	_mesh->vertex_positions = _data->positions;
	_mesh->vertex_normals = _data->normals;
	_mesh->vertex_texcoords = _data->texcoords;
	
	_mesh->transformation_matrices = NULL;
	_mesh->transformation_matrices = (mat4*)malloc(sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW);
	for(size_t _i = 0; _i < RDE_MAX_MODELS_PER_DRAW; _i++) {
		glm_mat4_zero(_mesh->transformation_matrices[_i]);
	}

	RDE_CHECK_GL(glBindVertexArray, _mesh->vao);
	
	size_t _positions_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_POSITION;
	RDE_CHECK_GL(glGenBuffers, 1, &_mesh->vbo[0]);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[0]);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _positions_size, _mesh->vertex_positions, GL_STATIC_DRAW);
	RDE_CHECK_GL(glEnableVertexAttribArray, 0);
	RDE_CHECK_GL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);

	if(_data->normals != NULL && _data->normals_size > 0) {
		size_t _normals_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_NORMAL;
		RDE_CHECK_GL(glGenBuffers, 1, &_mesh->vbo[1]);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[1]);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _normals_size, _mesh->vertex_normals, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 1);
		RDE_CHECK_GL(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	}

	if(_data->texcoords != NULL && _data->texcoords_size > 0) {
		size_t _texcoords_size = sizeof(float) * _mesh->vertex_count * RDE_NUMBER_OF_ELEMENTS_PER_VERTEX_TEXTURE_COORD;
		RDE_CHECK_GL(glGenBuffers, 1, &_mesh->vbo[2]);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[2]);
		RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, _texcoords_size, _mesh->vertex_texcoords, GL_STATIC_DRAW);
		RDE_CHECK_GL(glEnableVertexAttribArray, 2);
		RDE_CHECK_GL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	}

	RDE_CHECK_GL(glGenBuffers, 1, &_mesh->vbo[3]);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, _mesh->vbo[3]);
	RDE_CHECK_GL(glBufferData, GL_ARRAY_BUFFER, sizeof(mat4) * RDE_MAX_MODELS_PER_DRAW, NULL, GL_DYNAMIC_DRAW);
	
	RDE_CHECK_GL(glEnableVertexAttribArray, 3);
	RDE_CHECK_GL(glVertexAttribPointer, 3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);

	RDE_CHECK_GL(glEnableVertexAttribArray, 4);
	RDE_CHECK_GL(glVertexAttribPointer, 4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 4));

	RDE_CHECK_GL(glEnableVertexAttribArray, 5);
	RDE_CHECK_GL(glVertexAttribPointer, 5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 8));

	RDE_CHECK_GL(glEnableVertexAttribArray, 6);
	RDE_CHECK_GL(glVertexAttribPointer, 6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(float) * 12));

	RDE_CHECK_GL(glVertexAttribDivisor, 3, 1);
	RDE_CHECK_GL(glVertexAttribDivisor, 4, 1);
	RDE_CHECK_GL(glVertexAttribDivisor, 5, 1);
	RDE_CHECK_GL(glVertexAttribDivisor, 6, 1);
	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);

	RDE_CHECK_GL(glBindVertexArray, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);

	_mesh->material = rde_struct_create_material();
	_mesh->material.map_ka = _data->material.map_ka;
	_mesh->material.map_kd = _data->material.map_kd;
	_mesh->material.map_ks = _data->material.map_ks;
	_mesh->material.map_bump = _data->material.map_bump;
	_mesh->material.render_texture = _data->material.render_texture;
	_mesh->material.material_light_data = _data->material.material_light_data;


	rde_log_level(RDE_LOG_LEVEL_INFO, "Created mesh with %zu vertices. VAO: %u\n", _mesh->vertex_count, _vao);

	return _mesh;
}

rde_mesh* rde_rendering_mesh_create_cube(float _size, rde_material* _material) {
	CUBE_MESH_DATA(_size)

	rde_mesh_gen_data _data = {
		.name = "Cube",
		.vertex_count = _vertex_count,
		.positions = _positions,
		.positions_size = _vertex_count * 3,
		.texcoords = NULL,
		.texcoords_size = 0,
		.normals = _normals,
		.normals_size = _vertex_count * 3,
		.material = _material == NULL ? rde_struct_create_material() : *_material
	};

	return rde_struct_memory_mesh_create(&_data);
}

rde_mesh* rde_rendering_mesh_create_triangular_pyramid(rde_vec_2F _size, rde_material* _material) {
	TRIANGULAR_PYRAMID_MESH_DATA(_size.x, _size.y)

	rde_mesh_gen_data _data = {
		.name = "Triangular Pyramid",
		.vertex_count = _vertex_count,
		.positions = _positions,
		.positions_size = _vertex_count * 3,
		.texcoords = NULL,
		.texcoords_size = 0,
		.normals = _normals,
		.normals_size = _vertex_count * 3,
		.material = _material == NULL ? rde_struct_create_material() : *_material
	};

	return rde_struct_memory_mesh_create(&_data);
}

rde_mesh* rde_rendering_mesh_create_pyramid(rde_vec_3F _size, rde_material* _material) {
	PYRAMID_MESH_DATA(_size.x, _size.y, _size.z)

	rde_mesh_gen_data _data = {
		.name = "Pyramid",
		.vertex_count = _vertex_count,
		.positions = _positions,
		.positions_size = _vertex_count * 3,
		.texcoords = NULL,
		.texcoords_size = 0,
		.normals = _normals,
		.normals_size = _vertex_count * 3,
		.material = _material == NULL ? rde_struct_create_material() : *_material
	};

	return rde_struct_memory_mesh_create(&_data);
}

rde_mesh* rde_rendering_mesh_create_prism(rde_vec_3F _size, rde_material* _material) {
	PRISM_MESH_DATA(_size.x, _size.y, _size.z)

	rde_mesh_gen_data _data = {
		.name = "Cube",
		.vertex_count = _vertex_count,
		.positions = _positions,
		.positions_size = _vertex_count * 3,
		.texcoords = NULL,
		.texcoords_size = 0,
		.normals = _normals,
		.normals_size = _vertex_count * 3,
		.material = _material == NULL ? rde_struct_create_material() : *_material
	};

	return rde_struct_memory_mesh_create(&_data);
}

rde_mesh* rde_rendering_mesh_create_sphere(float _radius, rde_material* _material) {
	SPHERE_MESH_DATA(_radius)

	rde_mesh_gen_data _data = {
		.name = "Sphere",
		.vertex_count = _vertex_count,
		.positions = _positions,
		.positions_size = _vertex_count * 3,
		.texcoords = NULL,
		.texcoords_size = 0,
		.normals = _normals,
		.normals_size = _vertex_count * 3,
		.material = _material == NULL ? rde_struct_create_material() : *_material
	};

	return rde_struct_memory_mesh_create(&_data);
}

rde_model* rde_rendering_model_load(const char* _model_path) {
	const char* _extension = rde_util_file_get_name_extension(_model_path);

	for (size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_models; _i++) {
		if (strlen(ENGINE.models[_i].file_path) != 0 && strcmp(ENGINE.models[_i].file_path, _model_path) == 0) {
			return &ENGINE.models[_i];
		}
	}

	if(strcmp(_extension, RDE_FBX_EXTENSION) == 0) {
		rde_critical_error(true, RDE_ERROR_FEATURE_NOT_SUPPORTED_YET, "rde_rendering_model_load for FBX");
	} else if(strcmp(_extension, RDE_OBJ_EXTENSION) == 0) {
#if defined(RDE_OBJ_MODULE)
		return rde_inner_obj_load_model(_model_path);
#else
		rde_critical_error(true, RDE_ERROR_RENDERING_MODEL_MODULE_FORMAT_NOT_COMPILED, _model_path, _extension);
#endif
	} else if(strcmp(_extension, RDE_GLTF_EXTENSION) == 0) {
		rde_critical_error(true, RDE_ERROR_FEATURE_NOT_SUPPORTED_YET, "rde_rendering_model_load for GLTF");
	} else {
		rde_critical_error(true, RDE_ERROR_RENDERING_MODEL_UNSUPPORTED_FORMAT, _model_path, _extension);
	}

	return NULL;
}

void rde_rendering_mesh_destroy(rde_mesh* _mesh, bool _delete_allocated_buffers) {
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");

	RDE_CHECK_GL(glBindBuffer, GL_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
	RDE_CHECK_GL(glBindVertexArray, 0);

	// positions
	if(_mesh->vbo[0] != RDE_UINT_MAX) {
		RDE_CHECK_GL(glDeleteBuffers, 1, &_mesh->vbo[0]);
		_mesh->vbo[0] = RDE_UINT_MAX;
	}
	
	// normals
	if(_mesh->vbo[1] != RDE_UINT_MAX) {
		RDE_CHECK_GL(glDeleteBuffers, 1, &_mesh->vbo[1]);
		_mesh->vbo[1] = RDE_UINT_MAX;
	}

	// texcoords
	if(_mesh->vbo[2] != RDE_UINT_MAX) {
		RDE_CHECK_GL(glDeleteBuffers, 1, &_mesh->vbo[2]);
		_mesh->vbo[2] = RDE_UINT_MAX;
	}
	
	// transforms
	if(_mesh->vbo[3] != RDE_UINT_MAX) {
		RDE_CHECK_GL(glDeleteBuffers, 1, &_mesh->vbo[3]);
		_mesh->vbo[3] = RDE_UINT_MAX;
	}

	RDE_CHECK_GL(glDeleteVertexArrays, 1, &_mesh->vao);
	_mesh->vao = RDE_UINT_MAX;
	
	if(_delete_allocated_buffers) {
		free(_mesh->vertex_positions);
		_mesh->vertex_positions = NULL;

		free(_mesh->vertex_normals);
		_mesh->vertex_normals = NULL;

		free(_mesh->vertex_texcoords);
		_mesh->vertex_texcoords = NULL;
	}

	if(_mesh->material.map_ka != NULL && _mesh->material.map_ka != DEFAULT_TEXTURE) {
		rde_rendering_texture_unload(_mesh->material.map_ka);
		_mesh->material.map_ka = NULL;
	}
	if(_mesh->material.map_kd != NULL && _mesh->material.map_kd != DEFAULT_TEXTURE) {
		rde_rendering_texture_unload(_mesh->material.map_kd);
		_mesh->material.map_kd = NULL;
	}
	if(_mesh->material.map_ks != NULL && _mesh->material.map_ks != DEFAULT_TEXTURE) {
		rde_rendering_texture_unload(_mesh->material.map_ks);
		_mesh->material.map_ks = NULL;
	}
	if(_mesh->material.map_bump != NULL && _mesh->material.map_bump != DEFAULT_TEXTURE) {
		rde_rendering_texture_unload(_mesh->material.map_bump);
		_mesh->material.map_bump = NULL;
	}

	free(_mesh->transformation_matrices);
	_mesh->transformation_matrices = NULL;

	_mesh->material = rde_struct_create_material();
}


void rde_rendering_3d_begin_drawing(rde_camera* _camera, rde_window* _window, bool _draw_wireframe_over_mesh) {
	rde_critical_error(_camera == NULL || _window == NULL, RDE_ERROR_BEGIN_RENDER);

	RDE_CHECK_GL(glEnable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glDepthFunc, GL_LESS);

	RDE_CHECK_GL(glEnable, GL_CULL_FACE);
	RDE_CHECK_GL(glCullFace, GL_BACK);
	RDE_CHECK_GL(glFrontFace, GL_CCW);

	current_drawing_camera = _camera;
	current_drawing_window = _window;

	rde_vec_2F _aspect_ratios = rde_inner_rendering_get_aspect_ratio();
	float _aspect_ratio = rde_window_orientation_is_horizontal(_window) ? _aspect_ratios.y : _aspect_ratios.x;
	glm_perspective(_camera->fov, _aspect_ratio, _camera->near_far.x, _camera->near_far.y, projection_matrix);

	current_batch_3d.draw_mesh_wireframe = _draw_wireframe_over_mesh;
}

void rde_rendering_3d_draw_line(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, unsigned short _thickness, rde_shader* _shader) {
	const size_t _line_vertex_count = 2;

	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.line_shader : _shader;
	rde_inner_rendering_try_create_line_batch(_drawing_shader, _thickness);
	rde_inner_rendering_try_flush_line_batch(_drawing_shader, _thickness, _line_vertex_count);
	int _c = RDE_COLOR_TO_HEX_COLOR(_color);

	rde_line_vertex _vertex_0;
	_vertex_0.position = (rde_vec_3F){ _init.x, _init.y, _init.z };
	_vertex_0.color = _c;
	current_batch_3d.line_batch.vertices[current_batch_3d.line_batch.amount_of_vertices++] = _vertex_0;

	rde_line_vertex _vertex_1;
	_vertex_1.position = (rde_vec_3F){ _end.x, _end.y, _end.z };
	_vertex_1.color = _c;
	current_batch_3d.line_batch.vertices[current_batch_3d.line_batch.amount_of_vertices++] = _vertex_1;
}

void rde_rendering_3d_draw_mesh(const rde_transform* _transform, rde_mesh* _mesh, rde_shader* _shader) {
	const size_t _floats_per_matrix = 1;
	
	rde_shader* _drawing_shader = _shader == NULL ? ENGINE.mesh_shader : _shader;
	rde_inner_rendering_try_create_batch_3d(_drawing_shader, _mesh);
	rde_inner_rendering_try_flush_batch_3d(_drawing_shader, _mesh, _floats_per_matrix);
	
	glm_mat4_copy(ENGINE.world_transforms[_transform->index], _mesh->transformation_matrices[current_batch_3d.amount_of_models_per_draw]);
	current_batch_3d.amount_of_models_per_draw++;
}

void rde_rendering_3d_draw_model(const rde_transform* _transform, rde_model* _model, rde_shader* _shader) {
	for(uint _i = 0; _i < _model->mesh_array_size; _i++) {
		rde_mesh* _mesh = &_model->mesh_array[_i];
		rde_rendering_3d_draw_mesh(_transform, _mesh, _shader);
	}
}

void rde_rendering_3d_end_drawing() {

#if !IS_ANDROID()
	if(current_batch_3d.draw_mesh_wireframe) {
		RDE_CHECK_GL(glPolygonMode, GL_FRONT_AND_BACK, GL_LINE);
		rde_inner_rendering_flush_batch_3d();
		RDE_CHECK_GL(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL);
	}
#endif

	rde_inner_rendering_flush_batch_3d();

	rde_inner_rendering_reset_batch_3d();
	rde_inner_rendering_flush_line_batch();
	rde_inner_rendering_reset_line_batch();
	RDE_CHECK_GL(glDisable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glDisable, GL_CULL_FACE);
	current_drawing_camera = NULL;
}

void rde_rendering_lighting_set_directional_light_direction(rde_vec_3F _direction) {
	ENGINE.illumination.directional_light.direction = _direction;
}

void rde_rendering_lighting_set_directional_light_ambient_color(rde_color _ambient_color) {
	ENGINE.illumination.directional_light.ambient_color = (rde_vec_3F) { _ambient_color.r / 255.f,
															_ambient_color.g / 255.f,
															_ambient_color.a / 255.f
	};
}

void rde_rendering_lighting_set_directional_light_ambient_color_f(rde_vec_3F _ambient_color) {
	ENGINE.illumination.directional_light.ambient_color = _ambient_color;
}

void rde_rendering_lighting_set_directional_light_diffuse_color(rde_color _diffuse_color) {
	ENGINE.illumination.directional_light.diffuse_color = (rde_vec_3F) { _diffuse_color.r / 255.f,
															_diffuse_color.g / 255.f,
															_diffuse_color.a / 255.f
	};
}

void rde_rendering_lighting_set_directional_light_diffuse_color_f(rde_vec_3F _diffuse_color) {
	ENGINE.illumination.directional_light.diffuse_color = _diffuse_color;
}


void rde_rendering_lighting_set_directional_light_specular_color(rde_color _specular_color) {
	ENGINE.illumination.directional_light.specular_color = (rde_vec_3F) { _specular_color.r / 255.f,
															 _specular_color.g / 255.f,
															 _specular_color.a / 255.f
	};
}

void rde_rendering_lighting_set_directional_light_specular_color_f(rde_vec_3F _specular_color) {
	ENGINE.illumination.directional_light.specular_color = _specular_color;
}

rde_directional_light rde_rendering_lighting_get_directional_light() {
	return ENGINE.illumination.directional_light;
}

void rde_rendering_light_add_add_point_light(rde_point_light* _point_light) {
	bool _found = false;
	for(size_t _i = 0; _i < ENGINE.init_info.illumination_config.max_amount_of_point_lights; _i++) {
		if(ENGINE.illumination.point_lights[_i] == NULL) {
			ENGINE.illumination.point_lights[_i] = _point_light;
			_found = true;
			break;
		}
	}

	rde_critical_error(!_found, "Max point light amount '%d' reached", ENGINE.init_info.illumination_config.max_amount_of_point_lights);
}

void rde_rendering_light_add_add_spot_light(rde_spot_light* _spot_light) {
	bool _found = false;
	for(size_t _i = 0; _i < ENGINE.init_info.illumination_config.max_amount_of_spot_lights; _i++) {
		if(ENGINE.illumination.spot_lights[_i] == NULL) {
			ENGINE.illumination.spot_lights[_i] = _spot_light;
			_found = true;
			break;
		}
	}

	rde_critical_error(!_found, "Max spot light amount '%d' reached", ENGINE.init_info.illumination_config.max_amount_of_spot_lights);
}

rde_mesh_data rde_rendering_mesh_get_data(rde_mesh* _mesh) {
	rde_critical_error(_mesh == NULL, RDE_ERROR_NO_NULL_ALLOWED, "mesh");
	return (rde_mesh_data) {
		.amount_of_vertices = _mesh->vertex_count,
		.vertex_buffer_objects_ids = { _mesh->vbo[0],_mesh->vbo[1], _mesh->vbo[2], _mesh->vbo[3] },
	};
}

size_t rde_rendering_model_get_vertices_count(rde_model* _model) {
	size_t _total_vertices = 0;
	
	for(size_t _i = 0; _i < _model->mesh_array_size; _i++) {
		_total_vertices += rde_rendering_mesh_get_data(&_model->mesh_array[_i]).amount_of_vertices;
	}

	return _total_vertices;
}

void rde_rendering_model_set_light_data(rde_model* _model, rde_material_light_data _light_data) {
	rde_critical_error(_model == NULL, RDE_ERROR_NO_NULL_ALLOWED, "model");
	for(size_t _i = 0; _i < _model->mesh_array_size; _i++) {
		_model->mesh_array[_i].material.material_light_data = _light_data;
	}
}
rde_material_light_data rde_rendering_model_get_light_data(rde_model* _model) {
	rde_critical_error(_model == NULL, RDE_ERROR_NO_NULL_ALLOWED, "model");
	return _model->mesh_array[0].material.material_light_data;
}

rde_model_data rde_rendering_model_get_data(rde_model* _model) {
	rde_model_data _m;
	_m.amount_of_meshes = _model->mesh_array_size;
	_m.meshes = (rde_mesh**)malloc(sizeof(rde_mesh*) * _m.amount_of_meshes);

	for(size_t _i = 0; _i < _m.amount_of_meshes; _i++) {
		_m.meshes[_i] = &_model->mesh_array[_i];
	}

	return _m;
}

void rde_rendering_model_unload(rde_model* _model) {
	rde_critical_error(_model == NULL, RDE_ERROR_NO_NULL_ALLOWED, "obj model");

	for(size_t _c = 0; _c < stbds_arrlenu(_model->mesh_array); _c++) {
		rde_mesh* _mesh = &_model->mesh_array[_c];
		rde_rendering_mesh_destroy(_mesh, true);
	}

	stbds_arrfree(_model->mesh_array);
	_model->mesh_array = NULL;
	_model->mesh_array_size = 0;
	memset(_model->file_path, 0, RDE_MAX_PATH);
}

void rde_rendering_3d_draw_skybox(rde_camera* _camera) {
	if(ENGINE.skybox.opengl_texture_id < 0 || _camera == NULL) {
		return;
	}
	RDE_CHECK_GL(glEnable, GL_DEPTH_TEST);
	RDE_CHECK_GL(glDepthFunc, GL_LEQUAL);
	RDE_CHECK_GL(glUseProgram, ENGINE.skybox_shader->compiled_program_id);
	
	mat4 _view_projection_matrix = GLM_MAT4_IDENTITY_INIT;

	mat4 _view_matrix = GLM_MAT4_IDENTITY_INIT;
	rde_vec_3F _cam_pos = _camera->transform->position;
	rde_vec_3F _cam_direction = _camera->direction;
	rde_vec_3F _cam_up = _camera->up;
	glm_lookat( (vec3) { _cam_pos.x, _cam_pos.y, _cam_pos.z },
				(vec3) { _cam_pos.x + _cam_direction.x, _cam_pos.y + _cam_direction.y, _cam_pos.z + _cam_direction.z },
				(vec3) { _cam_up.x, _cam_up.y, _cam_up.z },
				_view_matrix
	);
	_view_matrix[3][0] = 0.0f;
	_view_matrix[3][1] = 0.0f;
	_view_matrix[3][2] = 0.0f;
	_view_matrix[3][3] = 0.0f;

	glm_mat4_mul(projection_matrix, _view_matrix, _view_projection_matrix);

	RDE_CHECK_GL(glUniformMatrix4fv, glGetUniformLocation(ENGINE.skybox_shader->compiled_program_id, "view_projection_matrix"), 1, GL_FALSE, (const void*)_view_projection_matrix);
	rde_vec_2I _mouse_pos = current_drawing_window->mouse_position;
	RDE_CHECK_GL(glUniform2f, glGetUniformLocation(ENGINE.skybox_shader->compiled_program_id, "mouse_position"), _mouse_pos.x, _mouse_pos.y);
	RDE_CHECK_GL(glUniform1f, glGetUniformLocation(ENGINE.skybox_shader->compiled_program_id, "mouse_position"), ENGINE.delta_time);

	RDE_CHECK_GL(glBindVertexArray, ENGINE.skybox.vao);
	RDE_CHECK_GL(glActiveTexture, GL_TEXTURE0);
	RDE_CHECK_GL(glBindTexture, GL_TEXTURE_CUBE_MAP, ENGINE.skybox.opengl_texture_id);
	RDE_CHECK_GL(glDrawArrays, GL_TRIANGLES, 0, 36);
	RDE_CHECK_GL(glBindVertexArray, 0);
	RDE_CHECK_GL(glDepthFunc, GL_LESS);
}



// ==============================================================================
// =							PRIVATE API - PHYSICS					 	  =
// ==============================================================================

#ifdef RDE_PHYSICS_MODULE

rde_vec_3F rde_inner_physics_rotate_box_point(rde_vec_3F _point, rde_vec_3F _rotation, rde_vec_3F _bounds) {
	mat4 _transformation_matrix = GLM_MAT4_IDENTITY_INIT;

	glm_translate(_transformation_matrix, (vec3) { _point.x, _point.y, _point.z });
	if(_rotation.x != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_rotation.x), (vec3){ 1.0f, 0.0f, 0.0f });
	}

	if(_rotation.y != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_rotation.y), (vec3){ 0.0f, 1.0f, 0.0f });
	}

	if(_rotation.z != 0) {
		glm_rotate(_transformation_matrix, glm_rad(_rotation.z), (vec3){ 0.0f, 0.0f, 1.0f });
	}

	vec4 _rotated_point;
	glm_mat4_mulv(_transformation_matrix, (vec4) { _bounds.x, _bounds.y, _bounds.z, 1.0f }, _rotated_point); 

	return (rde_vec_3F) { _rotated_point[0], _rotated_point[1], _rotated_point[2] };
}

void rde_physics_draw_debug_box(rde_transform* _transform, rde_jolt_box_shape_bounds* _box_bounds) {

	// Front and back faces
	for(int _i = 0; _i < 2; _i++) {
		float _depth = _i == 0 ? 1.0f : -1.0f;

		rde_vec_3F _p_0_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width, -_box_bounds->height, _box_bounds->depth * _depth });
		rde_vec_3F _p_0_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width, -_box_bounds->height, _box_bounds->depth * _depth });
		rde_rendering_3d_draw_line(_p_0_0, _p_0_1, RDE_COLOR_YELLOW, 1, NULL);


		rde_vec_3F _p_1_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width,  _box_bounds->height, _box_bounds->depth * _depth });
		rde_vec_3F _p_1_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width,  _box_bounds->height, _box_bounds->depth * _depth });
		rde_rendering_3d_draw_line(_p_1_0, _p_1_1, RDE_COLOR_YELLOW, 1, NULL);
	

		rde_vec_3F _p_2_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width, -_box_bounds->height, _box_bounds->depth * _depth });
		rde_vec_3F _p_2_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width,  _box_bounds->height, _box_bounds->depth * _depth });
		rde_rendering_3d_draw_line(_p_2_0, _p_2_1, RDE_COLOR_YELLOW, 1, NULL);
	

		rde_vec_3F _p_3_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width, -_box_bounds->height, _box_bounds->depth * _depth });
		rde_vec_3F _p_3_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width,  _box_bounds->height, _box_bounds->depth * _depth });
		rde_rendering_3d_draw_line(_p_3_0, _p_3_1, RDE_COLOR_YELLOW, 1, NULL);
	}
	


	// Top and bottom faces
	for(int _i = 0; _i < 2; _i++) {
		float _height = _i == 0 ? 1.0f : -1.0f;

		rde_vec_3F _p_0_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width,  _box_bounds->height * _height, -_box_bounds->depth });
		rde_vec_3F _p_0_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) { -_box_bounds->width,  _box_bounds->height * _height,  _box_bounds->depth });
		rde_rendering_3d_draw_line(_p_0_0, _p_0_1, RDE_COLOR_YELLOW, 1, NULL);
	

		rde_vec_3F _p_1_0 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width,  _box_bounds->height * _height, -_box_bounds->depth });
		rde_vec_3F _p_1_1 = rde_inner_physics_rotate_box_point(_transform->position, _transform->rotation, (rde_vec_3F) {  _box_bounds->width,  _box_bounds->height * _height, _box_bounds->depth });
		rde_rendering_3d_draw_line(_p_1_0, _p_1_1, RDE_COLOR_YELLOW, 1, NULL);
	}
}

void rde_physics_draw_debug_shapes_specific(rde_jolt_body* _body, rde_jolt_shape* _shape, rde_transform* _transform) {
	UNUSED(_body)

	RDE_JOLT_SHAPE_ _shape_type = rde_jolt_shape_get_type(_shape);
	switch(_shape_type) {
		case RDE_JOLT_SHAPE_BOX: {
			rde_jolt_box_shape_bounds _box_bounds;
			rde_jolt_shape_get_bounds(_shape, RDE_JOLT_SHAPE_BOX, (void*)&_box_bounds);
			rde_physics_draw_debug_box(_transform, &_box_bounds);
		} break;

		case RDE_JOLT_SHAPE_SPHERE: {
			rde_log_level(RDE_LOG_LEVEL_WARNING, "Rendering Debug Sphere is not implemented yet");
		} break;
	}
}

void rde_jolt_draw_debug_shapes(rde_window* _window, rde_camera* _camera) {
	rde_render_3d(_window, _camera, false, {
		rde_jolt_iterate_over_bodies(rde_physics_draw_debug_shapes_specific);
	})
}

#endif




// ==============================================================================
// =							PRIVATE API - EVENTS					 	   =
// ==============================================================================

COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_event_window_resize, {
	rde_vec_2I _new_window_size = _event->data.window_event_data.size;

	if(ENGINE.antialiasing.samples > 0) {
		rde_inner_rendering_destroy_current_antialiasing_config();
		rde_rendering_set_antialiasing(_window, (RDE_ANTIALIASING_)ENGINE.antialiasing.samples);		
	}

	rde_rendering_render_texture_update(DEFAULT_RENDER_TEXTURE, _new_window_size.x, _new_window_size.y);
	rde_rendering_render_texture_enable(DEFAULT_RENDER_TEXTURE);
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_focused_by_mouse, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_unfocused_by_mouse, {
	for(size_t _i = 0; _i < RDE_AMOUNT_OF_KEYS; _i++) {
		_window->key_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}

	for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOUSE_BUTTONS; _i++) {
		_window->mouse_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_focused_by_keyboard, {})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_unfocused_by_keyboard, {
	for(size_t _i = 0; _i < RDE_AMOUNT_OF_KEYS; _i++) {
		_window->key_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}

	for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOUSE_BUTTONS; _i++) {
		_window->mouse_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_moved, {
    for(size_t _i = 0; _i < RDE_AMOUNT_OF_KEYS; _i++) {
		_window->key_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}

	for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOUSE_BUTTONS; _i++) {
		_window->mouse_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_minimized, {
    for(size_t _i = 0; _i < RDE_AMOUNT_OF_KEYS; _i++) {
		_window->key_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}

	for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOUSE_BUTTONS; _i++) {
		_window->mouse_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_maximized, {
   	for(size_t _i = 0; _i < RDE_AMOUNT_OF_KEYS; _i++) {
		_window->key_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}

	for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOUSE_BUTTONS; _i++) {
		_window->mouse_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
	}
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(rde_inner_window_closed, { 
	size_t _existing_non_destroyed_windows = 0;
	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_windows; _i++) {
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
	} else if(_window->mouse_states[_event->data.mouse_event_data.button] == RDE_INPUT_STATUS_JUST_PRESSED) {
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
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_touch_down, {
	if(_window->mobile_states[_event->data.mobile_event_data.finger_id] == RDE_INPUT_STATUS_UNINITIALIZED || _window->mobile_states[_event->data.mobile_event_data.finger_id] == RDE_INPUT_STATUS_JUST_RELEASED) {
		_window->mobile_states[_event->data.mobile_event_data.finger_id] = RDE_INPUT_STATUS_JUST_PRESSED;
	} else if(_window->key_states[_event->data.mobile_event_data.finger_id] == RDE_INPUT_STATUS_JUST_PRESSED) {
		_window->mouse_states[_event->data.mobile_event_data.finger_id] = RDE_INPUT_STATUS_KEEP_PRESSED;
	}
})
COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(mobile_touch_up, {
	_window->mobile_states[_event->data.mobile_event_data.finger_id] = RDE_INPUT_STATUS_JUST_RELEASED;
})
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
			rde_vec_2I _window_size = rde_window_get_window_size(rde_engine_get_focused_window());
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN;
			_rde_event->data.mobile_event_data.init_touch_position = (rde_vec_2I) { _sdl_event->tfinger.x - _window_size.x * 0.5f, _sdl_event->tfinger.y - _window_size.y * 0.5f};
			_rde_event->data.mobile_event_data.pressure = _sdl_event->tfinger.pressure;
			_rde_event->data.mobile_event_data.finger_id = _sdl_event->tfinger.fingerId;
			_rde_event->time_stamp = _sdl_event->tfinger.timestamp;
			_rde_event->window_id = _sdl_event->tfinger.windowID;
		} break;

		case SDL_FINGERUP:{
			rde_vec_2I _window_size = rde_window_get_window_size(rde_engine_get_focused_window());
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_TOUCH_UP;
			_rde_event->data.mobile_event_data.end_touch_position = (rde_vec_2I) { _sdl_event->tfinger.x - _window_size.x * 0.5f, _sdl_event->tfinger.y - _window_size.y * 0.5f};
			_rde_event->data.mobile_event_data.pressure = _sdl_event->tfinger.pressure;
			_rde_event->data.mobile_event_data.finger_id = _sdl_event->tfinger.fingerId;
			_rde_event->time_stamp = _sdl_event->tfinger.timestamp;
			_rde_event->window_id = _sdl_event->tfinger.windowID;
		} break;

		case SDL_FINGERMOTION: {
			rde_vec_2I _window_size = rde_window_get_window_size(rde_engine_get_focused_window());
			_rde_event->type = RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED;
			_rde_event->time_stamp = _sdl_event->tfinger.timestamp;
			_rde_event->window_id = _sdl_event->tfinger.windowID;
			_rde_event->data.mobile_event_data.moved_touch_position = (rde_vec_2I) { _sdl_event->tfinger.x - _window_size.x * 0.5f, _sdl_event->tfinger.y - _window_size.y * 0.5f };
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
			_rde_event->data.mobile_event_data.init_touch_position = (rde_vec_2I) { _sdl_event->mgesture.x, _sdl_event->mgesture.y };
			_rde_event->data.mobile_event_data.pinch.rotation_of_fingers = rde_math_radians_to_degrees(_sdl_event->mgesture.dTheta);
			_rde_event->data.mobile_event_data.pinch.distance_moved_between_fingers = _sdl_event->mgesture.dDist;
			_rde_event->data.mobile_event_data.pinch.num_fingers_used = _sdl_event->mgesture.numFingers;
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
// =							PUBLIC API - EVENTS					 	    =
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
		ENGINE.drag_and_drop_events[_event_index](_event, _window);
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
	rde_window* _window = rde_engine_get_focused_window();

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
			for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOBILE_FINGERS; _i++) {
				_window->mobile_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
			}
		} break;

		case SDL_APP_DIDENTERBACKGROUND: {
			if(_terminated) {
				return 1;
			}
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Did Enter Background");
			ENGINE.pause_loop = true;
			for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOBILE_FINGERS; _i++) {
				_window->mobile_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
			}
		} break;

		case SDL_APP_WILLENTERFOREGROUND: {
			if(_terminated) {
				return 1;
			}
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Will Enter Foreground");
			ENGINE.pause_loop = false;
			for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOBILE_FINGERS; _i++) {
				_window->mobile_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
			}
		} break;

		case SDL_APP_DIDENTERFOREGROUND: {
			if(_terminated) {
				return 1;
			}
			rde_log_level(RDE_LOG_LEVEL_INFO, "Android App Did Enter Foreground");
			ENGINE.pause_loop = false;
			for(size_t _i = 0; _i < RDE_AMOUNT_OF_MOBILE_FINGERS; _i++) {
				_window->mobile_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
			}
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

bool rde_events_is_mobile_touch_just_pressed(rde_window* _window, uint _finger_id) {
	if(_finger_id >= RDE_AMOUNT_OF_MOBILE_FINGERS) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_MOBILE_FINGER_INPUT, RDE_AMOUNT_OF_MOBILE_FINGERS);
		return false;
	}
	return _window->mobile_states[_finger_id] == RDE_INPUT_STATUS_JUST_PRESSED;
}

bool rde_events_is_mobile_touch_pressed(rde_window* _window, uint _finger_id) {
	if(_finger_id >= RDE_AMOUNT_OF_MOBILE_FINGERS) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_MOBILE_FINGER_INPUT, RDE_AMOUNT_OF_MOBILE_FINGERS);
		return false;
	}
	return _window->mobile_states[_finger_id] == RDE_INPUT_STATUS_KEEP_PRESSED || _window->mobile_states[_finger_id] == RDE_INPUT_STATUS_JUST_PRESSED;
}

bool rde_events_is_mobile_touch_released(rde_window* _window, uint _finger_id) {
	if(_finger_id >= RDE_AMOUNT_OF_MOBILE_FINGERS) {
		rde_log_level(RDE_LOG_LEVEL_WARNING, RDE_WARNING_MOBILE_FINGER_INPUT, RDE_AMOUNT_OF_MOBILE_FINGERS);
		return false;
	}
	return _window->mobile_states[_finger_id] == RDE_INPUT_STATUS_JUST_RELEASED;
}

uint rde_events_mobile_get_finger_amount(rde_window* _window) {
	uint _fingers_in_use = 0;
	
	for (int _i = 0; _i < RDE_AMOUNT_OF_MOBILE_FINGERS; _i++) {
		if (rde_events_is_mobile_touch_pressed(_window, _i)) {
			_fingers_in_use++;
		}
	}

	return _fingers_in_use;
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

	for(int _i = 0; _i < RDE_AMOUNT_OF_MOBILE_FINGERS; _i++) {
		if(_window->mobile_states[_i] == RDE_INPUT_STATUS_JUST_PRESSED) {
			_window->mobile_states[_i] = RDE_INPUT_STATUS_KEEP_PRESSED;
		}

		if(_window->mobile_states[_i] == RDE_INPUT_STATUS_JUST_RELEASED) {
			_window->mobile_states[_i] = RDE_INPUT_STATUS_UNINITIALIZED;
		}
	}
}




#ifdef RDE_AUDIO_MODULE

// ==============================================================================
// =							PRIVATE API - AUDIO					 	    =
// ==============================================================================

// TODO: use this https://github.com/raysan5/raylib/blob/master/src/raudio.c as a reference, raylib does a more
// complex implementation that what I need, but it is a really good implementation on how to mix sounds with 
// mini audio and how to load different audio formats

void data_callback(ma_device* _device, void* _output, const void* _input, ma_uint32 _frame_count) {
	(void)_device;
	(void)_input;

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_sounds; _i++) {
		rde_sound* _sound = &ENGINE.sounds[_i];
		if(_sound->used && _sound->playing) {
			ma_data_source_read_pcm_frames(&_sound->miniaudio_decoder, _output, _frame_count, NULL);
		}
	}
}


// ==============================================================================
// =							PUBLIC API - AUDIO					 	     =
// ==============================================================================

void rde_audio_init(rde_sound_config _config) {
	ma_context _context;
	ma_device_info* _playback_devices_info;
	ma_uint32 _playback_device_count;
	ma_device_info* _capture_device_infos;
	ma_uint32 _capture_device_count;
	// ma_uint32 iDevice;

	rde_critical_error(ma_context_init(NULL, 0, NULL, &_context) != MA_SUCCESS, RDE_ERROR_MA_CONTEXT);
	rde_critical_error(ma_context_get_devices(&_context, &_playback_devices_info, &_playback_device_count, &_capture_device_infos, &_capture_device_count) != MA_SUCCESS, RDE_ERROR_MA_DEVICE_INFO);

	// printf("Playback Devices\n");
	// for (iDevice = 0; iDevice < _playback_device_count; ++iDevice) {
	// 	printf("    %u: %s\n", iDevice, _playback_devices_info[iDevice].name);
	// }

	ma_device_config _device_config = ma_device_config_init(ma_device_type_playback);
	_device_config.playback.format = DEFAULT_FORMAT;
	_device_config.playback.channels = _config.channels;
	//_device_config.playback.pDeviceID = &_playback_devices_info[2].id;
	_device_config.sampleRate = _config.rate;
	_device_config.dataCallback = data_callback;
	_device_config.pUserData = NULL;

	rde_critical_error(ma_device_init(NULL, &_device_config, &ENGINE.miniaudio_device) != MA_SUCCESS, RDE_ERROR_MA_DEVICE_INIT);
	rde_critical_error(ma_device_start(&ENGINE.miniaudio_device) != MA_SUCCESS, RDE_ERROR_MA_DEVICE_START);

	is_miniaudio_initialized = true;
	ENGINE.device_config = _config;
	rde_log_level(RDE_LOG_LEVEL_INFO, "Initiated audio module correctly");
}

rde_sound* rde_audio_load_sound(const char* _sound_path) {
	rde_sound* _sound = NULL;

	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_sounds; _i++) {
		rde_sound* _s = &ENGINE.sounds[_i];

		if(!_s->used) {
			_sound = _s;
			_sound->used = true;
			break;
		}
	}

	rde_critical_error(_sound == NULL, RDE_ERROR_MAX_LOADABLE_RESOURCE_REACHED, "sounds", ENGINE.init_info.heap_allocs_config.max_amount_of_sounds);
	ma_decoder_config _decoder_config = ma_decoder_config_init(DEFAULT_FORMAT, 
	                                                           ENGINE.device_config.channels, 
	                                                           ENGINE.device_config.rate);

	ma_result _result = ma_decoder_init_file(_sound_path, &_decoder_config, &_sound->miniaudio_decoder);
	rde_critical_error(_result != MA_SUCCESS, RDE_ERROR_MA_FILE_NOT_FOUND, _sound_path, _result);

	rde_log_level(RDE_LOG_LEVEL_INFO, "Loaded sound '%s' correctly", _sound_path);
	return _sound;
}

void rde_audio_unload_sound(rde_sound* _sound) {
	rde_critical_error(_sound == NULL, RDE_ERROR_NO_NULL_ALLOWED, "sound");
	ma_decoder_uninit(&_sound->miniaudio_decoder);
	_sound->used = false;
}

void rde_audio_play_sound(rde_sound* _sound) {
	rde_critical_error(_sound == NULL, RDE_ERROR_NO_NULL_ALLOWED, "sound");

	_sound->playing = true;
	_sound->paused = false;
	_sound->played_frame = 0;
}

void rde_audio_pause_sound(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED()
}

void rde_audio_stop_sound(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED()
}

void rde_audio_restart_sound(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED()
}

bool rde_audio_is_sound_playing(rde_sound* _sound) {
	UNUSED(_sound)
	UNIMPLEMENTED()
	return false;
}

bool rde_audio_set_sound_volume(rde_sound* _sound, float _volume) {
	UNUSED(_sound)
	UNUSED(_volume)
	UNIMPLEMENTED()
	return false;
}

void rde_audio_end() {
	for(size_t _i = 0; _i < ENGINE.init_info.heap_allocs_config.max_amount_of_sounds; _i++) {
		rde_sound* _sound = &ENGINE.sounds[_i];

		if(_sound->used) {
			rde_audio_unload_sound(_sound);
		}
	}

	if(is_miniaudio_initialized) {
		ma_device_uninit(&ENGINE.miniaudio_device);
	}
}

#endif


#if IS_MOBILE()
// ==============================================================================
// =							PUBLIC API - MOBILE					 	    =
// ==============================================================================

#if IS_ANDROID()
ANativeWindow* rde_android_get_native_window() {
	return ENGINE.android_native_window;
}
#endif
#endif






#ifdef RDE_ERROR_MODULE

// ==============================================================================
// =							PRIVATE API - ERROR					 	    =
// ==============================================================================

#if IS_WINDOWS()
	void rde_inner_print_stack_trace(FILE* _f) {
		void* _stack[RDE_MAX_STACK];
		HANDLE _process = GetCurrentProcess();

		SymInitialize(_process, NULL, TRUE);

		unsigned short _frames = CaptureStackBackTrace(0, RDE_MAX_STACK, _stack, NULL);
		SYMBOL_INFO* _symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
		IMAGEHLP_LINE* _line = (IMAGEHLP_LINE *)malloc(sizeof(IMAGEHLP_LINE));
		_line->SizeOfStruct = sizeof(IMAGEHLP_LINE);

		_symbol->MaxNameLen = 255;
		_symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

		for(int _i = 0; _i < _frames; _i++) {
			SymFromAddr(_process, (DWORD64)(_stack[_i] ), 0, _symbol);
			DWORD _dwDisplacement;
			SymGetLineFromAddr(_process, (DWORD64)(_stack[_i]), &_dwDisplacement, _line);

			if(strstr(_line->FileName, "errors.c") != NULL) {
				continue;
			}

			fprintf(_f, "%i: %s:%s:%lu - 0x%0llX\n", _frames - _i - 1, _line->FileName, _symbol->Name, _line->LineNumber, _symbol->Address);
		}

		free(_symbol);
		free(_line);
	}

	LONG WINAPI rde_inner_error_sig_handler(PEXCEPTION_POINTERS _sigfault_info) {
		printf("\n\n");
		switch(_sigfault_info->ExceptionRecord->ExceptionCode) {
			case EXCEPTION_ACCESS_VIOLATION: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_ACCESS_VIOLATION ERROR:The thread tried to read from or write to a virtual address for which it does not have the appropriate access.");
			} break;
			
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED ERROR: The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.");
			} break;

			case EXCEPTION_ILLEGAL_INSTRUCTION: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_ILLEGAL_INSTRUCTION ERROR: The thread tried to execute an invalid instruction. ");
			} break;

			case EXCEPTION_STACK_OVERFLOW: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_STACK_OVERFLOW ERROR: The thread used up its stack.");
			} break;

			case EXCEPTION_FLT_DIVIDE_BY_ZERO: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_FLT_DIVIDE_BY_ZERO ERROR: The thread tried to divide a floating-point value by a floating-point divisor of zero.");
			} break;

			case EXCEPTION_FLT_OVERFLOW: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_FLT_OVERFLOW ERROR: The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.");
			} break;

			case EXCEPTION_INT_DIVIDE_BY_ZERO: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_INT_DIVIDE_BY_ZERO ERROR: The thread tried to divide an integer value by an integer divisor of zero.");
			} break;

			case EXCEPTION_INT_OVERFLOW: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "EXCEPTION_INT_OVERFLOW ERROR: The result of an integer operation caused a carry out of the most significant bit of the result.");
			} break;

			default: {
				rde_log_level(RDE_LOG_LEVEL_ERROR, "UNHANDLED ERROR: Got an unhadled signal with value %lu", _sigfault_info->ExceptionRecord->ExceptionCode);
			} break;
		}

		#ifdef RDE_DEBUG
			rde_inner_print_stack_trace(stdout);
		#else
			FILE* _f = NULL;
			#if IS_WINDOWS()
				fopen_s(&_f, "rde_crash_logs.txt", "w");
			#else
				_f = fopen("rde_crash_logs.txt", "w");
			#endif
			rde_inner_print_stack_trace(_f);
			fclose(_f);

		#endif

		rde_engine_destroy_engine();
		exit(-1);

		return EXCEPTION_CONTINUE_SEARCH;
	}

#else

	void rde_inner_buf_printf(FILE* _f, const char* _fmt, ...) {
	    va_list _args;
	    va_start(_args, _fmt);
	    vfprintf(_f, _fmt, _args);
	    va_end(_args);
	}

	void rde_inner_print_stack_trace(FILE* _f) {
		rde_posix_stacktrace* _ret = (rde_posix_stacktrace*)malloc(sizeof(rde_posix_stacktrace));
	    _ret->trace_size = backtrace(_ret->trace, RDE_STACKTRACE_MAX_DEPTH);
	    const rde_posix_stacktrace* _stacktrace = (rde_posix_stacktrace*)(_ret);

	    char** _messages = backtrace_symbols(_stacktrace->trace, _stacktrace->trace_size);
	    rde_print_buf _out = { (char*) malloc(RDE_STACKTRACE_PRINT_BUFF_SIZE), 0, RDE_STACKTRACE_PRINT_BUFF_SIZE };

	    for (int i = 0; i < _stacktrace->trace_size; ++i) {
	        void* _tracei = _stacktrace->trace[i];
	        char* _msg = _messages[i];

	        /* calculate load offset */
	        Dl_info _info;
	        dladdr(_tracei, &_info);
	        if (_info.dli_fbase == (void*)0x400000) {
	            /* address from executable, so don't offset */
	            _info.dli_fbase = NULL;
	        }

	        while (*_msg && *_msg != '(') {
	        	 ++_msg;
	        }

	        *_msg = 0;
	        char _cmd[RDE_STACKTRACE_PRINT_BUFF_SIZE];
	        char _line[RDE_STACKTRACE_PRINT_LINE_BUFF_SIZE];

	        FILE* _fp;
	        snprintf(_cmd, RDE_STACKTRACE_PRINT_BUFF_SIZE, "addr2line -e %s -f -C -p %p 2>/dev/null", _messages[i], (void*)((char*)_tracei - (char*)_info.dli_fbase));

	        _fp = popen(_cmd, "r");
	        if (!_fp) {
	            rde_inner_buf_printf(_f, "Failed to generate trace further...\n");
	            break;
	        }

	        while (fgets(_line, sizeof(_line), _fp)) {
	            rde_inner_buf_printf(_f, "%s: ", _messages[i]);
	            if (strstr(_line, "?? ")) {
	                /* just output address if nothing can be found */
	                rde_inner_buf_printf(_f, "%p\n", _tracei);
	            }
	            else {
	                rde_inner_buf_printf(_f, "%s", _line);
	            }
	        }

	        pclose(_fp);
	    }

	    free(_messages);
	    free(_ret);

	    printf("%s \n", _out.buf);
	}

	void rde_inner_posix_signal_handler(int _sig, siginfo_t* _sig_info, void* _context) {
		(void)_context;

		FILE* _f = NULL;
	#ifdef RDE_DEBUG
	  	_f = stdout;
	#else
		_f = fopen("rde_crash_logs.txt", "w");
	#endif
	  
		switch(_sig) {
			case SIGSEGV:
				fprintf(_f, "Caught SIGSEGV: Segmentation Fault\n");
				break;
			case SIGINT:
				fprintf(_f, "stderr, Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n");
				break;
			case SIGFPE:
				switch(_sig_info->si_code) {
					case FPE_INTDIV:
						fprintf(_f, "Caught SIGFPE: (integer divide by zero)\n");
						break;
					case FPE_INTOVF:
						fprintf(_f, "Caught SIGFPE: (integer overflow)\n");
						break;
					case FPE_FLTDIV:
						fprintf(_f, "Caught SIGFPE: (floating-point divide by zero)\n");
						break;
					case FPE_FLTOVF:
						fprintf(_f, "Caught SIGFPE: (floating-point overflow)\n");
						break;
					case FPE_FLTUND:
						fprintf(_f, "Caught SIGFPE: (floating-point underflow)\n");
						break;
					case FPE_FLTRES:
						fprintf(_f, "Caught SIGFPE: (floating-point inexact result)\n");
						break;
					case FPE_FLTINV:
						fprintf(_f, "Caught SIGFPE: (floating-point invalid operation)\n");
						break;
					case FPE_FLTSUB:
						fprintf(_f, "Caught SIGFPE: (subscript out of range)\n");
						break;
					default:
						fprintf(_f, "Caught SIGFPE: Arithmetic Exception\n");
						break;
				}
			case SIGILL:
				switch(_sig_info->si_code) {
					case ILL_ILLOPC:
						fprintf(_f, "Caught SIGILL: (illegal opcode)\n");
						break;
					case ILL_ILLOPN:
						fprintf(_f, "Caught SIGILL: (illegal operand)\n");
						break;
					case ILL_ILLADR:
						fprintf(_f, "Caught SIGILL: (illegal addressing mode)\n");
						break;
					case ILL_ILLTRP:
						fprintf(_f, "Caught SIGILL: (illegal trap)\n");
						break;
					case ILL_PRVOPC:
						fprintf(_f, "Caught SIGILL: (privileged opcode)\n");
						break;
					case ILL_PRVREG:
						fprintf(_f, "Caught SIGILL: (privileged register)\n");
						break;
					case ILL_COPROC:
						fprintf(_f, "Caught SIGILL: (coprocessor error)\n");
						break;
					case ILL_BADSTK:
						fprintf(_f, "Caught SIGILL: (internal stack error)\n");
						break;
					default:
						fprintf(_f, "Caught SIGILL: Illegal Instruction\n");
						break;
				}
				break;
			case SIGTERM:
				fprintf(_f, "Caught SIGTERM: a termination request was sent to the program\n");
				break;
			case SIGABRT:
				fprintf(_f, "Caught SIGABRT: usually caused by an abort() or assert()\n");
				break;
			default:
				break;
		}

	  	rde_inner_print_stack_trace(_f);

	#ifndef RDE_DEBUG
		fclose(_f);
	#endif

	  exit(-1);
	}

	void rde_inner_set_posix_signal_handler() {
		stack_t _ss = {};
		uint8_t* _alternate_stack = (uint8_t*)malloc(sizeof(uint8_t) * RDE_STACKTRACE_BUFF_SIZE);
		_ss.ss_sp = (void*)_alternate_stack;
		_ss.ss_size = RDE_STACKTRACE_BUFF_SIZE;
		_ss.ss_flags = 0;

		#if !IS_MAC()
			if (sigaltstack(&_ss, NULL) != 0) { err(1, "sigaltstack"); }
		#endif

		struct sigaction _sig_action = {};
		_sig_action.sa_sigaction = rde_inner_posix_signal_handler;
		sigemptyset(&_sig_action.sa_mask);

		#ifdef __APPLE__
		    _sig_action.sa_flags = SA_SIGINFO;
		#else
		    _sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
		#endif

		if (sigaction(SIGSEGV, &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGFPE,  &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGINT,  &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGILL,  &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGTERM, &_sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGABRT, &_sig_action, NULL) != 0) { err(1, "sigaction"); }
	}
#endif



// ==============================================================================
// =							PUBLIC API - ERROR					 	     =
// ==============================================================================

void rde_critical_error(bool _condition, const char* _fmt, ...) {
	
	if(!_condition) {
		return;
	}

	times_enter_in_error++;

	if(times_enter_in_error > 1) {
		rde_log_level(RDE_LOG_LEVEL_ERROR, "There was an error loop, check previous message to fix it. \n");
		exit(-1);
	}

#ifdef RDE_DEBUG
	rde_log_level(RDE_LOG_LEVEL_ERROR, "An error made the program crash, check 'rde_crash_logs.txt'");
#else
	rde_log_level(RDE_LOG_LEVEL_ERROR, "An error made the program crash, check below");
#endif

#if IS_ANDROID()
	va_list _args;
	va_start(_args, _fmt);
	char* _error = (char*)malloc(sizeof(char) * 10000);
	memset(_error, 0, 10000);
	vsprintf(_error, _fmt, _args);
	va_end(_args);
	__android_log_print(ANDROID_LOG_DEBUG, "SDL_RDE", "%s", _error);
	free(_error);
#else
	FILE* _f = NULL;
	
	#if IS_WINDOWS()
		fopen_s(&_f, "rde_crash_logs.txt", "w");
	#else
		_f = fopen("rde_crash_logs.txt", "w");
	#endif

	va_list _args;
	va_start(_args, _fmt);
	
	#if defined(RDE_DEBUG)
		vfprintf(stdout, _fmt, _args);
		#ifdef RDE_ERROR_MODULE
		rde_inner_print_stack_trace(stdout);
		#endif
	#else
		vfprintf(_f, _fmt, _args);
		#if defined(RDE_ERROR_MODULE)
		rde_inner_print_stack_trace(_f);
		#endif
	#endif
		va_end(_args);
		fclose(_f);
#endif
	
	rde_engine_destroy_engine();

#ifdef RDE_DEBUG
	assert(false);
#else
	exit(-1);
#endif
}

#endif
