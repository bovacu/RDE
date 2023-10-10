#ifndef RDE_H
#define RDE_H

//#define MINIAUDIO_IMPLEMENTATION
//#include "miniaudio/miniaudio.h"
//#include <stdlib.h>
//#include <stdio.h>

// This is needed because of C++ name mangling, if this lib is linked to a C++ project and is compiled,
// without extern "C" won't work.
#ifdef __cplusplus
extern "C" {
#endif

// Index Begin
//
//		Defines:
//			- Numeric constants
//			- Shaders
//			- Compilation and export
//			- Util
//			- Warning silencer
//			- Generic funcs and structs
//			- Platform specific types
//		
//		Enums:
//			- State
//			- Events
//			- Keys
//			- Mouse
//			- Controller
//			- Platform
//			- Rendering
//			- File system
//			- Physics
//
//		Structs:
//			- Math
//			- Callbacks and function pointers
//			- Engine
//			- Events
//			- Rendering
//			- Audio
//			- Physics
//		
//		Functions:
// 			- Math
//			- Logging
//			- Util
//			- Engine
//			- Window
//			- Events
//			- Rendering
//			- Audio
//			- Physics
//			- File system
//
//		Global variables:
//			- Colors
//
// Index End


#include <assert.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>

/// *************************************************************************************************
/// *                                		DEFINES                         						*
/// *************************************************************************************************

/// ========================= NUMERIC CONSTANTS ============================

#define RDE_EPSILON 0.0001f
#define RDE_PI 3.141592741f

#define RDE_MAX_NUMBER_OF_WINDOWS 10
#define RDE_MAX_VERTICES_PER_BATCH 50000
#define RDE_MAX_LOADABLE_SHADERS 256

#ifdef RDE_RENDERING_2D_MODULE
#define RDE_MAX_LOADABLE_TEXTURES 512
#define RDE_MAX_LOADABLE_ATLASES 512
#define RDE_MAX_LOADABLE_FONTS 512
#endif

#ifdef RDE_RENDERING_3D_MODULE
#define RDE_MAX_LOADABLE_MODELS 512
#define RDE_MAX_LOADABLE_MODELS_TEXTURES 512
#endif

#ifdef	RDE_AUDIO_MODULE
#define RDE_MAX_LOADABLE_SOUNDS 128
#endif

#define RDE_INT_MAX 2147483647
#define RDE_INT_MIN (-2147483647 - 1)
#define RDE_LONG_MAX 2147483647
#define RDE_LONG_MIN (-2147483647 - 1)
#define RDE_FLOAT_MAX 3.402823466 E + 38
#define RDE_FLOAT_MIN 1.175494351 E - 38
#define RDE_DOUBLE_MAX 1.7976931348623158 E + 308
#define RDE_DOUBLE_MIN 2.2250738585072014 E - 308
#define RDE_UINT_MAX 4294967295
#define RDE_ULONG_MAX 4294967295

/// ============================== SHADERS =================================

// TODO: multiply by view_projection_matrix
#define RDE_COLOR_VERTEX_SHADER_2D "" \
	"#version 330 core\n" \
	"\n" \
	"layout(location = 0) in vec2 in_position;\n" \
	"layout(location = 1) in vec4 in_color;\n" \
	"layout(location = 2) in vec2 in_uv;\n" \
	"out vec4 color;\n" \
	"\n" \
	"uniform mat4 view_projection_matrix;\n" \
	"\n" \
	"void main() {\n" \
	"	gl_Position = view_projection_matrix * vec4(in_position, 0.0, 1.0);\n" \
	"	color = in_color;\n" \
	"}\n"

#define RDE_COLOR_FRAGMENT_SHADER_2D "" \
	"#version 330 core\n" \
	"\n" \
	"in vec4 color;\n" \
	"out vec4 out_color;\n" \
	"\n" \
	"void main() {\n" \
	"	out_color = vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);\n" \
	"}\n" 

// TODO: multiply by view_projection_matrix
#define RDE_TEXTURE_VERTEX_SHADER_2D "" \
	"#version 330 core\n" \
	"\n" \
	"layout(location = 0) in vec2 in_position;\n" \
	"layout(location = 1) in vec4 in_color;\n" \
	"layout(location = 2) in vec2 in_uv;\n" \
	"\n" \
	"uniform mat4 view_projection_matrix;\n" \
	"\n" \
	"out vec2 uv;\n" \
	"out vec4 color;\n" \
	"\n" \
	"void main(void) {\n" \
	"	uv = in_uv;\n" \
	"	color = in_color;\n" \
	"	gl_Position = view_projection_matrix * vec4(in_position, 0.0, 1.0);\n" \
	"}\n" 

#define RDE_TEXTURE_FRAGMENT_SHADER_2D "" \
	"#version 330 core\n" \
	"\n" \
	"in vec2 uv;\n" \
	"in vec4 color;\n" \
	"\n" \
	"uniform sampler2D tex;\n" \
	"layout(location = 0) out vec4 out_color;\n" \
	"\n" \
	"void main(void) {\n" \
	"	out_color = texture(tex, uv) * vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);\n" \
	"}\n" 

#define RDE_TEXT_VERTEX_SHADER_2D "" \
	"#version 330 core\n" \
	"\n" \
	"layout(location = 0) in vec2 in_position;\n" \
	"layout(location = 1) in vec4 in_color;\n" \
	"layout(location = 2) in vec2 in_uv;\n" \
	"\n" \
	"uniform mat4 view_projection_matrix;\n" \
	"\n" \
	"out vec2 uv;\n" \
	"out vec4 color;\n" \
	"\n" \
	"void main(void) {\n" \
	"	uv = in_uv;\n" \
	"	color = in_color;\n" \
	"	gl_Position = view_projection_matrix * vec4(in_position, 0.0, 1.0);\n" \
	"}\n"

#define RDE_TEXT_FRAGMENT_SHADER_2D "" \
	"#version 330 core\n" \
	"\n" \
	"in vec2 uv;\n" \
	"in vec4 color;\n" \
	"\n" \
	"uniform sampler2D tex;\n" \
	"out vec4 out_color;\n" \
	"\n" \
	"void main(void) {\n" \
	"	float d = texture(tex, uv).r;\n" \
	"	float aaf = fwidth(d);\n" \
	"	float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);\n" \
	"	out_color = vec4(color.rgb, alpha);\n" \
	"}\n"

#define RDE_FRAME_BUFFER_VERTEX_SHADER "" \
	"#version 330 core\n" \
	"layout (location = 0) in vec2 in_pos;\n" \
	"layout (location = 1) in vec2 in_tex_coords;\n" \
	"\n" \
	"out vec2 tex_coords;\n" \
	"uniform mat4 view_projection_matrix;\n" \
	"\n" \
	"void main() {\n" \
	"	gl_Position = vec4(in_pos.x, in_pos.y, 0.0, 1.0);\n" \
	"	tex_coords = in_tex_coords;\n" \
	"}\n"

#define RDE_FRAME_BUFFER_FRAGMENT_SHADER "" \
	"#version 330 core\n" \
	"\n" \
	"in vec2 tex_coords;\n" \
	"\n" \
	"uniform sampler2D screen_texture;\n" \
	"layout(location = 0) out vec4 out_color;\n" \
	"\n" \
	"void main() {\n" \
	"	out_color = texture(screen_texture, tex_coords);\n" \
	"}\n"

#define RDE_MESH_VERTEX_SHADER "" \
	"#version 330 core\n" \
	"\n" \
	"layout(location = 0) in vec3 in_pos;\n" \
	"layout(location = 1) in vec4 in_color;\n" \
	"layout(location = 2) in vec3 in_normal;\n" \
	"layout(location = 3) in vec2 in_text_coord;\n" \
	"\n" \
	"out vec4 color;\n" \
	"out vec3 normal;\n" \
	"out vec2 text_coord;\n" \
	"out vec3 frag_pos;\n" \
	"out mat4 model_matrix;\n" \
	"uniform mat4 view_projection_matrix;\n" \
	"uniform mat4 models[100];\n" \
	"\n" \
	"void main(){	\n" \
	"	mat4 _model = models[gl_InstanceID];\n" \
	"	gl_Position = view_projection_matrix * _model * vec4(in_pos, 1);\n" \
	"	color = in_color;\n" \
	"	normal = in_normal;\n" \
	"	frag_pos = in_pos;\n" \
	"	text_coord = in_text_coord;\n" \
	"	model_matrix = _model;\n" \
	"}"

#define RDE_MESH_FRAGMENT_SHADER "" \
	"#version 330 core\n" \
	"\n" \
	"in vec4 color;\n" \
	"in vec3 normal;\n" \
	"in vec2 text_coord;\n" \
	"in vec3 frag_pos;\n" \
	"in mat4 model_matrix;\n" \
	"\n" \
	"uniform sampler2D tex;\n" \
	"\n" \
	"out vec4 out_color;\n" \
	"vec3 lightColor = vec3(1.0, 1.0, 1.0);\n" \
	"vec3 lightPos = vec3(7.361173, 6.458405, 13.681020);\n" \
	"float ambientStrength = 1.0;\n" \
	"vec3 ambient = ambientStrength * lightColor;\n"\
	"\n" \
	"void main(){\n" \
	"	mat3 normalMatrix = transpose(inverse(mat3(model_matrix))); \n" \
	"	vec3 normal = normalize(normalMatrix * normal); \n" \
	"	vec3 fragPosition = vec3(model_matrix * vec4(frag_pos, 1)); \n" \
	"	vec3 surfaceToLight = lightPos - fragPosition; \n" \
	"	float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal)); \n" \
	"	brightness = clamp(brightness, 0, 1) * ambientStrength; \n" \
	"	vec4 surfaceColor = texture(tex, text_coord); \n" \
	"	vec4 color_norm = vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f); \n" \
	"	gl_FragColor = vec4(brightness * lightColor * surfaceColor.rgb * color_norm.rgb, surfaceColor.a * color_norm.a) ;\n" \
	"}"

//#define RDE_MESH_FRAGMENT_SHADER "" \
//	"#version 330 core\n" \
//	"\n" \
//	"in vec4 color;\n" \
//	"in vec3 normal;\n" \
//	"in vec2 text_coord;\n" \
//	"in vec3 frag_pos;\n" \
//	"\n" \
//	"uniform sampler2D tex;\n" \
//	"\n" \
//	"out vec4 out_color;\n" \
//	"vec3 lightColor = vec3(1.0, 1.0, 1.0);\n" \
//	"vec3 lightPos = vec3(7.361173, 6.458405, 13.681020);\n" \
//	"float ambientStrength = 0.6;\n" \
//    "vec3 ambient = ambientStrength * lightColor;\n"\
//	"\n" \
//	"void main(){\n" \
//	"	vec3 _norm = normalize(normal);\n" \
//	"	vec3 lightDir = normalize(lightPos - frag_pos);\n" \
//	"	float diff = max(dot(_norm, lightDir), 0.0);\n" \
//	"	vec3 diffuse = diff * lightColor;\n" \
//	"	gl_FragColor = texture(tex, text_coord) * vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f) * vec4(ambient + diffuse, 1.0);\n" \
//	"}"


#define RDE_COLOR_VERTEX_SHADER_2D_ES "" \
	"#version 300 es\n" \
	"\n" \
	"layout(location = 0) in vec2 position;\n" \
	"layout(location = 1) in vec4 color;\n" \
	"out vec4 color_from_vshader;\n" \
	"\n" \
	"uniform mat4 view_projection_matrix;\n" \
	"\n" \
	"void main() {\n" \
	"	gl_Position = view_projection_matrix * vec4(position, 0.0, 1.0);\n" \
	"	color_from_vshader = color;\n" \
	"}\n"

#define RDE_COLOR_FRAGMENT_SHADER_2D_ES "" \
	"#version 300 es\n" \
	"\n" \
	"precision mediump float;\n" \
	"\n" \
	"in vec4 color_from_vshader;\n" \
	"out vec4 out_color;\n" \
	"\n" \
	"void main() {\n" \
	"	out_color = color_from_vshader;\n" \
	"}\n"

#define RDE_TEXTURE_VERTEX_SHADER_2D_ES "" \
	"#version 300 es\n" \
	"\n" \
	"layout(location = 0) in vec2 in_position;\n" \
	"layout(location = 1) in vec4 in_color;\n" \
	"layout(location = 2) in vec2 in_uv;\n" \
	"\n" \
	"uniform mat4 view_projection_matrix;\n" \
	"\n" \
	"out vec2 uv;\n" \
	"out vec4 color;\n" \
	"\n" \
	"void main(void) {\n" \
	"	uv = in_uv;\n" \
	"	color = in_color;\n" \
	"	gl_Position = view_projection_matrix * vec4(in_position, 0.0, 1.0);\n" \
	"}\n"

#define RDE_TEXTURE_FRAGMENT_SHADER_2D_ES "" \
	"#version 300 es\n" \
	"\n" \
	"precision mediump float;\n" \
	"\n" \
	"in vec2 uv;\n" \
	"in vec4 color;\n" \
	"\n" \
	"uniform sampler2D tex;\n" \
	"\n" \
	"out vec4 out_color;\n" \
	"\n" \
	"void main(void) {\n" \
	"	out_color = texture(tex, uv) * vec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);\n" \
	"}\n"

#define RDE_FRAME_BUFFER_VERTEX_SHADER_ES "" \
	"#version 300 es\n" \
	"layout (location = 10) in vec2 in_pos;\n" \
	"layout (location = 11) in vec2 in_tex_coords;\n" \
	"\n" \
	"out vec2 tex_coords;\n" \
	"\n" \
	"void main() {\n" \
	"	gl_Position = vec4(in_pos.x, in_pos.y, 0.0, 1.0);\n" \
	"	tex_coords = in_tex_coords;\n" \
	"}\n"

#define RDE_FRAME_BUFFER_FRAGMENT_SHADER_ES "" \
	"#version 300 es\n" \
	"\n" \
	"precision mediump float;\n" \
	"\n" \
	"in vec2 tex_coords;\n" \
	"out vec4 out_color;\n" \
	"\n" \
	"uniform sampler2D screen_texture;\n" \
	"\n" \
	"void main() {\n" \
	"	out_color = texture(screen_texture, tex_coords);\n" \
	"}\n"

/// ====================== COMPILATION AND EXPORT ==========================

#if defined(__APPLE__)
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE
		#ifndef IOS_PLATFORM
			#define IOS_PLATFORM
		#endif
	#elif TARGET_OS_MAC
		#define MAC_PLATFORM
	#endif
#endif

#define IS_MAC() (defined(__APPLE__) && defined(MAC_PLATFORM))
#define IS_WINDOWS() _WIN32
#define IS_WASM() __EMSCRIPTEN__
#define IS_LINUX() (defined(__linux__))
#define IS_DESKTOP() (IS_LINUX() || IS_MAC() || IS_WINDOWS())
#define IS_IOS() (defined(__APPLE__) && defined(IOS_PLATFORM))
#define IS_APPLE() (IS_MAC() || IS_IOS())
#define IS_ANDROID() (defined(__ANDROID__))
#define IS_MOBILE() (IS_ANDROID() || IS_IOS())

#ifdef RDE_EXPORT
	#define RDE_API [[gnu::visibility("default")]]
	#define RDE_HIDDEN [[gnu::visibility("hidden")]]
	#define RDE_INTERNAL [[gnu::visibility("internal")]]
	#define RDE_DEPRECATED(_explanation) [[gnu::deprecated(_explanation)]]
#endif

#if IS_WINDOWS()
	#define RDE_FUNC __declspec(dllexport)
	#define RDE_FUNC_STATIC __declspec(dllexport) static
	#define RDE_FUNC_EXTERNAL extern "C" RDE_FUNC
#else
	#define RDE_FUNC 
	#define RDE_FUNC_STATIC static
	#define RDE_FUNC_EXTERNAL
#endif

#if IS_WINDOWS()
	#include <io.h>
	#include <fcntl.h>
	#include <windows.h>
#endif			 

#define RDE_SHOW_WINDOWS_CONSOLE 							\
	if(AllocConsole()) {									\
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);	\
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);		\
	}

/// ============================== UTIL ====================================

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define LINE_STRING STRINGIZE(__LINE__)
#define FUNC_STRING STRINGIZE(__func__)
#define FILE_STRING STRINGIZE(__FILE__)

#define RDE_COLOR_TO_HEX_COLOR(_color) (((int)_color.a) << 24) + (((int)_color.b) << 16) + (((int)_color.g) << 8) + ((int)_color.r)

/// ======================= WARNING SILENCER ===============================

#define UNUSED(_x) (void)_x;

#define UNIMPLEMENTED(_message) {				\
		assert(false);							\
	}

#define UNIMPLEMENTED_STRUCT() short foo;



/// ==================== GENERIC FUNCS AND STRUCTS ==========================

#if IS_WINDOWS()
#define RDE_MAIN(_window, _heap_allocs_config, _mandatory_callbacks, _init_func, _end_func)	\
	int main(int _argc, char** _argv) {														\
		RDE_SHOW_WINDOWS_CONSOLE															\
																							\
		_window = rde_engine_create_engine(_argc, _argv, _heap_allocs_config);				\
		rde_setup_initial_info(_mandatory_callbacks);										\
																							\
		_init_func(_argc, _argv);															\
																							\
		rde_engine_on_run();																\
		_end_func();																		\
		rde_engine_destroy_engine();														\
																							\
		return 0;																			\
	}
#else
#define RDE_MAIN(_window, _heap_allocs_config, _mandatory_callbacks, _init_func, _end_func)	\
	int main(int _argc, char** _argv) {														\
		_window = rde_engine_create_engine(_argc, _argv, _heap_allocs_config);				\
		rde_setup_initial_info(_mandatory_callbacks);										\
																							\
		_init_func(_argc, _argv);															\
																							\
		rde_engine_on_run();																\
		_end_func();																		\
		rde_engine_destroy_engine();														\
																							\
		return 0;																			\
	}
#endif

#define SPECIALIZED_VEC2(_type, _name) 	\
	typedef struct {					\
		_type x;						\
		_type y;						\
	} _name;

#define SPECIALIZED_VEC3(_type, _name) 	\
	typedef struct {					\
		_type x;						\
		_type y;						\
		_type z;						\
	} _name;

#define SPECIALIZED_VEC4(_type, _name) 	\
	typedef struct {					\
		_type x;						\
		_type y;						\
		_type z;						\
		_type w;						\
	} _name;

#define SPECIALIZED_MAT2(_type, _name) 	\
	typedef struct {					\
		union {							\
			struct {					\
				float m00, m01;			\
				float m10, m11;			\
			};							\
										\
			float m[2][2];				\
			float v[4];					\
		};								\
	} _name;

#define SPECIALIZED_MAT3(_type, _name) 	\
	typedef struct {					\
		union {							\
			struct {					\
				float m00, m01, m02;	\
				float m10, m11, m12;	\
				float m20, m21, m22;	\
			};							\
										\
			float m[3][3];				\
			float v[9];					\
		};								\
	} _name;

#define SPECIALIZED_MAT4(_type, _name) 		\
	typedef struct {						\
		union {								\
			struct {						\
				float m00, m01, m02, m03;	\
				float m10, m11, m12, m13;	\
				float m20, m21, m22, m23;	\
				float m30, m31, m32, m33;	\
			};								\
											\
			float m[4][4];					\
			float v[16];					\
		};									\
	} _name;

#define COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(_func_name, _callback_type, _callback_name, _extra_code) 	\
	void _func_name(rde_window* _window, rde_event* _event) {					\
		UNUSED(_window);																					\
		if(!ENGINE._callback_type._callback_name.block_engine_default_implementation) {					\
			_extra_code																						\
		}																									\
																											\
		if(ENGINE._callback_type._callback_name._callback != NULL) {									\
			ENGINE._callback_type._callback_name._callback(_event);										\
		}																									\
	}

/// ====================== PLATFORM SPECIFIC TYPES ==========================

typedef unsigned long ulong;
typedef unsigned int uint;






/// *************************************************************************************************
/// *                                		  ENUMS                         						*
/// *************************************************************************************************

/// ================================= STATE =================================

typedef enum {
	RDE_INPUT_STATUS_ERROR 			=-1,
	RDE_INPUT_STATUS_UNINITIALIZED 	= 0,
	RDE_INPUT_STATUS_JUST_PRESSED 	= 1,
	RDE_INPUT_STATUS_KEEP_PRESSED 	= 2,
	RDE_INPUT_STATUS_JUST_RELEASED 	= 3,
	RDE_INPUT_STATUS_KEEP_RELEASED 	= 4,
} RDE_INPUT_STATUS_;

typedef enum {
	RDE_MOUSE_STATUS_ENTERED,
	RDE_MOUSE_STATUS_EXITED
} RDE_MOUSE_STATUS_;



/// ================================ EVENTS ================================

typedef enum {
	RDE_EVENT_TYPE_NONE, 
	
	RDE_EVENT_TYPE_WINDOW_BEGIN,
	RDE_EVENT_TYPE_WINDOW_CLOSED, RDE_EVENT_TYPE_WINDOW_RESIZED, RDE_EVENT_TYPE_WINDOW_MOVED, RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED, RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED,
	RDE_EVENT_TYPE_WINDOW_KEYBOARD_FOCUSED, RDE_EVENT_TYPE_WINDOW_KEYBOARD_UNFOCUSED, RDE_EVENT_TYPE_WINDOW_MINIMIZED, RDE_EVENT_TYPE_WINDOW_MAXIMIZED, RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED,
	RDE_EVENT_TYPE_WINDOW_END,

	RDE_EVENT_TYPE_DISPLAY_BEGIN,
	RDE_EVENT_TYPE_DISPLAY_CONNECTED, RDE_EVENT_TYPE_DISPLAY_DISCONNECTED, RDE_EVENT_TYPE_DISPLAY_CHANGED_ORIENTATION,
	RDE_EVENT_TYPE_DISPLAY_END,	

	RDE_EVENT_TYPE_GAME_FRAME_BEGIN,
	RDE_EVENT_TYPE_GAME_FRAME, RDE_EVENT_TYPE_GAME_UPDATE, RDE_EVENT_TYPE_GAME_RENDER,
	RDE_EVENT_TYPE_GAME_FRAME_END,	

	RDE_EVENT_TYPE_KEY_BEGIN,
	RDE_EVENT_TYPE_KEY_PRESSED, RDE_EVENT_TYPE_KEY_RELEASED, RDE_EVENT_TYPE_KEY_TYPED, RDE_EVENT_TYPE_TEXT_TYPED, 
	RDE_EVENT_TYPE_KEY_END,	

	RDE_EVENT_TYPE_MOUSE_BEGIN,
	RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED, RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED, RDE_EVENT_TYPE_MOUSE_SCROLLED, RDE_EVENT_TYPE_MOUSE_MOVED,
	RDE_EVENT_TYPE_MOUSE_END,

	RDE_EVENT_TYPE_CONTROLLER_AXIS_MOVED, RDE_EVENT_TYPE_CONTROLLER_BUTTON_DOWN, RDE_EVENT_TYPE_CONTROLLER_BUTTON_UP,

	RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN, RDE_EVENT_TYPE_MOBILE_TOUCH_UP, RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED
} RDE_EVENT_TYPE_;



/// =============================== KEYBOARD ================================

typedef enum {
	// From SDL2
	RDE_KEYBOARD_KEY_SPACE = 44,
	RDE_KEYBOARD_KEY_APOSTROPHE = 52,
	RDE_KEYBOARD_KEY_COMMA = 54,
	RDE_KEYBOARD_KEY_MINUS = 45,
	RDE_KEYBOARD_KEY_PERIOD = 55,
	RDE_KEYBOARD_KEY_SLASH = 56,

	RDE_KEYBOARD_KEY_D0 = 39,
	RDE_KEYBOARD_KEY_D1 = 30,
	RDE_KEYBOARD_KEY_D2 = 31,
	RDE_KEYBOARD_KEY_D3 = 32,
	RDE_KEYBOARD_KEY_D4 = 33,
	RDE_KEYBOARD_KEY_D5 = 34,
	RDE_KEYBOARD_KEY_D6 = 35,
	RDE_KEYBOARD_KEY_D7 = 36,
	RDE_KEYBOARD_KEY_D8 = 37,
	RDE_KEYBOARD_KEY_D9 = 38,

	RDE_KEYBOARD_KEY_SEMICOLON = 51,
	RDE_KEYBOARD_KEY_EQUAL = 46,

	RDE_KEYBOARD_KEY_A = 4,
	RDE_KEYBOARD_KEY_B = 5,
	RDE_KEYBOARD_KEY_C = 6,
	RDE_KEYBOARD_KEY_D = 7,
	RDE_KEYBOARD_KEY_E = 8,
	RDE_KEYBOARD_KEY_F = 9,
	RDE_KEYBOARD_KEY_G = 10,
	RDE_KEYBOARD_KEY_H = 11,
	RDE_KEYBOARD_KEY_I = 12,
	RDE_KEYBOARD_KEY_J = 13,
	RDE_KEYBOARD_KEY_K = 14,
	RDE_KEYBOARD_KEY_L = 15,
	RDE_KEYBOARD_KEY_M = 16,
	RDE_KEYBOARD_KEY_N = 17,
	RDE_KEYBOARD_KEY_O = 18,
	RDE_KEYBOARD_KEY_P = 19,
	RDE_KEYBOARD_KEY_Q = 20,
	RDE_KEYBOARD_KEY_R = 21,
	RDE_KEYBOARD_KEY_S = 22,
	RDE_KEYBOARD_KEY_T = 23,
	RDE_KEYBOARD_KEY_U = 24,
	RDE_KEYBOARD_KEY_V = 25,
	RDE_KEYBOARD_KEY_W = 26,
	RDE_KEYBOARD_KEY_X = 27,
	RDE_KEYBOARD_KEY_Y = 28,
	RDE_KEYBOARD_KEY_Z = 29,

	RDE_KEYBOARD_KEY_LEFT_BRACKET = 47,
	RDE_KEYBOARD_KEY_BACK_SLASH = 49,
	RDE_KEYBOARD_KEY_RIGHT_BRACKET = 48,
	RDE_KEYBOARD_KEY_GRACE_ACCENT = 53,

	RDE_KEYBOARD_KEY_ESCAPE = 41,
	RDE_KEYBOARD_KEY_ENTER = 40,
	RDE_KEYBOARD_KEY_TAB = 43,
	RDE_KEYBOARD_KEY_BAKCSPACE = 42,
	RDE_KEYBOARD_KEY_INSERT = 73,
	RDE_KEYBOARD_KEY_DELETE = 76,
	RDE_KEYBOARD_KEY_RIGHT = 79,
	RDE_KEYBOARD_KEY_LEFT = 80,
	RDE_KEYBOARD_KEY_DOWN = 81,
	RDE_KEYBOARD_KEY_UP = 82,
	RDE_KEYBOARD_KEY_PAGE_UP = 75,
	RDE_KEYBOARD_KEY_PAGE_DOWN = 78,
	RDE_KEYBOARD_KEY_HOME = 74,
	RDE_KEYBOARD_KEY_END = 77,
	RDE_KEYBOARD_KEY_CAPS_LOCK = 57,
	RDE_KEYBOARD_KEY_SCROLL_LOCK = 71,
	RDE_KEYBOARD_KEY_NUM_LOCK = 83,
	RDE_KEYBOARD_KEY_PRINT_SCREEN = 70,
	RDE_KEYBOARD_KEY_PAUSE = 72,
	RDE_KEYBOARD_KEY_F1 = 58,
	RDE_KEYBOARD_KEY_F2 = 59,
	RDE_KEYBOARD_KEY_F3 = 60,
	RDE_KEYBOARD_KEY_F4 = 61,
	RDE_KEYBOARD_KEY_F5 = 62,
	RDE_KEYBOARD_KEY_F6 = 63,
	RDE_KEYBOARD_KEY_F7 = 64,
	RDE_KEYBOARD_KEY_F8 = 65,
	RDE_KEYBOARD_KEY_F9 = 66,
	RDE_KEYBOARD_KEY_F10 = 67,
	RDE_KEYBOARD_KEY_F11 = 68,
	RDE_KEYBOARD_KEY_F12 = 69,
	RDE_KEYBOARD_KEY_F13 = 104,
	RDE_KEYBOARD_KEY_F14 = 105,
	RDE_KEYBOARD_KEY_F15 = 106,
	RDE_KEYBOARD_KEY_F16 = 107,
	RDE_KEYBOARD_KEY_F17 = 108,
	RDE_KEYBOARD_KEY_F18 = 109,
	RDE_KEYBOARD_KEY_F19 = 110,
	RDE_KEYBOARD_KEY_F20 = 111,
	RDE_KEYBOARD_KEY_F21 = 112,
	RDE_KEYBOARD_KEY_F22 = 113,
	RDE_KEYBOARD_KEY_F23 = 114,
	RDE_KEYBOARD_KEY_F24 = 115,

	/* Keypad */
	RDE_KEYBOARD_KEY_KP0 = 98,
	RDE_KEYBOARD_KEY_KP1 = 89,
	RDE_KEYBOARD_KEY_KP2 = 90,
	RDE_KEYBOARD_KEY_KP3 = 91,
	RDE_KEYBOARD_KEY_KP4 = 92,
	RDE_KEYBOARD_KEY_KP5 = 93,
	RDE_KEYBOARD_KEY_KP6 = 94,
	RDE_KEYBOARD_KEY_KP7 = 95,
	RDE_KEYBOARD_KEY_KP8 = 96,
	RDE_KEYBOARD_KEY_KP9 = 97,

	RDE_KEYBOARD_KEY_LEFT_SHIFT = 225,
	RDE_KEYBOARD_KEY_LEFT_CTRL = 224,
	RDE_KEYBOARD_KEY_LEFT_ALT = 226,
	RDE_KEYBOARD_KEY_RIGHT_SHIFT = 229,
	RDE_KEYBOARD_KEY_RIGHT_CTRL = 228,
	RDE_KEYBOARD_KEY_RIGHT_ALT = 230,
	RDE_KEYBOARD_KEY_MENU = 118,
	RDE_KEYBOARD_KEY_NONE = 0xFFFF
} RDE_KEYBOARD_KEY_;



/// ================================ MOUSE ==================================

typedef enum {
	// From SDL2
	RDE_MOUSE_BUTTON_0 = 1,
	RDE_MOUSE_BUTTON_1 = 2,
	RDE_MOUSE_BUTTON_2 = 3,
	RDE_MOUSE_BUTTON_3 = 4,
	RDE_MOUSE_BUTTON_4 = 5,
	RDE_MOUSE_BUTTON_5 = 6,
	RDE_MOUSE_BUTTON_6 = 7,
	RDE_MOUSE_BUTTON_7 = 8,

	RDE_MOUSE_BUTTON_LAST = RDE_MOUSE_BUTTON_7,
	RDE_MOUSE_BUTTON_LEFT = RDE_MOUSE_BUTTON_0,
	RDE_MOUSE_BUTTON_RIGHT = RDE_MOUSE_BUTTON_2,
	RDE_MOUSE_BUTTON_MIDDLE = RDE_MOUSE_BUTTON_1,
	RDE_MOUSE_BUTTON_NONE = 0xFFFF
} RDE_MOUSE_BUTTON_;



/// ============================== CONTROLLER ===============================

typedef enum {
	RDE_CONTROLLER_BUTTON_A            = 0,             
	RDE_CONTROLLER_BUTTON_B            = 1,             
	RDE_CONTROLLER_BUTTON_X            = 2,             
	RDE_CONTROLLER_BUTTON_Y            = 3,             
	RDE_CONTROLLER_BUTTON_LB           = 9,  
	RDE_CONTROLLER_BUTTON_RB           = 10,  
	RDE_CONTROLLER_BUTTON_GUIDE        = 5,         
	RDE_CONTROLLER_BUTTON_START        = 6,         
	RDE_CONTROLLER_BUTTON_SELECT       = 4,          
	RDE_CONTROLLER_BUTTON_R_JOY   	   = 8,    
	RDE_CONTROLLER_BUTTON_L_JOY    	   = 7,     
	RDE_CONTROLLER_BUTTON_DPAD_UP      = 11,        
	RDE_CONTROLLER_BUTTON_DPAD_RIGHT   = 14,     
	RDE_CONTROLLER_BUTTON_DPAD_DOWN    = 12,      
	RDE_CONTROLLER_BUTTON_DPAD_LEFT    = 13,      
	RDE_CONTROLLER_BUTTON_NONE		   = 0xFFFF
} RDE_CONTROLLER_BUTTON_;

typedef enum {
	RDE_CONTROLLER_AXIS_LEFT,
	RDE_CONTROLLER_AXIS_RIGHT,
	RDE_CONTROLLER_AXIS_LT,
	RDE_CONTROLLER_AXIS_RT,
	RDE_CONTROLLER_AXIS_NONE
} RDE_CONTROLLER_AXIS_;



/// =============================== PLATFORM ================================

typedef enum {
	RDE_PLATFORM_TYPE_LINUX,
	RDE_PLATFORM_TYPE_WINDOWS,
	RDE_PLATFORM_TYPE_MAC,
	RDE_PLATFORM_TYPE_ANDROID,
	RDE_PLATFORM_TYPE_IOS,
	RDE_PLATFORM_TYPE_EMSCRIPTEN,
	RDE_PLATFORM_TYPE_UNSUPPORTED
} RDE_PLATFORM_TYPE_;

typedef enum {
	RDE_INPUT_TYPE_WINDOW,
	RDE_INPUT_TYPE_MOUSE,
	RDE_INPUT_TYPE_KEYBOARD,
	RDE_INPUT_TYPE_CONTROLLER,
	RDE_INPUT_TYPE_MOBILE
} RDE_INPUT_TYPE_;

typedef enum {
	RDE_BATTERY_UKNOWN,
	RDE_BATTERY_ON_BATTERY,
	RDE_BATTERY_NO_BATTERY,
	RDE_BATTERY_CHARGING,
	RDE_BATTERY_CHARGED
} RDE_BATTERY_;



/// ============================== RENDERING ================================

typedef enum {
	RDE_UI_ANCHOR_MIDDLE              = 1 << 1,
	RDE_UI_ANCHOR_LEFT                = 1 << 2,
	RDE_UI_ANCHOR_RIGHT               = 1 << 3,
	RDE_UI_ANCHOR_TOP                 = 1 << 4,
	RDE_UI_ANCHOR_BOTTOM              = 1 << 5,
	RDE_UI_ANCHOR_LEFT_BOTTOM         = 1 << 6,
	RDE_UI_ANCHOR_LEFT_TOP            = 1 << 7,
	RDE_UI_ANCHOR_RIGHT_BOTTOM        = 1 << 8,
	RDE_UI_ANCHOR_RIGHT_TOP           = 1 << 9
} RDE_UI_ANCHOR_;

typedef enum {
	RDE_UI_STRETCH_NO_STRETCH          = 1 << 10,
	RDE_UI_STRETCH_VERTICAL_STRETCH    = 1 << 11,
	RDE_UI_STRETCH_HORIZONTAL_STRETCH  = 1 << 12,
	RDE_UI_STRETCH_FULL_STRETCH        = 1 << 13
} RDE_UI_STRETCH_;

typedef enum {
	RDE_BATCH_PRIORITY_SPRITE = 0,
	RDE_BATCH_PRIORITY_TEXT = 1,
	RDE_BATCH_PRIORITY_NONE = 2,
} RDE_BATCH_PRIORITY_;

typedef enum {
	RDE_RENDERIZABLE_TYPE_NONE        = 0,
	RDE_RENDERIZABLE_TYPE_SPRITE      = 1,
	RDE_RENDERIZABLE_TYPE_TEXT        = 2,
	RDE_RENDERIZABLE_TYPE_UI_IMAGE    = 3,
	RDE_RENDERIZABLE_TYPE_UI_TEXT     = 4
} RDE_RENDERIZABLE_TYPE_;

typedef enum {
	RDE_UNIFORM_FV_1,
	RDE_UNIFORM_FV_2,
	RDE_UNIFORM_FV_3,
	RDE_UNIFORM_FV_4,
	RDE_UNIFORM_FV_MATRIX_2,
	RDE_UNIFORM_FV_MATRIX_3,
	RDE_UNIFORM_FV_MATRIX_4,
#ifndef __EMSCRIPTEN__
	RDE_UNIFORM_FV_MATRIX_2x3,
	RDE_UNIFORM_FV_MATRIX_3x2,
	RDE_UNIFORM_FV_MATRIX_4x2,
	RDE_UNIFORM_FV_MATRIX_2x4,
	RDE_UNIFORM_FV_MATRIX_4x3,
	RDE_UNIFORM_FV_MATRIX_3x4
#endif
} RDE_UNIFORM_FV_;

typedef enum {
	RDE_UNIFORM_IV_1,
	RDE_UNIFORM_IV_2,
	RDE_UNIFORM_IV_3,
	RDE_UNIFORM_IV_4
} RDE_UNIFORM_IV_;

typedef enum {
	RDE_UNIFORM_UIV_1,
	RDE_UNIFORM_UIV_2,
	RDE_UNIFORM_UIV_3,
	RDE_UNIFORM_UIV_4
} RDE_UNIFORM_UIV_;

typedef enum {
	RDE_CAMERA_TYPE_PERSPECTIVE,
	RDE_CAMERA_TYPE_ORTHOGRAPHIC
} RDE_CAMERA_TYPE_;

/// ============================= FILE SYSTEM ===============================

typedef enum {
	RDE_IMAGE_EXTENSION_PNG,
	RDE_IMAGE_EXTENSION_JPG,
	RDE_IMAGE_EXTENSION_BMP,
	RDE_IMAGE_EXTENSION_SVG
} RDE_IMAGE_EXTENSION_;

typedef enum {
	RDE_FILE_RESULT_OK,
	RDE_FILE_RESULT_NOT_FOUND,
	RDE_FILE_RESULT_LINE_OUT_OF_SCOPE,
	RDE_FILE_RESULT_NOT_ALL_LINES_IN_SCOPE
} RDE_FILE_RESULT_;

typedef enum {
	RDE_FILE_MODE_READ,
	RDE_FILE_MODE_WRITE,
	RDE_FILE_MODE_APPEND,
	RDE_FILE_MODE_READ_AND_WRITE,
	RDE_FILE_MODE_READ_AND_APPEND,
} RDE_FILE_MODE_;



/// =============================== PHYSICS =================================

typedef enum {
	RDE_PHYSICS_BODY_TYPE_STATIC,
	RDE_PHYSICS_BODY_TYPE_KINEMATIC,
	RDE_PHYSICS_BODY_TYPE_DYNAMIC
} RDE_PHYSICS_BODY_TYPE_;

typedef enum {
	RDE_PHYSICS_SHAPE_TYPE_BOX,
	RDE_PHYSICS_SHAPE_TYPE_CIRCLE,
	RDE_PHYSICS_SHAPE_TYPE_POLYGON,
	RDE_PHYSICS_SHAPE_TYPE_SEGMENT
} RDE_PHYSICS_SHAPE_TYPE_;



/// =============================== LOCALIZATION ============================

typedef enum { 
	RDE_LANGUAGE_EN_US, 
	RDE_LANGUAGE_EN_GB, 
	RDE_LANGUAGE_EN_CA, 
	RDE_LANGUAGE_ES_MX, 
	RDE_LANGUAGE_ES_ES, 
	RDE_LANGUAGE_PT_BR, 
	RDE_LANGUAGE_FR, 
	RDE_LANGUAGE_ZH, 
	RDE_LANGUAGE_RU, 
	RDE_LANGUAGE_DE, 
	RDE_LANGUAGE_IT, 
	RDE_LANGUAGE_JP, 
	RDE_LANGUAGE_MAX 
} RDE_LANGUAGE_;


/// =============================== LOG ============================

typedef enum { 
	RDE_LOG_COLOR_RED, 
	RDE_LOG_COLOR_GREEN, 
	RDE_LOG_COLOR_YELLOW, 
	RDE_LOG_COLOR_BLUE, 
	RDE_LOG_COLOR_PURPLE, 
	RDE_LOG_COLOR_CYAN, 
	RDE_LOG_COLOR_WHITE
} RDE_LOG_COLOR_;

typedef enum { 
	RDE_LOG_LEVEL_DEBUG,
	RDE_LOG_LEVEL_INFO,
	RDE_LOG_LEVEL_WARNING,
	RDE_LOG_LEVEL_ERROR
} RDE_LOG_LEVEL_;

/// *************************************************************************************************
/// *                                		STRUCSTS                         						*
/// *************************************************************************************************


/// ================================= MATH ==================================

SPECIALIZED_VEC2(int, 			rde_vec_2I);
SPECIALIZED_VEC2(unsigned int, 	rde_vec_2UI);
SPECIALIZED_VEC2(float, 		rde_vec_2F);
SPECIALIZED_VEC2(double,		rde_vec_2D);
SPECIALIZED_VEC2(long,			rde_vec_2L);
SPECIALIZED_VEC2(unsigned long, rde_vec_2UL);
SPECIALIZED_VEC2(size_t, 		rde_vec_2ST);

SPECIALIZED_VEC3(int, 			rde_vec_3I);
SPECIALIZED_VEC3(unsigned int, 	rde_vec_3UI);
SPECIALIZED_VEC3(float, 		rde_vec_3F);
SPECIALIZED_VEC3(double,		rde_vec_3D);
SPECIALIZED_VEC3(long,			rde_vec_3L);
SPECIALIZED_VEC3(unsigned long, rde_vec_3UL);
SPECIALIZED_VEC3(size_t, 		rde_vec_3ST);

SPECIALIZED_VEC4(int, 			rde_vec_4I);
SPECIALIZED_VEC4(unsigned int, 	rde_vec_4UI);
SPECIALIZED_VEC4(float, 		rde_vec_4F);
SPECIALIZED_VEC4(double,		rde_vec_4D);
SPECIALIZED_VEC4(long,			rde_vec_4L);
SPECIALIZED_VEC4(unsigned long, rde_vec_4UL);
SPECIALIZED_VEC4(size_t, 		rde_vec_4ST);

SPECIALIZED_MAT2(int, 			rde_mat_2I);
SPECIALIZED_MAT2(unsigned int, 	rde_mat_2UI);
SPECIALIZED_MAT2(float, 		rde_mat_2F);
SPECIALIZED_MAT2(double,		rde_mat_2D);
SPECIALIZED_MAT2(long,			rde_mat_2L);
SPECIALIZED_MAT2(unsigned long, rde_mat_2UL);
SPECIALIZED_MAT2(size_t, 		rde_mat_2ST);

SPECIALIZED_MAT3(int, 			rde_mat_3I);
SPECIALIZED_MAT3(unsigned int, 	rde_mat_3UI);
SPECIALIZED_MAT3(float, 		rde_mat_3F);
SPECIALIZED_MAT3(double,		rde_mat_3D);
SPECIALIZED_MAT3(long,			rde_mat_3L);
SPECIALIZED_MAT3(unsigned long, rde_mat_3UL);
SPECIALIZED_MAT3(size_t, 		rde_mat_3ST);

SPECIALIZED_MAT4(int, 			rde_mat_4I);
SPECIALIZED_MAT4(unsigned int, 	rde_mat_4UI);
SPECIALIZED_MAT4(float, 		rde_mat_4F);
SPECIALIZED_MAT4(double,		rde_mat_4D);
SPECIALIZED_MAT4(long,			rde_mat_4L);
SPECIALIZED_MAT4(unsigned long, rde_mat_4UL);
SPECIALIZED_MAT4(size_t, 		rde_mat_4ST);

struct rde_line {
	UNIMPLEMENTED_STRUCT()
};

struct rde_triangle {
	UNIMPLEMENTED_STRUCT()
};

struct rde_rectangle {
	UNIMPLEMENTED_STRUCT()
};

struct rde_bouding_box {
	UNIMPLEMENTED_STRUCT()
};

typedef struct {
	float probability_rolled;
	bool happened;
} rde_probability;
rde_probability rde_struct_create_probability() {
	rde_probability _p;
	_p.probability_rolled = 0.f;
	_p.happened = false;
	return _p;
}

/// ================== CALLBACKS AND FUNCTION POINTERS ======================

typedef struct rde_engine rde_engine;
typedef struct rde_window rde_window;
typedef struct rde_event rde_event;

typedef void (*rde_event_func_inner)(rde_event*);
typedef void (*rde_event_func_outer)(rde_window*, rde_event*);
typedef void (*rde_engine_user_side_loop_func)(float);
typedef void (*rde_engine_user_side_loop_func_2)(float, rde_window*);

typedef struct rde_inner_window_data rde_inner_window_info;

typedef struct {
	bool block_engine_default_implementation;
	rde_event_func_inner _callback;
} rde_event_callback;
rde_event_callback rde_struct_create_event_callback() {
	rde_event_callback _c;
	_c.block_engine_default_implementation = false;
	_c._callback = NULL;
	return _c;
}

typedef struct {
	rde_event_callback on_window_resize;
	rde_event_callback on_window_focused_by_mouse;
	rde_event_callback on_window_unfocused_by_mouse;
	rde_event_callback on_window_focused_by_keyboard;
	rde_event_callback on_window_unfocused_by_keyboard;
	rde_event_callback on_window_moved;
	rde_event_callback on_window_minimized;
	rde_event_callback on_window_maximized;
	rde_event_callback on_window_closed;
	rde_event_callback on_window_display_changed;
} rde_window_callbacks;
rde_window_callbacks rde_struct_create_window_callbacks() {
	rde_window_callbacks _w;
	_w.on_window_resize = rde_struct_create_event_callback();
	_w.on_window_focused_by_mouse = rde_struct_create_event_callback();
	_w.on_window_unfocused_by_mouse = rde_struct_create_event_callback();
	_w.on_window_focused_by_keyboard = rde_struct_create_event_callback();
	_w.on_window_unfocused_by_keyboard = rde_struct_create_event_callback();
	_w.on_window_moved = rde_struct_create_event_callback();
	_w.on_window_minimized = rde_struct_create_event_callback();
	_w.on_window_maximized = rde_struct_create_event_callback();
	_w.on_window_closed = rde_struct_create_event_callback();
	_w.on_window_display_changed = rde_struct_create_event_callback();
	return _w;
}

typedef struct {
	rde_event_callback on_display_connected;
	rde_event_callback on_display_disconnected;
	rde_event_callback on_display_changed_orientation;
} rde_display_callbacks;
rde_display_callbacks rde_struct_create_display_callbacks() {
	rde_display_callbacks _d;
	_d.on_display_connected = rde_struct_create_event_callback();
	_d.on_display_disconnected = rde_struct_create_event_callback();
	_d.on_display_changed_orientation = rde_struct_create_event_callback();
	return _d;
}

typedef struct {
	rde_event_callback on_app_terminating; 				/// Android -> onDestroy(), iOS -> applicationWillTerminate()
	rde_event_callback on_app_low_memory; 				/// Android -> onLowMemory(), iOS -> applicationDidReceiveMemoryWarning()
	rde_event_callback on_app_will_enter_background; 	/// Android -> onPause(), iOS -> applicationWillResignActive()
	rde_event_callback on_app_did_enter_background; 	/// Android -> onPause(), iOS -> applicationDidEnterBackground()
	rde_event_callback on_app_will_enter_foreground; 	/// Android -> onResume(), iOS -> applicationWillEnterForeground()
	rde_event_callback on_app_did_enter_foreground; 	/// Android -> onResume(), iOS -> applicationDidBecomeActive()
} rde_mobile_callbacks;
rde_mobile_callbacks rde_struct_create_mobile_callbacks() {
	rde_mobile_callbacks _m;
	_m.on_app_terminating = rde_struct_create_event_callback();
	_m.on_app_low_memory = rde_struct_create_event_callback();
	_m.on_app_will_enter_background = rde_struct_create_event_callback();
	_m.on_app_did_enter_background = rde_struct_create_event_callback();
	_m.on_app_will_enter_foreground = rde_struct_create_event_callback();
	_m.on_app_did_enter_foreground = rde_struct_create_event_callback();
	return _m;
}

typedef struct {
	rde_event_callback on_controller_added;
	rde_event_callback on_controller_removed;
	rde_event_callback on_controller_reassigned;
} rde_controller_callbacks;
rde_controller_callbacks rde_struct_create_controller_callbacks() {
	rde_controller_callbacks _c;
	_c.on_controller_added = rde_struct_create_event_callback();
	_c.on_controller_removed = rde_struct_create_event_callback();
	_c.on_controller_reassigned = rde_struct_create_event_callback();
	return _c;
}

typedef struct {
	rde_event_callback on_drag_and_drop_begin;
	rde_event_callback on_drag_and_drop_end;
} rde_drag_and_drop_callbacks;
rde_drag_and_drop_callbacks rde_struct_create_drag_and_drop_callbacks() {
	rde_drag_and_drop_callbacks _d;
	_d.on_drag_and_drop_begin = rde_struct_create_event_callback();
	_d.on_drag_and_drop_end = rde_struct_create_event_callback();
	return _d;
}

typedef struct {
	rde_engine_user_side_loop_func on_update;
	rde_engine_user_side_loop_func on_fixed_update;
	rde_engine_user_side_loop_func on_late_update;
	rde_engine_user_side_loop_func_2 on_render;
} rde_end_user_mandatory_callbacks;
rde_end_user_mandatory_callbacks rde_struct_create_end_user_mandatory_callbacks() {
	rde_end_user_mandatory_callbacks _e;
	_e.on_update = NULL;
	_e.on_fixed_update = NULL;
	_e.on_late_update = NULL;
	_e.on_render = NULL;
	return _e;
}

/// ============================== ENGINE ===================================

typedef struct {
	int index;
} rde_display_info;
rde_display_info rde_struct_create_display_info() {
	rde_display_info _d;
	_d.index = -1;
	return _d;
}

typedef struct {
	size_t max_number_of_windows;
	size_t max_number_of_vertices_per_batch;
	size_t max_number_of_shaders;
	
#ifdef RDE_RENDERING_2D_MODULE
	size_t max_number_of_textures;
	size_t max_number_of_atlases;
	size_t max_number_of_fonts;
#endif

#ifdef RDE_RENDERING_3D_MODULE
	size_t max_number_of_models;
	size_t max_number_of_models_textures;
#endif

#ifdef RDE_AUDIO_MODULE
	size_t max_number_of_sounds;
#endif

} rde_engine_heap_allocs_config;

/// ============================== EVENTS ===================================

typedef struct {
	rde_vec_2I position;
	rde_vec_2I size;
	int display_index;
	bool minimized;
	bool maximized;
} rde_event_window;
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

typedef struct {
	int orientation;
	int display_index;
} rde_event_display;
rde_event_display rde_struct_create_event_display() {
	rde_event_display _e;
	_e.orientation = -1;
	_e.display_index = -1;
	return _e;
}

typedef struct {
	RDE_KEYBOARD_KEY_ key;
	char typed_char;
	const char* typed_text;
	int amount_of_times_pressed;
} rde_event_key;
rde_event_key rde_struct_create_event_key() {
	rde_event_key _e;
	_e.key = RDE_KEYBOARD_KEY_NONE;
	_e.typed_char = '\0';
	_e.amount_of_times_pressed = -1;
	return _e;
}

typedef struct {
	rde_vec_2I position;
	rde_vec_2F scrolled;
	RDE_MOUSE_BUTTON_ button;
	int amount_of_times_pressed;
} rde_event_mouse;
rde_event_mouse rde_struct_create_event_mouse() {
	rde_event_mouse _e;
	_e.button = RDE_MOUSE_BUTTON_NONE;
	_e.position.x = -1;
	_e.position.y = -1;
	_e.scrolled.x = -1.f;
	_e.scrolled.y = -1.f;
	return _e;
}

typedef struct {
	size_t controller_id;
	rde_vec_2F left_joystick;
	rde_vec_2F right_joystick;
	rde_vec_2F back_buttons;
	RDE_CONTROLLER_BUTTON_ button;
	RDE_CONTROLLER_AXIS_ axis;
} rde_event_controller;
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

typedef struct {
	rde_vec_2I init_touch_position;
	rde_vec_2I end_touch_position;
	float pressure;
	int finger_id;
} rde_event_mobile;
rde_event_mobile rde_struct_create_event_mobile() {
	rde_event_mobile _e;
	_e.init_touch_position.x = -1;
	_e.init_touch_position.y = -1;
	_e.end_touch_position.x = -1;
	_e.end_touch_position.y = -1;
	_e.pressure = -1.f;
	_e.finger_id = -1;
	return _e;
}

typedef struct {
	size_t window_id;
	char* file_path;
} rde_event_drag_and_drop;
rde_event_drag_and_drop rde_struct_create_event_drag_and_drop() {
	rde_event_drag_and_drop _e;
	_e.window_id = 0;
	_e.file_path = NULL;
	return _e;
}

typedef struct {
	rde_event_window window_event_data;
	rde_event_key key_event_data;
	rde_event_mouse mouse_event_data;
	rde_event_controller controller_event_data;
	rde_event_mobile mobile_event_data;
	rde_event_display display_event_data;
} rde_event_data;
rde_event_data rde_struct_create_event_data() {
	rde_event_data _e;
	_e.window_event_data = rde_struct_create_event_window();
	_e.key_event_data = rde_struct_create_event_key();
	_e.mouse_event_data = rde_struct_create_event_mouse();
	_e.controller_event_data = rde_struct_create_event_controller();
	_e.mobile_event_data = rde_struct_create_event_mobile();
	_e.display_event_data = rde_struct_create_event_display();
	return _e;
}

struct rde_event {
	RDE_EVENT_TYPE_ type;
	ulong time_stamp;
	uint window_id;
	bool handled;
	rde_event_data data;
};
rde_event rde_struct_create_event() {
	rde_event _e;
	_e.type = RDE_EVENT_TYPE_NONE;
	_e.time_stamp = 0;
	_e.window_id = 0;
	_e.handled = false;
	_e.data = rde_struct_create_event_data();
	return _e;
}

typedef struct rde_transform rde_transform;
struct rde_transform {
	rde_vec_3F position;
	rde_vec_3F rotation;
	rde_vec_3F scale;
	rde_transform* parent;
};
rde_transform rde_struct_create_transform() {
	rde_transform _t;
	_t.position.x = 0.f;
	_t.position.y = 0.f;
	_t.position.z = 0.f;
	_t.rotation.x = 0.f;
	_t.rotation.y = 0.f;
	_t.rotation.z = 0.f;
	_t.scale.x = 1.f;
	_t.scale.y = 1.f;
	_t.scale.z = 1.f;
	_t.parent = NULL;
	return _t;
}

/// ============================ RENDERING ==================================

typedef struct rde_shader rde_shader;
typedef struct rde_render_texture rde_render_texture;

#if defined(RDE_RENDERING_2D_MODULE) || defined(RDE_RENDERING_3D_MODULE)
typedef struct rde_texture rde_texture;
#endif

#if defined(RDE_RENDERING_2D_MODULE)
typedef struct rde_atlas rde_atlas;
typedef struct rde_font rde_font;

typedef struct {
	rde_vec_3F position;
	int color;
	rde_vec_2F texture_coordinates;
} rde_vertex_2d;
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

#endif 

#if defined(RDE_RENDERING_3D_MODULE)
typedef struct rde_mesh rde_mesh;
typedef struct rde_model rde_model;

struct rde_material_map {
	UNIMPLEMENTED_STRUCT()
};

struct rde_material {
	UNIMPLEMENTED_STRUCT()
};

struct rde_model_bone {
	UNIMPLEMENTED_STRUCT()
};

struct rde_model_animation {
	UNIMPLEMENTED_STRUCT()
};

#endif

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} rde_color;
rde_color rde_struct_create_color() {
	rde_color _c;
	_c.r = 255;
	_c.g = 255;
	_c.b = 255;
	_c.a = 255;
	return _c;
}

typedef struct {
	rde_vec_2I* vertices;
	size_t vertices_count;
} rde_polygon;
rde_polygon rde_struct_create_polygon() {
	rde_polygon _p;
	_p.vertices = NULL;
	_p.vertices_count = 0;
	return _p;
}

struct rde_viewport {
	UNIMPLEMENTED_STRUCT()
};

typedef struct {
	size_t id;
	float zoom;
	rde_transform transform;
	rde_vec_3F direction;
	rde_vec_3F up;
	rde_vec_2F near_far;
	RDE_CAMERA_TYPE_ camera_type;
	bool enabled;
} rde_camera;
rde_camera rde_struct_create_camera() {
	rde_camera _c;
	_c.id = 0;
	_c.zoom = 1.f;
	_c.transform = rde_struct_create_transform();
	_c.camera_type = RDE_CAMERA_TYPE_ORTHOGRAPHIC;
	_c.enabled = true;
	_c.direction = (rde_vec_3F) { 0.0f, 0.0f, -1.0f };
	_c.up = (rde_vec_3F) { 0.0f, 1.0f, 0.0f };
	_c.near_far = (rde_vec_2F) { 0.1f, 100.f };
	return _c;
}

/// ============================ AUDIO ==================================

#ifdef RDE_AUDIO_MODULE
typedef struct rde_sound rde_sound;
typedef struct {
	void* user_data;
	unsigned short channels;
	unsigned int rate;
} rde_sound_config;
rde_sound_config rde_struct_create_audio_config() {
	rde_sound_config _s;
	_s.user_data = NULL;
	_s.channels = 2;
	_s.rate = 48000;
	return _s;
}
#endif

/// *************************************************************************************************
/// *                                GLOBAL VARIABLES                         						*
/// *************************************************************************************************

const rde_color RDE_COLOR_WHITE 			= { 255, 255, 255, 255 };
const rde_color RDE_COLOR_BLACK 			= {   0,   0,   0, 255 };
const rde_color RDE_COLOR_BLUE 				= {   0,   0, 255, 255 };
const rde_color RDE_COLOR_RED 				= { 255,   0,   0, 255 };
const rde_color RDE_COLOR_YELLOW			= { 255, 255,   0, 255 };
const rde_color RDE_COLOR_MAGENTA			= { 255,   0, 255, 255 };
const rde_color RDE_COLOR_ORANGE 			= { 255,  69,   0, 255 };
const rde_color RDE_COLOR_GREEN 			= {   0, 255,   0, 255 };
const rde_color RDE_COLOR_BROWN 			= { 139,  69,  19, 255 };
const rde_color RDE_COLOR_PURPLE 			= { 128,   0, 128, 255 };
const rde_color RDE_COLOR_GRAY				= { 128, 128, 128, 255 };
const rde_color RDE_COLOR_TRANSPARENT		= {   0,   0,   0,   0 };
const rde_color RDE_COLOR_DISABLED_GRAY		= { 220, 220, 220, 255 };
const rde_color RDE_COLOR_PLACEHOLDER_TEXT	= { 220, 220, 220, 128 };
const rde_color RDE_COLOR_RDE_DUCK_YELLOW	= { 255, 213,  81, 255 };
const rde_color RDE_COLOR_GOLD				= { 255, 213,  81, 255 };
const rde_color RDE_COLOR_PINK				= { 255, 109, 194, 255 };

const rde_engine_heap_allocs_config RDE_DEFAULT_HEAP_ALLOCS_CONFIG = {
	RDE_MAX_NUMBER_OF_WINDOWS,
	RDE_MAX_VERTICES_PER_BATCH,
	RDE_MAX_LOADABLE_SHADERS,

#ifdef RDE_RENDERING_2D_MODULE
	RDE_MAX_LOADABLE_TEXTURES,
	RDE_MAX_LOADABLE_ATLASES,
	RDE_MAX_LOADABLE_FONTS,
#endif

#ifdef RDE_RENDERING_3D_MODULE
	RDE_MAX_LOADABLE_MODELS,
	RDE_MAX_LOADABLE_MODELS_TEXTURES,
#endif

#ifdef RDE_AUDIO_MODULE
	RDE_MAX_LOADABLE_SOUNDS
#endif
};


/// *************************************************************************************************
/// *                                		FUNCTIONS                         						*
/// *************************************************************************************************

/// ============================ LOG ========================================

RDE_FUNC void rde_log_color(RDE_LOG_COLOR_ _color, const char* _fmt, ...);
RDE_FUNC void rde_log_level(RDE_LOG_LEVEL_ _level, const char* _fmt, ...);

/// ============================ UTIL =======================================

RDE_FUNC const char* rde_util_get_file_name_extension(const char* _file_name);
RDE_FUNC size_t rde_util_get_string_width(const char* _string, const rde_font* _font);
RDE_FUNC rde_vec_2I rde_util_get_string_size(const char* _string, const rde_font* _font);
RDE_FUNC char* rde_util_string_trim(char* _s);

/// ============================ MATH =======================================

RDE_FUNC void rde_math_set_random_seed(long _seed); /// Sets the seed for the random math module, -1 will not set it to the current time
RDE_FUNC int rde_math_get_random_int(int _min_included, int _max_included);
RDE_FUNC float rde_math_get_random_float(float _min_included, float _max_included);
RDE_FUNC rde_probability rde_math_get_probability(float _normalized_chance_to_happen);

RDE_FUNC void rde_math_convert_world_position_to_screen_coordinates_3d(rde_window* _window, rde_vec_3F* _vec);
RDE_FUNC void rde_math_convert_world_position_to_screen_coordinates_2d(rde_window* _window, rde_vec_2F* _vec);
RDE_FUNC void rde_math_convert_world_size_to_screen_size(rde_window* _window, rde_vec_2F* _vec);

RDE_FUNC rde_vec_3F rde_math_cross_product(rde_vec_3F _vec_0, rde_vec_3F _vec_1);
RDE_FUNC void rde_math_normalize(rde_vec_3F* _vec);

#define RDE_DECLARE_CLAMP_FUNCS(_type) \
	RDE_FUNC inline _type rde_math_clamp_##_type(_type _value, _type _min, _type _max);

RDE_DECLARE_CLAMP_FUNCS(int)
RDE_DECLARE_CLAMP_FUNCS(uint)
RDE_DECLARE_CLAMP_FUNCS(float)
RDE_DECLARE_CLAMP_FUNCS(double)
RDE_DECLARE_CLAMP_FUNCS(long)
RDE_DECLARE_CLAMP_FUNCS(ulong)

RDE_FUNC inline float rde_math_radians_to_degrees(float _radians);
RDE_FUNC inline float rde_math_degrees_to_radians(float _degrees);

#define RDE_DECLARE_EASING_FUNCS(_form, _type) \
	RDE_FUNC inline float rde_math_easing_##_form##_##_type(float _current_time, float _start_value, float _change_in_value, float _duration);

RDE_DECLARE_EASING_FUNCS(in, linear)
RDE_DECLARE_EASING_FUNCS(in, quadratic)
RDE_DECLARE_EASING_FUNCS(out, quadratic)
RDE_DECLARE_EASING_FUNCS(in_out, quadratic)
RDE_DECLARE_EASING_FUNCS(in, cubic)
RDE_DECLARE_EASING_FUNCS(out, cubic)
RDE_DECLARE_EASING_FUNCS(in_out, cubic)
RDE_DECLARE_EASING_FUNCS(in, quartic)
RDE_DECLARE_EASING_FUNCS(out, quartic)
RDE_DECLARE_EASING_FUNCS(in_out, quartic)
RDE_DECLARE_EASING_FUNCS(in, quintic)
RDE_DECLARE_EASING_FUNCS(out, quintic)
RDE_DECLARE_EASING_FUNCS(in_out, quintic)
RDE_DECLARE_EASING_FUNCS(in, sine)
RDE_DECLARE_EASING_FUNCS(out, sine)
RDE_DECLARE_EASING_FUNCS(in_out, sine)
RDE_DECLARE_EASING_FUNCS(in, exponential)
RDE_DECLARE_EASING_FUNCS(out, exponential)
RDE_DECLARE_EASING_FUNCS(in_out, exponential)
RDE_DECLARE_EASING_FUNCS(in, circular)
RDE_DECLARE_EASING_FUNCS(out, circular)
RDE_DECLARE_EASING_FUNCS(in_out, circular)

/// ============================ ENGINE =====================================


RDE_FUNC rde_window* rde_engine_create_engine(int _argc, char** _argv, rde_engine_heap_allocs_config _heap_allocs_config);
RDE_FUNC void rde_setup_initial_info(const rde_end_user_mandatory_callbacks _end_user_callbacks); /// Setsup some basic info the engine needs, call this BEFORE rde_engine_create_engine

RDE_FUNC RDE_PLATFORM_TYPE_ rde_engine_get_platform();

RDE_FUNC float rde_engine_get_fixed_delta();
RDE_FUNC void rde_engine_set_fixed_delta(float _fixed_dt);

RDE_FUNC void rde_engine_on_run();

RDE_FUNC bool rde_engine_is_running();
RDE_FUNC void rde_engine_set_running(bool _running);

RDE_FUNC rde_vec_2I rde_engine_get_display_size();

RDE_FUNC rde_display_info* rde_engine_get_available_displays();
RDE_FUNC void rde_engine_switch_window_display(rde_window* _window, size_t _new_display);

RDE_FUNC rde_window* rde_engine_get_focused_window();

RDE_FUNC void rde_engine_use_rde_2d_physics_system(bool _use);

RDE_FUNC bool rde_engine_is_vsync_active();
RDE_FUNC void rde_engine_set_vsync_active(bool _vsync);

RDE_FUNC void rde_engine_destroy_engine();


/// ============================ WINDOW =====================================

RDE_FUNC rde_window* rde_window_create_window();
RDE_FUNC void rde_window_set_callbacks(rde_window* _window, rde_window_callbacks _callbacks);

RDE_FUNC rde_vec_2I	rde_window_get_window_size(rde_window* _window);
RDE_FUNC void rde_window_set_window_size(rde_window* _window, rde_vec_2I _size);

RDE_FUNC rde_vec_2I	rde_window_get_position(rde_window* _window);
RDE_FUNC void rde_window_set_position(rde_window* _window, rde_vec_2I _position);

RDE_FUNC const char* rde_window_get_title(rde_window* _window);
RDE_FUNC void rde_window_set_title(rde_window* _window, const char* _title);

RDE_FUNC bool rde_window_orientation_is_horizontal(rde_window* _window);

RDE_FUNC void rde_window_take_screen_shot(rde_window* _window, rde_vec_2I _position, rde_vec_2I _size_of_rectangle, const char* _file_name_with_extension);
RDE_FUNC unsigned char* getAreaOfScreenPixels(rde_window* _window, rde_vec_2I _position, rde_vec_2I _size); // returns a dynamic array of the pixels in a RGBA format (so 4 elements per pixel). User MUST free the returned array.

RDE_FUNC float rde_window_get_aspect_ratio(rde_window* _window);

RDE_FUNC bool rde_window_is_mouse_out_of_window_allowed();
RDE_FUNC void rde_window_allow_mouse_out_of_window(bool _allow_mouse_out_of_window);

RDE_FUNC void rde_window_set_icon(rde_window* _window, const char* _path_to_icon);

RDE_FUNC void* rde_window_get_native_sdl_window_handle(rde_window* _window);
RDE_FUNC void* rde_window_get_native_sdl_gl_context_handle(rde_window* _window);

RDE_FUNC void rde_window_destroy_window(rde_window* _window);

/// ============================ EVENTS =====================================

RDE_FUNC void rde_events_window_consume_events(rde_window* _window, rde_event* _event);
RDE_FUNC void rde_events_display_consume_events(rde_window* _window, rde_event* _event);
RDE_FUNC void rde_events_keyboard_consume_events(rde_window* _window, rde_event* _event);
RDE_FUNC void rde_events_mouse_consume_events(rde_window* _window, rde_event* _event);

RDE_FUNC bool rde_events_is_key_just_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key);
RDE_FUNC bool rde_events_is_key_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key);
RDE_FUNC bool rde_events_is_key_just_released(rde_window* _window, RDE_KEYBOARD_KEY_ _key);

RDE_FUNC bool rde_events_is_mouse_button_just_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button);
RDE_FUNC bool rde_events_is_mouse_button_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button);
RDE_FUNC bool rde_events_is_mouse_button_just_released(rde_window* _window, RDE_MOUSE_BUTTON_ _button);
RDE_FUNC rde_vec_2F rde_events_mouse_get_scrolled(rde_window* _window);
RDE_FUNC rde_vec_2I rde_events_mouse_get_position(rde_window* _window);

#if IS_MOBILE()
RDE_FUNC int rde_events_mobile_consume_events(void* _user_data, SDL_Event* _event);
#endif

/// ============================ RENDERING ==================================

RDE_FUNC rde_shader* rde_rendering_load_shader(const char* _vertex_code, const char* _fragment_code);
RDE_FUNC void rde_rendering_set_shader_uniform_value_float(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_FV_ _type, float* _data, bool _transpose);
RDE_FUNC void rde_rendering_set_shader_uniform_value_int(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_IV_ _type, int* _data);
RDE_FUNC void rde_rendering_set_shader_uniform_value_uint(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_UIV_ _type, uint* _data);
RDE_FUNC void rde_rendering_unload_shader(rde_shader* _shader);

RDE_FUNC rde_texture* rde_rendering_load_texture(const char* _file_path);
RDE_FUNC rde_texture* rde_rendering_load_text_texture(const char* _file_path);
RDE_FUNC void rde_rendering_unload_texture(rde_texture* _texture);

#ifdef RDE_RENDERING_2D_MODULE
RDE_FUNC rde_atlas* rde_rendering_load_atlas(const char* _texture_path, const char* _config_path);
RDE_FUNC rde_texture* rde_rendering_get_atlas_sub_texture(rde_atlas* _atlas, const char* _texture_name);
RDE_FUNC void rde_rendering_unload_atlas(rde_atlas* _atlas);
#endif

RDE_FUNC rde_texture* rde_rendering_create_memory_texture(size_t _width, size_t _height, int _channels);
RDE_FUNC void rde_rendering_memory_texture_set_pixel(rde_texture* _memory_texture, rde_vec_2I _position, rde_color _color);
RDE_FUNC rde_color rde_rendering_memory_texture_get_pixel(rde_texture* _memory_texture, rde_vec_2I _position);
RDE_FUNC void rde_rendering_memory_texture_gpu_upload(rde_texture* _memory_texture);
RDE_FUNC unsigned char* rde_rendering_memory_texture_get_pixels(rde_texture* _memory_texture);
RDE_FUNC void rde_rendering_destroy_memory_texture(rde_texture* _memory_texture);

#if defined(RDE_RENDERING_2D_MODULE) || defined(RDE_RENDERING_3D_MODULE)
RDE_FUNC rde_font* rde_rendering_load_font(const char* _font_path, const char* _font_config_path);
RDE_FUNC void rde_rendering_unload_font(rde_font* _font);
#endif

RDE_FUNC void rde_rendering_set_background_color(const rde_color _color);

#ifdef RDE_RENDERING_2D_MODULE
RDE_FUNC void rde_rendering_begin_drawing_2d(rde_camera* _camera, rde_window* _window);
RDE_FUNC void rde_rendering_draw_point_2d(rde_vec_2F _position, rde_color _color, rde_shader* _shader); /// Draws a point in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_line_2d(rde_vec_2F _init, rde_vec_2F _end, rde_color _color, rde_shader* _shader); /// Draws a batched line in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_triangle_2d(rde_vec_2F _verte_a, rde_vec_2F _vertex_b, rde_vec_2F _vertex_c, rde_color _color, rde_shader* _shader); /// Draws a batched triangle in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_rectangle_2d(rde_vec_2F _bottom_left, rde_vec_2F _top_right, rde_color _color, rde_shader* _shader); /// Draws a batched rectangle in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_circle_2d(rde_vec_2F _position, float _radius, rde_color _color, rde_shader* _shader); /// Draws a batched circle in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_polygon_2d(const rde_transform* _transform, const rde_polygon* _polygon, rde_color _color, const rde_shader* _shader);  /// Draws a batched polygon in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_texture_2d(const rde_transform* _transform, const rde_texture* _texture, rde_color _tint_color, rde_shader* _shader); /// Draws a batched quad texture in 2D space, pass RDE_COLOR_WHITE to _tint_color for no tint effects, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_memory_texture_2d(const rde_transform* _transform, rde_texture* _texture, rde_color _tint_color, rde_shader* _shader); /// Draws a batched quad texture in 2D space, pass RDE_COLOR_WHITE to _tint_color for no tint effects, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_text_2d(const rde_transform* _transform, const rde_font* _font, const char* _text, rde_color _tint_color, rde_shader* _shader); /// Draws a batched group of quads representing the _text in 2D space, pass RDE_COLOR_WHITE to _tint_color for no tint effects, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_end_drawing_2d();
#endif

#ifdef RDE_RENDERING_3D_MODULE
RDE_FUNC rde_mesh* rde_struct_create_memory_mesh(size_t _vertex_count, size_t _indices_count); // creates a new mesh that when not needed anymore, needs to be destroyed. A quad mesh will have 4 vertices and 6 indices and uploads to GPU
RDE_FUNC void rde_rendering_mesh_set_vertex_positions(rde_mesh* _mesh, float* _positions, bool _free_positions_on_destroy); // sets the position of the vertices, each position must have 3 floats (x, y, z) and uploads to GPU
RDE_FUNC void rde_rendering_mesh_set_indices(rde_mesh* _mesh, unsigned int* _indices, bool _free_indices_on_destroy); // sets the indices of the mesh, a quad should have 6 indices and uploads to GPU
RDE_FUNC void rde_rendering_mesh_set_vertex_colors(rde_mesh* _mesh, unsigned int* _colors, bool _free_colors_on_destroy); // sets the colors of the vertices, 1 usigned int for each vertex (0xFF0000FF is red, for example) and uploads to GPU
RDE_FUNC void rde_rendering_mesh_set_vertex_normals(rde_mesh* _mesh, float* _normals, bool _free_normals_on_destroy); // sets the normals of the vertices, each position must have 3 floats (x, y, z) and uploads to GPU
RDE_FUNC void rde_rendering_mesh_set_vertex_texture_data(rde_mesh* _mesh, unsigned int _texture_coords_size, float* _texture_coords, rde_texture* _texture, bool _free_texture_coords_on_destroy); // sets the data to draw a mesh with a texture. each text_coord has 2 floats (x, y) and neither text_coords nor texture can be NULL // sets the colors of the vertices, 1 usigned int for each vertex (0xFF0000FF is red, for example)
RDE_FUNC void rde_rendering_destroy_mesh(rde_mesh* _mesh);

#ifdef RDE_FBX_MODULE
RDE_FUNC rde_model* rde_rendering_load_fbx_model(const char* _fbx_path, const char* _texture_path);
#endif

#ifdef RDE_OBJ_MODULE
RDE_FUNC rde_model* rde_rendering_load_obj_model(const char* _obj_path);
#endif

#if defined(RDE_OBJ_MODULE) || defined(RDE_FBX_MODULE)
RDE_FUNC void rde_rendering_unload_model(rde_model* _model);
#endif

RDE_FUNC void rde_rendering_begin_drawing_3d(rde_camera* _camera, rde_window* _window);
RDE_FUNC void rde_rendering_draw_point_3d(rde_vec_3F _position, rde_color _color, rde_shader* _shader); /// Draws a point in 3D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_line_3d(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, rde_shader* _shader); /// Draws a batched line in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_draw_mesh_3d(const rde_transform* _transform, rde_mesh* _mesh, rde_shader* _shader);
RDE_FUNC void rde_rendering_draw_model_3d(const rde_transform* _transform, rde_model* _model, rde_shader* _shader);
RDE_FUNC void rde_rendering_end_drawing_3d();
#endif

/// ============================ AUDIO ======================================

#ifdef RDE_AUDIO_MODULE

RDE_FUNC void rde_audio_init(rde_sound_config _config);

RDE_FUNC rde_sound* rde_audio_load_sound(const char* _sound_path);
RDE_FUNC void rde_audio_unload_sound(rde_sound* _sound);

RDE_FUNC void rde_audio_play_sound(rde_sound* _sound);
RDE_FUNC void rde_audio_pause_sound(rde_sound* _sound);
RDE_FUNC void rde_audio_stop_sound(rde_sound* _sound);
RDE_FUNC void rde_audio_restart_sound(rde_sound* _sound);

RDE_FUNC bool rde_audio_is_sound_playing(rde_sound* _sound);

RDE_FUNC bool rde_audio_set_sound_volume(rde_sound* _sound, float _volume);

RDE_FUNC void rde_audio_end();

#endif

/// ============================ PHYSICS ====================================

#ifdef RDE_PHYSICS_2D_MODULE

#endif

/// ============================ FILE SYSTEM ================================

#endif

#ifdef __cplusplus
}
#endif