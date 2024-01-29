#ifndef RDE_H
#define RDE_H

// This is needed because of C++ name mangling, if this lib is linked to a C++ project and is compiled
// without extern "C" won't work.
#ifdef __cplusplus
extern "C" {
#endif

// 											==============================================================================
// 											=						GENERAL INFO ABOUT THE ENGINE					 	 =
// 											==============================================================================
// 
// - Most types exposed to the user are defined in the source file, and for the user they
//	 are just pointers that can be used to operate on those structures, and this has a reason.
//	 Some fields on the structs could be modified by end-user wihtout side-effects, but others
//	 may cause serious problem, unexpected behaviour or even crash the application, and this is
//	 why they are hidden from the end-user, that being said, there are some data that it is still
//	 relevant for the user. For those cases, every 'typedef struct my_hidden_struct my_hidden_struct'
//	 has an associeted a 'my_hidden_struct_data' which contains all the relevant information that 
//	 the end-user could need.
//
// - API "namespaces". As C has no namespaces, the API of the engine uses a naming convention that
//   mimics namespaces. Everything on the engine starts with 'rde_'. Then the next namespace, for
//	 example 'rde_window', 'rde_rendering', 'rde_util'... It can have many "namespaces" levels, such 
//	 as 'rde_rendering_model', which would be rde::rendering::model and then all methods related to
//	 models, such as 'rde_rendering_model_load' or 'rde_rendering_model_unload'. There is a way to
//	 disable namespaces, just define RDE_NO_NAMESPACES before including rde.h
//
// - Any function that has _inner_ in the name is just part of the internal computation of the engine,
//	 and will not be exposed to the user API.
//
// - Documentation is over-explanatory as I use it to later generate an API Documentation with NaturalDocs.


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
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

// 											==============================================================================
// 											=									DEFINES					 	   	 		 =
// 											==============================================================================

// =================================================================== NUMERIC AND CONSTANTS ===================================================================

// Group: Constants

// Constant: RDE_EPSILON
// Margin error for physics and mathematics calculations, by default is 0.0001f
#define RDE_EPSILON 0.0001f

// Constant: RDE_PI
// PI number, by default is 3.14159265358979323846264338327950288
#define RDE_PI 3.14159265358979323846264338327950288

// Constant: RDE_MAX_AMOUNT_OF_WINDOWS
// Max number of windows the application can create, by default is 10
#define RDE_MAX_AMOUNT_OF_WINDOWS 10

// Constant: RDE_MAX_VERTICES_PER_BATCH
// Max number of vertices to group on a 2D batch drawcall, by default is 50000
#define RDE_MAX_VERTICES_PER_BATCH 50000

// Constant: RDE_MAX_LOADABLE_SHADERS
// Max number of shaders the application can create and load, by default is 256
#define RDE_MAX_LOADABLE_SHADERS 256

// Constant: RDE_MAX_LOADABLE_TEXTURES
// Max number of textures the application can create and load, by default is 512
#define RDE_MAX_LOADABLE_TEXTURES 512

// Constant: RDE_MAX_LOADABLE_ATLASES
// Max number of atlases the application can create and load, by default is 512
#define RDE_MAX_LOADABLE_ATLASES 512

// Constant: RDE_MAX_LOADABLE_FONTS
// Max number of fonts the application can create and load, by default is 512
#define RDE_MAX_LOADABLE_FONTS 512

// Constant: RDE_MAX_LOADABLE_MODELS
// Max number of 3D models the application can create and load, by default is 512
#define RDE_MAX_LOADABLE_MODELS 512

// Constant: RDE_MAX_LOADABLE_MODELS_TEXTURES
// Max number of textures for 3D models the application can create and load, by default is 512
#define RDE_MAX_LOADABLE_MODELS_TEXTURES 512

#ifdef	RDE_AUDIO_MODULE
// Constant: RDE_MAX_LOADABLE_SOUNDS
// Max number of sounds the application can create and load, by default is 128
#define RDE_MAX_LOADABLE_SOUNDS 128

// Constant: RDE_MAX_LOADABLE_MUSICS
// Max number of musics the application can create and load, by default is 128
#define RDE_MAX_LOADABLE_MUSICS 128
#endif

// Constant: RDE_INT_MAX
// Max value of an integer 2147483647
#define RDE_INT_MAX 2147483647

// Constant: RDE_INT_MIN
// Min value of an integer -2147483648
#define RDE_INT_MIN -2147483648

// Constant: RDE_LONG_MAX
// Max value of a long 2147483647
#define RDE_LONG_MAX 2147483647

// Constant: RDE_LONG_MIN
// Min value of a long -2147483648
#define RDE_LONG_MIN -2147483648

// Constant: RDE_FLOAT_MAX
// Max value of a float 3.402823466 E+38
#define RDE_FLOAT_MAX 3.402823466 E + 38

// Constant: RDE_FLOAT_MIN
// Min value of a float 1.175494351 E-38
#define RDE_FLOAT_MIN 1.175494351 E - 38

// Constant: RDE_DOUBLE_MAX
// Max value of n double 1.7976931348623158 E+308
#define RDE_DOUBLE_MAX 1.7976931348623158 E + 308

// Constant: RDE_DOUBLE_MIN
// Min value of a double 2.2250738585072014 E-308
#define RDE_DOUBLE_MIN 2.2250738585072014 E - 308

// Constant: RDE_UINT_MAX
// Max value of an unsifned int 4294967295
#define RDE_UINT_MAX 4294967295

// Constant: RDE_ULONG_MAX
// Max value of an unsigned long 4294967295
#define RDE_ULONG_MAX 4294967295

// Constant: RDE_ARR_AMOUNT_INCREASE
// Amount to increase when capacity is full, by default 2
#define RDE_ARR_AMOUNT_INCREASE 2
	
// Constant: RDE_ARR_OPERAND_INCREASE
// Operand to use when capacity is full, by default * and can also be +
#define RDE_ARR_OPERAND_INCREASE *
	
// Constant: RDE_ARR_DEFAULT_CAPACITY
// Default capacity when creating a new dynamic array, by default 50
#define RDE_ARR_DEFAULT_CAPACITY 50

// Constant: RDE_HASH_MAP_DEFAULT_CAPACITY
// Default capacity when creating a new hash map, by default 64. MUST be a power of 2.
#define RDE_HASH_MAP_DEFAULT_CAPACITY 64
	
// Constant: RDE_STR_DEFAULT_SIZE
// Default size when creating a new string, by default 100
#define RDE_STR_DEFAULT_SIZE 100

// Constant: RDE_SHADER_LINE
// Name for the line shader, it is used to search this shader with <rde_rendering_shader_get_by_name>. By default is 'line_shader'.
#define RDE_SHADER_LINE "line_shader"

// Constant: RDE_SHADER_COLOR
// Name for the geometry shapes shader, it is used to search this shader with <rde_rendering_shader_get_by_name>. By default is 'color_shader'.
#define RDE_SHADER_COLOR "color_shader"

// Constant: RDE_SHADER_TEXTURE
// Name for the textures, it is used to search this shader with <rde_rendering_shader_get_by_name>. By default is 'texture_shader'.
#define RDE_SHADER_TEXTURE "texture_shader"

// Constant: RDE_SHADER_TEXT
// Name for the text, it is used to search this shader with <rde_rendering_shader_get_by_name>. By default is 'text_shader'.
#define RDE_SHADER_TEXT "text_shader"

// Constant: RDE_SHADER_FRAMEBUFFER
// Name for the framebuffer, it is used to search this shader with <rde_rendering_shader_get_by_name>. By default is 'framebuffer_shader'.
#define RDE_SHADER_FRAMEBUFFER "framebuffer_shader"

// Constant: RDE_SHADER_MESH
// Name for the 3D meshes, it is used to search this shader with <rde_rendering_shader_get_by_name>. By default is 'mesh_shader'.
#define RDE_SHADER_MESH "mesh_shader"

// Constant: RDE_SHADER_SKYBOX
// Name for the skybox, it is used to search this shader with <rde_rendering_shader_get_by_name>. By default is 'skybox_shader'.
#define RDE_SHADER_SKYBOX "skybox_shader"

// Constant: RDE_SHADER_SHADOWS
// Name for the shadow map, it is used to search this shader with <rde_rendering_shader_get_by_name>. By default is 'depth_shadows_shader'.
#define RDE_SHADER_DEPTH_SHADOWS "depth_shadows_shader"

/// =================================================================== COMPILATION AND EXPORT ===================================================================

// Group: Macros
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

// Macro: RDE_IS_MAC
// Checks if the current platform is MacOS. Use for compile time.
#define RDE_IS_MAC() (defined(__APPLE__) && defined(MAC_PLATFORM))

// Macro: RDE_IS_WINDOWS
// Checks if the current platform is Windows. Use for compile time.
#define RDE_IS_WINDOWS() _WIN32

// Macro: RDE_IS_WASM
// Checks if the current platform is WASM. Use for compile time.
#define RDE_IS_WASM() __EMSCRIPTEN__

// Macro: RDE_IS_LINUX
// Checks if the current platform is Linux. Use for compile time.
#define RDE_IS_LINUX() (defined(__linux__))

// Macro: RDE_IS_DESKTOP
// Checks if the current platform is Windows, Linux or MacOS. Use for compile time.
#define RDE_IS_DESKTOP() (RDE_IS_LINUX() || RDE_IS_MAC() || RDE_IS_WINDOWS())

// Macro: RDE_IS_IOS
// Checks if the current platform is iOS (iPhone or iPad). Use for compile time.
#define RDE_IS_IOS() (defined(__APPLE__) && defined(IOS_PLATFORM))

// Macro: RDE_IS_APPLE
// Checks if the current platform is iPhone, iPad or MacOS. Use for compile time.
#define RDE_IS_APPLE() (RDE_IS_MAC() || RDE_IS_IOS())

// Macro: RDE_IS_ANDROID
// Checks if the current platform is Android. Use for compile time.
#define RDE_IS_ANDROID() (defined(__ANDROID__))

// Macro: RDE_IS_MOBILE
// Checks if the current platform is Android, iPad or iPhone. Use for compile time.
#define RDE_IS_MOBILE() (RDE_IS_ANDROID() || RDE_IS_IOS())

#if RDE_IS_WINDOWS()
// Macro: RDE_FUNC
// Export macro use for windows, so the marked functions are exported correctly to the library.
	#define RDE_FUNC __declspec(dllexport)

// Macro: RDE_FUNC_STATIC
// Same as <RDE_FUNC> but static
	#define RDE_FUNC_STATIC __declspec(dllexport) static

// Macro: RDE_FUNC_EXTERNAL
// Same as <RDE_FUNC> but extern "C".
	#define RDE_FUNC_EXTERNAL extern "C" RDE_FUNC
#else
	#define RDE_FUNC 
	#define RDE_FUNC_STATIC static
	#define RDE_FUNC_EXTERNAL
#endif

#if RDE_IS_WINDOWS()
	#include <io.h>
	#include <fcntl.h>
	#include <windows.h>
	#include <shlwapi.h>
#endif			 

// Macro: RDE_SHOW_WINDOWS_CONSOLE
// If used on a Windows platform, it will show the windows console while executing the main program.
#define RDE_SHOW_WINDOWS_CONSOLE 							\
	if(AllocConsole()) {									\
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);	\
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);		\
	}

/// =================================================================== PLATFORM SPECIFIC TYPES ===================================================================

typedef unsigned long ulong;
typedef unsigned int uint;

/// =================================================================== UTIL ===================================================================

#ifndef typeof
#ifdef __clang__
#define typeof(x) __typeof__(x)
#endif
#endif


#define RDE_STRINGIZE(x) STRINGIZE2(x)
#define RDE_STRINGIZE2(x) #x

// Macro: RDE_LINE_STRING
// Gives the line where it is called as a string.
#define RDE_LINE_STRING STRINGIZE(__LINE__)

// Macro: RDE_FUNC_STRING
// Gives the function name where it is called as a string.
#define RDE_FUNC_STRING STRINGIZE(__func__)

// Macro: RDE_FILE_STRING
// Gives the file name where it is called as a string.
#define RDE_FILE_STRING STRINGIZE(__FILE__)

// Macro: RDE_COLOR_TO_HEX_COLOR
// Transforms a rde_color into an int hex value.
// Parameters:
//	_color - rde_color to convert to int
//
//	======= C =======
//	int _green_hex = RDE_COLOR_TO_HEX_COLOR(RDE_COLOR_GREEN);
//	=================
#define RDE_COLOR_TO_HEX_COLOR(_color) (((int)_color.a) << 24) + (((int)_color.b) << 16) + (((int)_color.g) << 8) + ((int)_color.r)

// Macro: RDE_DEPRECATED
// Used to deprecate functions or structs.
// Parameters:
//	_func 	 	 - full signature of the function.
//	_message 	 - why it is depracated.
//	_replacement - what to use instead.
//
//	======= C =======
//	RDE_DEPRECATED(my_deprecated_func(int _a, bool _c), "This was deprecated on version 1.4", "Use my_non_deprecated_func(int _a, bool _c, const char* _str)");
//	=================
#define RDE_DEPRECATED(_func, _message, _replacement) _func __attribute__((deprecated(_message, _replacement)));


#if !RDE_IS_ANDROID()
//  Macro: rde_log_level
//
//	Prints to the console with a specific severity (which includes an automatic color). Adds new line automatically.
//
//	Parameters:
//		_level - level of severity.
//		_fmt   - formatted C style string to print.
//	======= C =======
//	rde_log_level(RDE_LOG_LEVEL_INFO, "This number %i is an %s", 5, "integer");
//	=================
	#define rde_log_level(_level, _fmt, ...) 							\
	do { 																\
		if(!rde_engine_logs_supressed()) {								\
			rde_log_level_inner(_level, _fmt __VA_OPT__(,) __VA_ARGS__);\
			printf("\n");												\
		}																\
	} while(0);

//  Macro: rde_log_color
//
//	Prints to the console on a specific color. Adds new line automatically.
//
//	Parameters:
//		_color - Color to print to the console.
//		_fmt   - formatted C style string to print.
//	======= C =======
//	rde_log_color(RDE_LOG_COLOR_BLUE, "This number %i is an %s and the line will be blue", 5, "integer");
//	=================
	#define rde_log_color(_color, _fmt, ...) 								\
	do { 																	\
		if(!rde_engine_logs_supressed()) {									\
			rde_log_color_inner(_color, _fmt  __VA_OPT__(,) __VA_ARGS__);	\
			printf("\n");													\
		}																	\
	} while(0);
	
//  Macro: rde_log_level_sl
//
//	Prints to the console with a specific severity (which includes an automatic color). Does not add new.
//
//	Parameters:
//		_level - level of severity.
//		_fmt   - formatted C style string to print.
//	======= C =======
//	rde_log_level_sl(RDE_LOG_LEVEL_INFO, "This number %i is an %s", 5, "integer");
//	=================
	#define rde_log_level_sl(_level, _fmt, ...) do { 						\
		if(!rde_engine_logs_supressed()) {									\
			rde_log_level_inner(_level, _fmt  __VA_OPT__(,) __VA_ARGS__);	\
		}																	\
	} while(0);
	
//  Macro: rde_log_color_sl
//
//	Prints to the console on a specific color. Does not add new line.
//
//	Parameters:
//		_color - Color to print to the console.
//		_fmt   - formatted C style string to print.
//	======= C =======
//	rde_log_color_sl(RDE_LOG_COLOR_BLUE, "This number %i is an %s and the line will be blue", 5, "integer");
//	=================
	#define rde_log_color_sl(_color, _fmt, ...) do { 						\
		if(!rde_engine_logs_supressed()) {									\
			rde_log_color_inner(_color, _fmt  __VA_OPT__(,) __VA_ARGS__);	\
		}																	\
	} while(0);
#else
	#define rde_log_level(_level, _fmt, ...) do { 												\
		if(!rde_engine_logs_supressed()) {														\
			__android_log_print(ANDROID_LOG_DEBUG, "SDL_RDE", _fmt __VA_OPT__(,) __VA_ARGS__);	\
			printf("\n");																		\
		}																						\
	} while(0);
	
	#define rde_log_color(_color, _fmt, ...) do { 												\
		if(!rde_engine_logs_supressed()) {														\
			__android_log_print(ANDROID_LOG_DEBUG, "SDL_RDE", _fmt __VA_OPT__(,) __VA_ARGS__);	\
			printf("\n");																		\
		}																						\
	} while(0);
	
	#define rde_log_level_sl(_level, _fmt, ...) do { 											\
		if(!rde_engine_logs_supressed()) {														\
			__android_log_print(ANDROID_LOG_DEBUG, "SDL_RDE", _fmt __VA_OPT__(,) __VA_ARGS__);	\
		}																						\
	} while(0);
	
	#define rde_log_color_sl(_color, _fmt, ...) do { 											\
		if(!rde_engine_logs_supressed()) {														\
			__android_log_print(ANDROID_LOG_DEBUG, "SDL_RDE", _fmt __VA_OPT__(,) __VA_ARGS__);	\
		}																						\
	} while(0);
#endif

#define RDE_SAFE_ARR_ACCESS(_type) RDE_FUNC _type rde_arr_s_get_##_type(uint _index, _type* _arr, uint _arr_size, char* _fmt, ...);
#define RDE_SAFE_ARR_SET(_type) RDE_FUNC void rde_arr_s_set_##_type(uint _index, _type _value, _type* _arr, uint _arr_size, char* _fmt, ...);
#define RDE_DECLARE_CLAMP_FUNCS(_type) RDE_FUNC _type rde_math_clamp_##_type(_type _value, _type _min, _type _max);
#define RDE_DECLARE_EASING_FUNCS(_form, _type) RDE_FUNC float rde_math_easing_##_form##_##_type(float _current_time, float _start_value, float _change_in_value, float _duration);
	
// Macro: rde_render_3d
// More compact way of rendering 3D without shadows. Result of doing <rde_rendering_3d_begin_drawing>, draw your scene, <rde_rendering_3d_end_drawing>
//
// Parameters:
//	_window - rde_window pointer being rendered.
//	_camera - rde_camera pointer that will render onto the window.
//	_draw_wireframe - bool to draw wireframe over the models.
//	_block_of_code - custom rendering code, must be between { }.
//
//	======= C =======
//	rde_render_3d(_my_window_pointer, _my_camera_pointer, false, {
//		rde_rendering_3d_draw_model(_my_transform, _my_model, NULL);
//	});
//	=================
#define rde_render_3d(_window, _camera, _draw_wireframe, _block_of_code)\
	rde_rendering_3d_begin_drawing(_camera, _window, _draw_wireframe);	\
	_block_of_code														\
	rde_rendering_3d_end_drawing();

// Macro: rde_render_3d_with_shadows
// More compact way of rendering 3D with shadows. Result of doing <rde_rendering_3d_begin_drawing>, draw your scene, <rde_rendering_3d_end_drawing>
//
// Parameters:
//	_window - rde_window pointer being rendered.
//	_camera - rde_camera pointer that will render onto the window.
//	_draw_wireframe - bool to draw wireframe over the models.
//	_block_of_code - custom rendering code, must be between { }.
//
//	======= C =======
//	rde_render_3d_with_shadows(_my_window_pointer, _my_camera_pointer, false, {
//		rde_rendering_3d_draw_model(_my_transform, _my_model, NULL);
//	});
//	=================
#define rde_render_3d_with_shadows(_window, _camera, _draw_wireframe, _block_of_code)	\
	rde_rendering_3d_begin_drawing(_camera, _window, _draw_wireframe);					\
	rde_rendering_shadows_begin(_window, _camera);										\
	_block_of_code																		\
	rde_rendering_shadows_end();														\
	_block_of_code																		\
	rde_rendering_3d_end_drawing();

// Macro: rde_render_2d
// More compact way of rendering 2D. Result of doing <rde_rendering_2d_begin_drawing>, draw your scene, <rde_rendering_2d_end_drawing>
//
// Parameters:
//	_window - rde_window pointer being rendered.
//	_camera - rde_camera pointer that will render onto the window.
//	_is_hud - bool to know if it is something to be drawn as Hud, which means it will stay on place on the screen even if camera moves.
//	_block_of_code - custom rendering code, must be between { }.
//
//	======= C =======
//	rde_render_2d(_my_window_pointer, _my_camera_pointer, false, {
//		rde_rendering_2d_texture(_my_transform, _my_texture, RDE_COLO_WHITE, NULL);
//	});
//	=================
#define rde_render_2d(_window, _camera, _is_hud, _block_of_code)\
	rde_rendering_2d_begin_drawing(_camera, _window, _is_hud);	\
	_block_of_code												\
	rde_rendering_2d_end_drawing();

#if RDE_IS_WINDOWS()
// Macro: rde_strcpy
// Platform independent standard strcpy.
//
// Parameters:
//	_dst - char array to copy data.
//	_dst_size - size of char array to copy data.
//	_src - data to copy into _dst.
//
//	======= C =======
//	char _my_dst[128] = {0};
//	rde_strcpy(_my_dst, 128, _source_data_pointer);
//	=================
// or
//	======= C =======
//	char _my_dst[128] = {0};
//	rde_strcpy(_my_dst, 128, "Hello, Duck!");
//	=================
#define rde_strcpy(_dst, _dst_size, _src) strcpy_s(_dst, _dst_size, _src)
#else
#define rde_strcpy(_dst, _dst_size, _src) strcpy(_dst, _src)
#endif

#if RDE_IS_WINDOWS()
// Macro: rde_strcat
// Platform independent standard strcat.
//
// Parameters:
//	_dst - char array to concat data.
//	_dst_size - size of char array to concat data.
//	_src - data to concat into _dst.
//
//	======= C =======
//	char _my_dst[128] = {0};
//	rde_strcat(_my_dst, 128, "Hello, ");
//	rde_strcat(_my_dst, 128, "Duck!");
//	=================
#define rde_strcat(_dst, _dst_size, _src) strcat_s(_dst, _dst_size, _src)
#else
#define rde_strcat(_dst, _dst_size, _src) strcat(_dst, _src)
#endif

#if RDE_IS_WINDOWS()
// Macro: rde_strncat
// Platform independent standard strncat.
//
// Parameters:
//	_dst - char array to concat data.
//	_dst_size - size of char array to concat data.
//	_src - data to concat into _dst.
//	_src_size - amount of bytes to copy from _src to _dst.
//
//	======= C =======
//	char _my_dst[128] = {0};
//	rde_strncat(_my_dst, 128, "Hello, ", 7);
//	rde_strncat(_my_dst, 128, "Duck!", 5);
//	=================
#define rde_strncat(_dst, _dst_size, _src, _src_amount) strncat_s(_dst, _dst_size, _src, _src_amount)
#else
#define rde_strncat(_dst, _dst_size, _src, _src_amount) strncat(_dst, _src, _src_amount)
#endif

#if RDE_IS_WINDOWS()
// Macro: rde_strtok
// Platform independent standard strtok.
//
// Parameters:
//	_dst - char array to concat data.
//	_del - tokenizer delimiter.
//	_ctx - context. This is only used on Windows on specific cases <https://learn.microsoft.com/es-es/cpp/c-runtime-library/reference/strtok-s-strtok-s-l-wcstok-s-wcstok-s-l-mbstok-s-mbstok-s-l?view=msvc-170>
//
//	======= C =======
//	char* _tokens = rde_strtok("Hello, Duck!", ",", NULL);
//	=================
#define rde_strtok(_str, _del, _ctx) strtok_s(_str, _del, _ctx)
#else
#define rde_strtok(_str, _del, _ctx) strtok(_str, _del)
#endif

#if RDE_IS_WINDOWS()
// Macro: rde_snprintf
// Platform independent standard rde_snprintf.
//
// Parameters:
//	_str - destination to save the formatted string.
//	_str_size - size of the destination.
//	_fmt - formatted string.
//
//	======= C =======
//  char _a[128] = {0};
//	rde_snprintf(_a, 128, "%s, %s", "Hello",      "Duck");
//	=================
#define rde_snprintf(_str, _str_size, _fmt, ...) snprintf(_str, _str_size, _fmt, __VA_ARGS__)
#else
#define rde_snprintf(_str, _str_size, _fmt, ...) snprintf(_str, _str_size, _fmt, __VA_ARGS__)
#endif
	
#if RDE_IS_WINDOWS()
// Macro: rde_snprintf
// Platform independent standard rde_vsprintf.
//
// Parameters:
//	_str - destination to save the formatted string.
//	_str_size - size of the destination.
//	_fmt - formatted string.
//
//	======= C =======
//	va_list _args;
//	va_start(_args, _fmt);
//	char _msg = {0};
//	rde_vsprintf(_msg, _fmt, _args);
//	va_end(_args);
//	=================	
#define rde_vsprintf(_str, _str_size, _fmt, ...) vsprintf_s(_str, _str_size, _fmt, __VA_ARGS__)
#else
#define rde_vsprintf(_str, _str_size, _fmt, ...) vsprintf(_str, _fmt, __VA_ARGS__)
#endif

#ifndef rde_malloc
// Macro: rde_malloc
// Custom malloc, defined as default malloc but can be reimplemented by end users. It checks for memory allocation errors.
//
// Parameters:
//	_ptr - pointer that the memory will be allocated to.
//	_type - type to alloc.
//	_amount - amount of elements of size _type_size to allocate.
//
//	======= C =======
//	char* _my_string = NULL;
//	rde_malloc(_my_string, char, 128);
//	=================
	#define rde_malloc(_ptr, _type, _amount) 																							\
		do {																															\
			rde_critical_error(_ptr != NULL, "rde_malloc -> Tried to allocate to _ptr, but it was not NULL, check if _ptr has been initialized to NULL, otherwise it may be pointing to already allocated memory and cause a memory leak. \n"); \
			_ptr = (_type*)malloc(sizeof(_type) * (_amount));																			\
			if(_ptr == NULL) {																											\
				rde_critical_error(true, "rde_malloc -> Could not allocate enough memory (%d bytes) \n", sizeof(_type) * (_amount));	\
			}																															\
		} while(0)

// Macro: rde_malloc_init
// Same as <rde_malloc> but it declares the variable _ptr.
//
// Parameters:
//	_ptr - pointer that the memory will be allocated to.
//	_type - type to alloc.
//	_amount - amount of elements of size _type_size to allocate.
//
//	======= C =======
//	rde_malloc_init(_my_string, char, 128);
//	=================
	#define rde_malloc_init(_ptr, _type, _amount) _type* _ptr = NULL; rde_malloc(_ptr, _type, _amount)
#endif

#ifndef rde_calloc
// Macro: rde_calloc
// Custom rde_calloc, defined as default rde_calloc but can be reimplemented by end users. It checks for memory allocation errors.
//
// Parameters:
//	_ptr - pointer that the memory will be allocated to.
//	_type - type to alloc.
//	_amount - amount of elements of size _type_size to allocate.
//
//	======= C =======
//	char* _my_string = NULL;
//	rde_calloc(_my_string, char, 128);
//	=================
	#define rde_calloc(_ptr, _type, _amount) 																							\
		do {																															\
			rde_critical_error(_ptr != NULL, "rde_calloc -> Tried to allocate to _ptr, but it was not NULL, check if _ptr has been initialized to NULL, otherwise it may be pointing to already allocated memory and cause a memory leak. \n"); \
			_ptr = (_type*)calloc(sizeof(_type), (_amount));																			\
			if(_ptr == NULL) {																											\
				rde_critical_error(true, "rde_calloc -> Could not allocate enough memory (%d bytes) \n", sizeof(_type) * (_amount));	\
			}																															\
		} while(0)

// Macro: rde_calloc_init
// Same as <rde_calloc> but it declares the variable _ptr.
//
// Parameters:
//	_ptr - pointer that the memory will be allocated to.
//	_type - type to alloc.
//	_amount - amount of elements of size _type_size to allocate.
//
//	======= C =======
//	rde_calloc_init(_my_string, char, 128);
//	=================
	#define rde_calloc_init(_ptr, _type, _amount) _type* _ptr = NULL; rde_calloc(_ptr, _type, _amount)
#endif

#ifndef rde_realloc
// Macro: rde_realloc
// Custom rde_realloc, it allocs new memory with std realloc, and frees the memory if needed. Can be reimplemented by end users.
//
// Parameters:
//	_ptr - pointer that the memory will be allocated to.
//	_type - type to alloc.
//	_amount - amount of elements of size _type_size to allocate.
//
//	======= C =======
//	char* _my_string = NULL;
//	rde_calloc(_my_string, sizeof(char), 128);
//	rde_realloc(_my_string, _my_string, char, 256);
//	=================
#define rde_realloc(_ptr_src, _ptr_dst, _type, _amount)																				\
	do {																															\
		rde_critical_error(_ptr_src == NULL, "rde_realloc -> _ptr_src cannot be NULL \n");											\
		rde_critical_error(_ptr != NULL, "rde_realloc -> Tried to allocate to _ptr_dst, but it was not NULL, check if _ptr_dst has been initialized to NULL, otherwise it may be pointing to already allocated memory and cause a memory leak. \n"); \
		_type* _new_memory = (_type*)realloc(_ptr_src, sizeof(_type) * (_amount));													\
		if(_new_memory == NULL) {																									\
			free(_ptr_src);																											\
			rde_critical_error(true, "rde_realloc -> Could not allocate enough memory (%d bytes) \n", sizeof(_type) * (_amount));	\
		}																															\
		_ptr_dst = _new_memory;																										\
	} while(0)
#endif

#ifndef rde_free
// Macro: rde_free
// Custom rde_free, defined as default rde_free that also reassings the _ptr to NULL, but can be reimplemented by end users.
//
// Parameters:
//	_ptr - pointer to free.
//
//	======= C =======
//	rde_calloc_init(_my_string, sizeof(char), 128);
//	rde_free(_my_string);
//	=================
	#define rde_free(_ptr) do { free(_ptr); _ptr = NULL; } while(0)
#endif

// Macro: rde_flags_has
// Computes if _value contains the specified flags on _flag. _flag can contain multiple flags at once.
//
// Parameters:
//	_value - (uint/uint32_t) value containing flags.
//	_flag - (uint/uint32_t) flags to test.
//
//	======= C =======
//	typedef enum {
//		a = 1 << 0,
//		b = 1 << 1,
//		c = 1 << 2,
//	} e_;
//	e _e = e_a | e_c;
//	bool _f = rde_util_flag_has(_e, e_a);
//	=================
#define rde_flags_has(_value, _flag) (((_value) & (_flag)) == (_flag))

// Macro: rde_util_next_power_of_2
// This is a very fast way of computing next power of 2 of a given element.
// Source: https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2.
// CHAR_BIT is equals to 8 on 64bit systems and to 4 on 32bit systems, so on 64 bit system is (sizeof(unsigned long) * CHAR_BIT) => 64 and on 32bits systems is 32
// __builtin_clzl: counts leading number of zeros
//
// Parameters:
//	_value - (uint/uint32_t) value to check next power of 2.
//
//	======= C =======
//	uint _a = 30;
//	uint _next_pow_2 = rde_util_next_power_of_2(_a);
//	=================
#define rde_util_next_power_of_2(_value) (1 << ((sizeof(unsigned long) * CHAR_BIT) - __builtin_clzl(_value - 1)))
	
// Macro: RDE_PROFILE_TIME
// Simple time counter to profile a specific block of code.
//
// Parameters:
//	_name - destination to save the formatted string.
//	_code_block - size of the destination.
//
//	======= C =======
//	RDE_PROFILE_TIME("function_a", function_a())
//	=================
#define RDE_PROFILE_TIME(_name, _code_block)				   									\
{									  															\
	clock_t  _start = clock();							 										\
	_code_block																					\
	clock_t _end = clock() - _start;					   										\
	rde_log_level(RDE_LOG_LEVEL_INFO, "%s - Took %f""s", _name, ((double)_end)/CLOCKS_PER_SEC); \
}

// Macro: RDE_MAX
// Gets the maximum value of 2 values.
//
// Parameters:
//	_a - value a.
//	_b - value b.
//
//	======= C =======
//	int _max = RDE_MAX(15, -5)
//	=================
#define RDE_MAX(a, b) ((a) > (b) ? (a) : (b));

// Macro: RDE_MIN
// Gets the minimum value of 2 values.
//
// Parameters:
//	_a - value a.
//	_b - value b.
//
//	======= C =======
//	int _min = RDE_MIN(15, -5)
//	=================
#define RDE_MIN(a, b) ((a) < (b) ? (a) : (b));

/// =================================================================== WARNING SILENCER ===================================================================

// Macro: RDE_UNUSED
// Used to silence non-used variables warnings.
//
// Parameters:
//	_x - variable name to be silenced.
//
//	======= C =======
//	RDE_UNUSED(_my_unused_var)
//	=================
#define RDE_UNUSED(_x) (void)_x;

// Macro: RDE_UNIMPLEMENTED
// Used to trigger a crash if a function is not implemented.
//
//	======= C =======
//	RDE_UNIMPLEMENTED()
//	=================
#define RDE_UNIMPLEMENTED() assert(false && __func__);

// Macro: RDE_UNIMPLEMENTED_STRUCT
// Used to give a "default" body to structure if it is not yet filled.
//
//	======= C =======
//	typedef struct {
//		RDE_UNIMPLEMENTED_STRUCT();
//	} my_unfilled_struct;
//	=================
#define RDE_UNIMPLEMENTED_STRUCT() short foo;


/// =================================================================== GENERIC FUNCS AND STRUCTS ===================================================================

#if RDE_IS_WINDOWS()
// Macro: RDE_MAIN
// Simple and direct entry point to use on main source file of a project. Everything done in macro can be done without it, this is just a "quicker/automatic" way to have an entry point for the game.
//
// Parameters:
//	_window - output variable where the default window will be saved.
//	_config_path - path to the config.rde file. This is REQUIRED to have to build an RDE application.
//	_mandatory_callbacks - struct of callbacks that the user MUST fill for the application to run. Struct is <rde_end_user_mandatory_callbacks>.
//	_init_func - function that will be executed after engine initialization.
//	_end_func - function that will be executed before engine destruction.
//
//	======= C =======
//	rde_window* default_window = NULL;
//
//	void on_event(rde_event* _event, rde_window* _window);
//	void on_update(float _dt);
//	void on_fixed_update(float _dt);
//	void on_late_update(float _dt);
//	void on_render(float _dt, rde_window* _window);
//
//	const rde_end_user_mandatory_callbacks mandatory_callbacks = {
//		&on_update,
//		&on_fixed_update,
//		&on_late_update,
//		&on_render
//	};
//
//	void init_func(int _argc, char** _argv) {
//		...	
//	}
//
//	void end_func() {
//		...	
//	}
//
//	RDE_MAIN(default_window, "assets/config.rde_config", mandatory_callbacks, init_func, end_func);
//	=================
#define RDE_MAIN(_window, _config_path, _mandatory_callbacks, _init_func, _end_func)	\
	int main(int _argc, char** _argv) {												 	\
		RDE_SHOW_WINDOWS_CONSOLE														\
																						\
		_window = rde_engine_create_engine(_argc, _argv, _config_path);				 	\
		rde_setup_initial_info(_mandatory_callbacks);								   	\
																						\
		_init_func(_argc, _argv);													   	\
																						\
		rde_engine_on_run();															\
		_end_func();																	\
		rde_engine_destroy_engine();													\
																						\
		return 0;																	   	\
	}
	
#elif RDE_IS_ANDROID()

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#define SDL_GESTURE_IMPLEMENTATION 1
#include "SDL3/SDL_gesture.h"
#include <jni.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/log.h>
#define RDE_MAIN(_window, _config_path, _mandatory_callbacks, _init_func, _end_func)   	\
	int main(int _argc, char* _argv[]) {											   	\
		_window = rde_engine_create_engine(_argc, _argv, _config_path);					\
		rde_setup_initial_info(_mandatory_callbacks);								  	\
																						\
		_init_func(_argc, _argv);													  	\
																						\
		rde_engine_on_run();														   	\
		_end_func();																   	\
		rde_engine_destroy_engine();												   	\
																						\
		return 0;																	  	\
	}

#else

#define RDE_MAIN(_window, _config_path, _mandatory_callbacks, _init_func, _end_func)	\
	int main(int _argc, char** _argv) {												 	\
		_window = rde_engine_create_engine(_argc, _argv, _config_path);				 	\
		rde_setup_initial_info(_mandatory_callbacks);								   	\
																						\
		_init_func(_argc, _argv);													   	\
																						\
		rde_engine_on_run();															\
		_end_func();																	\
		rde_engine_destroy_engine();													\
																						\
		return 0;																	   	\
	}

#endif

// Macro: RDE_SPECIALIZED_VEC2
// Declares a struct for a Vec2 of an specific type. This struct has fields, x and y.
//
// Parameters:
//	_type - the type of the fields.
//	_name - the name of the specialized Vec2.
// ======= C =======
// 	RDE_SPECIALIZED_VEC2(int, int)
// ==================
#define RDE_SPECIALIZED_VEC2(_type, _name) 	\
	typedef struct _name _name;				\
	struct _name {							\
		_type x;							\
		_type y;							\
	};

// Macro: RDE_SPECIALIZED_VEC3
// Declares a struct for a Vec3 of an specific type. This struct has fields, x, y and z.
//
// Parameters:
//	_type - the type of the fields.
//	_name - the name of the specialized Vec3.
// ======= C =======
// 	RDE_SPECIALIZED_VEC3(int, int)
// ==================
#define RDE_SPECIALIZED_VEC3(_type, _name) 	\
	typedef struct _name _name;				\
	struct _name {							\
		_type x;							\
		_type y;							\
		_type z;							\
	};

// Macro: RDE_SPECIALIZED_VEC4
// Declares a struct for a Vec4 of an specific type. This struct has fields, x, y, z and w.
//
// Parameters:
//	_type - the type of the fields.
//	_name - the name of the specialized Vec4.
// ======= C =======
// 	RDE_SPECIALIZED_VEC4(int, int)
// ==================
#define RDE_SPECIALIZED_VEC4(_type, _name) 	\
	typedef struct _name _name;				\
	struct _name {							\
		_type x;							\
		_type y;							\
		_type z;							\
		_type w;							\
	};

// Macro: RDE_SPECIALIZED_MAT2
// Declares a struct for a Mat2 of an specific type. This struct has 4 elements, that can be accessed as independent elements, as an array of 4 elements or an array of arrays.
//
// Parameters:
//	_type - the type of the fields.
//	_name - the name of the specialized Vec4.
// ======= C =======
// 	RDE_SPECIALIZED_MAT2(int, int)
// ==================
#define RDE_SPECIALIZED_MAT2(_type, _name) 	\
	typedef struct _name _name;				\
	struct _name {							\
		union {								\
			struct {						\
				float m00, m01;				\
				float m10, m11;				\
			};								\
											\
			float m[2][2];					\
			float v[4];						\
		};									\
	};

// Macro: RDE_SPECIALIZED_MAT3
// Declares a struct for a Mat3 of an specific type. This struct has 9 elements, that can be accessed as independent elements, as an array of 9 elements or an array of arrays.
//
// Parameters:
//	_type - the type of the fields.
//	_name - the name of the specialized Vec4.
// ======= C =======
// 	RDE_SPECIALIZED_MAT3(int, int)
// ==================
#define RDE_SPECIALIZED_MAT3(_type, _name) 	\
	typedef struct _name _name;				\
	struct _name {							\
		union {								\
			struct {						\
				float m00, m01, m02;		\
				float m10, m11, m12;		\
				float m20, m21, m22;		\
			};								\
											\
			float m[3][3];					\
			float v[9];						\
		};									\
	};

// Macro: RDE_SPECIALIZED_MAT4
// Declares a struct for a Mat4 of an specific type. This struct has 16 elements, that can be accessed as independent elements, as an array of 16 elements or an array of arrays.
//
// Parameters:
//	_type - the type of the fields.
//	_name - the name of the specialized Vec4.
// ======= C =======
// 	RDE_SPECIALIZED_MAT4(int, int)
// ==================
#define RDE_SPECIALIZED_MAT4(_type, _name) 		\
	typedef struct _name _name;					\
	struct _name {								\
		union {									\
			struct {							\
				float m00, m01, m02, m03;		\
				float m10, m11, m12, m13;		\
				float m20, m21, m22, m23;		\
				float m30, m31, m32, m33;		\
			};									\
												\
			float m[4][4];						\
			float v[16];						\
		};										\
	};

// Macro: rde_arr_decl
// Defines a new type of dynamic array.
//
// Parameters:
//	_type - the type of data that the array will hold.
//
//	======= C =======
//	typedef struct {
//		long a;
//	} A;
//
//	rde_arr_decl(A);
//	=================
#define rde_arr_decl(_type)	\
typedef struct {			\
	_type* memory;			\
	unsigned long capacity;	\
	unsigned long used;		\
} rde_arr_##_type

// Macro: rde_arr_init
// Inits a dynamic array inner fields to the default values.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//
//	======= C =======
//	rde_arr_A _arr;
// 	rde_arr_init(_arr);
//	=================
#define rde_arr_init(_dyn_arr) 					\
	do {										\
		(_dyn_arr)->memory = NULL;				\
		(_dyn_arr)->capacity = 0;				\
		(_dyn_arr)->used = 0;					\
	} while(0)

// Macro: rde_arr_new_with_capacity
// Allocates the data for the dynamic array. Needs to be freed with <rde_arr_free> once it is not needed.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//	_capacity - the initial amount of elements of the array.
//
//	======= C =======
//	rde_arr_new_with_capacity(_arr, 10);
//	=================
#define rde_arr_new_with_capacity(_dyn_arr, _capacity) 																												\
	do {																																							\
		if((_dyn_arr)->memory != NULL) {																															\
			free((_dyn_arr)->memory);																																\
		}																																							\
																																									\
		(_dyn_arr)->memory = (typeof((_dyn_arr)->memory))malloc(sizeof(typeof(*(_dyn_arr)->memory)) * _capacity);													\
		rde_critical_error((_dyn_arr)->memory == NULL, "Bytes %d could not be allocated for %s", sizeof(typeof(*(_dyn_arr)->memory)) * _capacity, "rde_arr new"); 	\
		(_dyn_arr)->used = 0;																																		\
		(_dyn_arr)->capacity = _capacity;																															\
	} while(0)

// Macro: rde_arr_new
// Allocates the data for the dynamic array. Needs to be freed with <rde_arr_free> once it is not needed.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//
//	======= C =======
//	rde_arr_new(_arr);
//	=================
#define rde_arr_new(_dyn_arr) rde_arr_new_with_capacity(_dyn_arr, RDE_ARR_DEFAULT_CAPACITY)
	
// Macro: rde_arr_add
// Adds an element to an existing dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//	_element - the data to add to the back of the array.
//
//	======= C =======
//	A _b = { .a = 10 };
//	rde_arr_add(_arr, _b);
//	=================
#define rde_arr_add(_dyn_arr, _element)																							\
	do {																														\
		rde_critical_error((_dyn_arr)->memory == NULL, "Uninitialized rde_arr");												\
		if((_dyn_arr)->used + 1 == (_dyn_arr)->capacity) {																		\
			(_dyn_arr)->capacity = (_dyn_arr)->capacity RDE_ARR_OPERAND_INCREASE RDE_ARR_AMOUNT_INCREASE;						\
			size_t _size = sizeof(typeof((_dyn_arr)->memory));																	\
			void* _new_memory = (void*)realloc((_dyn_arr)->memory, _size * (_dyn_arr)->capacity);								\
			if(_new_memory == NULL) {																							\
				free((_dyn_arr)->memory);																						\
				rde_critical_error(true, "Bytes %d could not be allocated for %s", _size * (_dyn_arr)->capacity, "rde_arr add");\
			}																													\
			(_dyn_arr)->memory = _new_memory;																					\
		}																														\
		(_dyn_arr)->memory[(_dyn_arr)->used++] = _element;																		\
	} while(0)

// Macro: rde_arr_remove
// Removes an element by index from an existing dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//	_index - the index to remove. There is a safety check on bounds.
//
//	======= C =======
//	rde_arr_remove(_arr, 0);
//	=================
#define rde_arr_remove(_dyn_arr, _index)																											\
	do {																																			\
		rde_critical_error((_dyn_arr)->memory == NULL, "Uninitialized rde_arr");																	\
		rde_critical_error(_index < 0 || (unsigned long)_index >= (_dyn_arr)->used, "Arr[%d] out of bounds, max %d \n", _index, (_dyn_arr)->used);	\
		rde_calloc_init(_new_memory, typeof(*(_dyn_arr)->memory), (_dyn_arr)->capacity);															\
		memcpy(_new_memory, (_dyn_arr)->memory, _index * sizeof(typeof(*(_dyn_arr)->memory)));														\
		if((ulong)_index != (_dyn_arr)->used - 1) {																									\
			memcpy(_new_memory + _index, (_dyn_arr)->memory + (_index + 1), ((_dyn_arr)->used - _index - 1) * sizeof(typeof(*(_dyn_arr)->memory)));	\
		}																																			\
		rde_free((_dyn_arr)->memory);																												\
		(_dyn_arr)->memory = _new_memory;																											\
		(_dyn_arr)->used--;																															\
	} while(0)

// Macro: rde_arr_get_element
// Gets an element by index from an existing dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//	_index - the index to remove. There is a safety check on bounds.
//	_value - variable to store the result.
//
//	======= C =======
//	A _retrieve;
//	rde_arr_get_element(_arr, 0, _retrieve);
//	=================
#define rde_arr_get_element(_dyn_arr, _index, _value) 																									\
	do {																																				\
		rde_critical_error((_dyn_arr)->memory == NULL, "Uninitialized rde_arr");																		\
		rde_critical_error(_index < 0 || (unsigned long)_index >= (_dyn_arr)->capacity, "Arr[%d] out of bounds, max %d", _index, (_dyn_arr)->capacity);	\
		_value = (_dyn_arr)->memory[_index];																											\
	} while(0)

// Macro: rde_arr_get_element_ptr
// Gets an element by index from an existing dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//	_index - the index to remove. There is a safety check on bounds.
//	_value - variable to store the result as pointer.
//
//	======= C =======
//	A* _retrieve;
//	rde_arr_get_element_ptr(_arr, 0, _retrieve);
//	=================
#define rde_arr_get_element_ptr(_dyn_arr, _index, _value) 																				\
	do {																																\
		rde_critical_error((_dyn_arr)->memory == NULL, "Uninitialized rde_arr");														\
		rde_critical_error(_index < 0 || (unsigned long)_index >= (_dyn_arr)->capacity, "Arr[%d] out of bounds, max %d", _index, (_dyn_arr)->capacity);\
		_value = &(_dyn_arr)->memory[_index];																							\
	} while(0)

// Macro: rde_arr_get_total_capacity
// Returns the full capacity of the dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//
//	======= C =======
//	ulong  _capacity = rde_arr_get_total_capacity(_arr);
//	=================
#define rde_arr_get_total_capacity(_dyn_arr) (_dyn_arr)->capacity
	
// Macro: rde_arr_get_length
// Returns the used capacity of the dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//
//	======= C =======
//	ulong  _used_capacity = rde_arr_get_length(_arr);
//	=================
#define rde_arr_get_length(_dyn_arr) (_dyn_arr)->used

// Macro: rde_arr_set_element
// Sets an element by index on an existing dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//	_index - the index to remove. There is a safety check on bounds.
//	_element - new element to place at _index.
//
//	======= C =======
//	A _new_b = { .a = -5 };
//	rde_arr_set_element_(_arr, 0, _new_b);
//	=================
#define rde_arr_set_element(_dyn_arr, _index, _element)																					\
	do {																																\
		rde_critical_error((_dyn_arr)->memory == NULL, "Uninitialized rde_arr");														\
		rde_critical_error(_index < 0 || (unsigned long)_index >= (_dyn_arr)->capacity, "Arr[%d] out of bounds, max %d", _index, (_dyn_arr)->capacity);\
		(_dyn_arr)->memory[_index] = _element;																							\
	} while(0)
	
// Macro: rde_arr_clear
// Clears all the data in the array to the default value. Resets 'used' field to 0. Does not deallocate memory.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//
//	======= C =======
//	rde_arr_clear(_arr);
//	=================
#define rde_arr_clear(_dyn_arr)													\
	do {																		\
		rde_critical_error((_dyn_arr)->memory == NULL, "Uninitialized rde_arr");\
		for(unsigned int _i = 0; _i < (_dyn_arr)->capacity; _i++) {				\
			(_dyn_arr)->memory[_i] = (_dyn_arr)->defaul_value;					\
		}																		\
		(_dyn_arr)->used = 0;													\
	} while(0)
	
// Macro: rde_arr_free
// Frees the dynamic array and resets all the fields.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//
//	======= C =======
//	rde_arr_free(_arr);
//	=================
#define rde_arr_free(_dyn_arr)			\
	do {								\
		if((_dyn_arr)->memory != NULL) {\
			free((_dyn_arr)->memory);	\
			(_dyn_arr)->memory = NULL;	\
		}								\
		(_dyn_arr)->capacity = 0;		\
		(_dyn_arr)->used = 0;			\
	} while(0)

// Macro: rde_hash_map_decl
// Declares a new type of hash_map, with key of type _key_type and value of type _value_type.
//
// It also declares 2 callbacks:
//	- typedef uint (*hash_fn_##_name)(_key_type*): pointer to function which will calculate the hash of the entries in the table. There are default ones for const char* (<rde_util_hash_map_str_hash>),
//	  and for int/uint (<rde_util_hash_map_int_hash>).
//	- typedef int (*cmp_fn_##_name)(_key_type*, _key_type*): pointer to function to compare keys. MUST retunr 0 if equals or other value of not equals.
//
// Parameters:
//	_name - name of the new struct.
//	_key_type - type of the key.
//	_value_type - type of the value.
//
//	======= C =======
//	rde_hash_map_decl(hash_map_test, const char*, int);
//	=================
#define rde_hash_map_decl(_name, _key_type, _value_type)  	\
    typedef struct _name _name;                           	\
    typedef struct entry_##_name entry_##_name;           	\
    typedef uint (*hash_fn_##_name)(_key_type*);          	\
    typedef int (*cmp_fn_##_name)(_key_type*, _key_type*);	\
															\
    struct entry_##_name {                                	\
        _key_type key;                                    	\
        _value_type value;                                	\
        RDE_HASH_MAP_STATE_ state;                        	\
    };                                                    	\
															\
    struct _name {                                        	\
        entry_##_name* entries;                           	\
        _key_type key_type;                               	\
        uint capacity;                                    	\
        uint actual_size;                                 	\
        uint used;                                        	\
        hash_fn_##_name hash_fn;                          	\
        cmp_fn_##_name cmp_fn;                            	\
        key_dbg_fn_##_name key_dbg_fn;                    	\
        value_dbg_fn_##_name value_dbg_fn;                	\
    }
    
#define rde_hash_map_init(_hash_map, _hash_fn, _cmp_fn)                                                                         															\
    do {                                                                                                                            														\
		rde_critical_error(_hash_fn == NULL, "Tried to use a NULL _hash_fn\n");																												\
		rde_critical_error(_cmp_fn == NULL, "Tried to use a NULL _cmp_fn\n");																												\
		static_assert(RDE_HASH_MAP_DEFAULT_CAPACITY && (RDE_HASH_MAP_DEFAULT_CAPACITY & (RDE_HASH_MAP_DEFAULT_CAPACITY - 1)) == 0, "RDE_HASH_MAP_DEFAULT_CAPACITY must be a power of 2");	\
        (_hash_map)->capacity = RDE_HASH_MAP_DEFAULT_CAPACITY;                                                                      														\
        (_hash_map)->actual_size = RDE_HASH_MAP_DEFAULT_CAPACITY;                                                                   														\
        (_hash_map)->used = 0;                                                                                                      														\
        (_hash_map)->entries = (typeof((_hash_map)->entries))calloc((_hash_map)->capacity, sizeof(typeof(*(_hash_map)->entries)));  														\
        for(uint _i = 0; _i < (_hash_map)->capacity; _i++) (_hash_map)->entries[_i].state = RDE_HASH_MAP_STATE_EMPTY;               														\
        (_hash_map)->hash_fn = _hash_fn;                                                                                            														\
        (_hash_map)->cmp_fn = _cmp_fn;                                                                                          															\
    } while(0)

#define rde_hash_map_hash_to_index(_hash_map, _ptr_key, _out_index)             	\
    do {                                                                        	\
		rde_critical_error(_hash_map == NULL, "Tried to use a NULL _hash_map\n");	\
        typeof((_hash_map)->key_type) _map_key = (_ptr_key);                    	\
        uint _key_hash = (_hash_map)->hash_fn(&(_map_key));                     	\
        _key_hash = hashmap_hash_default(&_key_hash, sizeof(_key_hash));        	\
        _out_index = _key_hash & ((_hash_map)->actual_size - 1);                	\
    } while(0)

#define rde_hash_map_get_ideal_size(_hash_map, _out_value)                                                  \
    do {                                                                                                    \
		rde_critical_error(_hash_map == NULL, "Tried to use a NULL _hash_map\n");							\
        uint _size = (_hash_map)->used;                                                                     \
        uint _ts = _size + (_size / 3);                                                                     \
        _out_value = (_ts < (_hash_map)->capacity ? (_hash_map)->capacity : rde_util_next_power_of_2(_ts)); \
    } while(0)

#define rde_hash_map_find_whole_entry(_hash_map, _ptr_key, _out_ptr_entry)      	\
    do {                                                                        	\
		rde_critical_error(_hash_map == NULL, "Tried to use a NULL _hash_map\n");	\
        uint _index = 0;                                                        	\
        rde_hash_map_hash_to_index((_hash_map), *(_ptr_key), _index);           	\
        bool _found = false;                                                    	\
                                                                                	\
        for(uint _i = 0; _i < (_hash_map)->actual_size; _i++) {                 	\
            typeof((_hash_map)->entries) _entry = &(_hash_map)->entries[_index];	\
            if(rde_util_flag_has(_entry->state, RDE_HASH_MAP_STATE_EMPTY)) {    	\
                _out_ptr_entry = _entry;                                        	\
                _found = true;                                                  	\
                break;                                                          	\
            }                                                                   	\
            if((_hash_map)->cmp_fn((_ptr_key), &(_entry->key)) == 0) {          	\
                _out_ptr_entry = _entry;                                        	\
                _found = true;                                                  	\
                break;                                                          	\
            }                                                                   	\
            _index = (_index + 1) & ((_hash_map)->actual_size - 1);             	\
        }                                                                       	\
        rde_critical_error(!_found, "Tried to get a non existen key\n");            \
    } while(0)

#define rde_hash_map_resize_and_rehash(_hash_map, _new_size)                                                                            \
    do {                                                                                                                                \
        rde_critical_error((_new_size & (_new_size - 1)) != 0, "Size must be ^2\n");                                                   	\
        rde_critical_error(_new_size < (_hash_map)->used, "New size must be > used size\n");                                            \
                                                                                                                                        \
        typeof((_hash_map)->entries) _new_entries = (typeof((_hash_map)->entries))calloc(_new_size, sizeof(*((_hash_map)->entries)));   \
        assert(_new_entries != NULL && "Could not allocate memory");                                                                    \
        for(uint _i = 0; _i < _new_size; _i++) _new_entries[_i].state = RDE_HASH_MAP_STATE_EMPTY;                                       \
                                                                                                                                        \
        uint _old_size = (_hash_map)->actual_size;                                                                                      \
        typeof((_hash_map)->entries) _old_entries = (_hash_map)->entries;                                                               \
        (_hash_map)->actual_size = _new_size;                                                                                           \
        (_hash_map)->entries = _new_entries;                                                                                            \
                                                                                                                                        \
        for(uint _i = 0; _i < _old_size; _i++) {                                                                                        \
            typeof((_hash_map)->entries) _old_entry = &_old_entries[_i];                                                                \
            if(rde_util_flag_has(_old_entry->state, RDE_HASH_MAP_STATE_EMPTY) ||                                                        \
                rde_util_flag_has(_old_entry->state, RDE_HASH_MAP_STATE_DELETED)) {                                                     \
                continue;                                                                                                               \
            }                                                                                                                           \
            typeof((_hash_map)->entries) _new_entry = NULL;                                                                             \
            rde_hash_map_find_whole_entry((_hash_map), &_old_entry->key, _new_entry);                                                   \
            *_new_entry = *_old_entry;                                                                                                  \
        }                                                                                                                               \
        free(_old_entries);                                                                                                             \
    } while(0)

#define rde_hash_map_find_key(_hash_map, _ref_ptr_key, _out_ptr_value)              \
    do {                                                                            \
        bool _found = false;                                                        \
        rde_hash_map_try_find_key(_hash_map, _ref_ptr_key, _found, _out_ptr_value); \
        rde_critical_error(!_found, "Tried to get a non existen key\n");            \
    } while(0)

#define rde_hash_map_try_find_key(_hash_map, _ptr_key, _out_found, _out_ptr_value)  \
    do {                                                                            \
		rde_critical_error(_hash_map == NULL, "Tried to use a NULL _hash_map\n");	\
        uint _index = 0;                                                            \
        rde_hash_map_hash_to_index((_hash_map), *(_ptr_key), _index);               \
                                                                                    \
        for(uint _i = 0; _i < (_hash_map)->actual_size; _i++) {                     \
            typeof((_hash_map)->entries) _entry = &(_hash_map)->entries[_index];    \
            if(rde_util_flag_has(_entry->state, RDE_HASH_MAP_STATE_EMPTY) ||        \
                rde_util_flag_has(_entry->state, RDE_HASH_MAP_STATE_DELETED)) {     \
                _out_found = false;                                                 \
                _out_ptr_value = NULL;                                              \
                break;                                                              \
            }                                                                       \
            if((_hash_map)->cmp_fn((_ptr_key), &(_entry->key)) == 0) {              \
                _out_found = true;                                                  \
                _out_ptr_value = &(_entry->value);                                  \
                break;                                                              \
            }                                                                       \
            _index = (_index + 1) & ((_hash_map)->actual_size - 1);                 \
        }                                                                           \
    } while(0)

#define rde_hash_map_add_entry(_hash_map, _key, _value)                     		\
    do {                                                                    		\
		rde_critical_error(_hash_map == NULL, "Tried to use a NULL _hash_map\n");	\
        uint _table_size = 0;                                               		\
        rde_hash_map_get_ideal_size(_hash_map, _table_size);                		\
        if(_table_size > (_hash_map)->actual_size) {                        		\
            rde_hash_map_resize_and_rehash(_hash_map, _table_size);         		\
        }                                                                   		\
                                                                            		\
        uint _index = 0;                                                    		\
        rde_hash_map_hash_to_index(_hash_map, _key, _index);                		\
                                                                            		\
        typeof((_hash_map)->entries) _entry = &(_hash_map)->entries[_index];		\
        while(!rde_util_flag_has(_entry->state, RDE_HASH_MAP_STATE_EMPTY)) {		\
            _index = (_index + 1) & ((_hash_map)->actual_size - 1);         		\
            _entry = &(_hash_map)->entries[_index];                         		\
        }                                                                   		\
                                                                            		\
        _entry->key = _key;                                                 		\
        _entry->value = _value;                                             		\
        _entry->state = RDE_HASH_MAP_STATE_USED;                            		\
                                                                            		\
        (_hash_map)->used++;                                                		\
    } while(0)

#define rde_hash_map_has_key(_hash_map, _ptr_key, _out_value)                   	\
    do {                                                                        	\
		rde_critical_error(_hash_map == NULL, "Tried to use a NULL _hash_map\n");	\
        uint _index = 0;                                                        	\
        rde_hash_map_hash_to_index((_hash_map), *(_ptr_key), _index);           	\
                                                                                	\
        for(uint _i = 0; _i < (_hash_map)->actual_size; _i++) {                 	\
            typeof((_hash_map)->entries) _entry = &(_hash_map)->entries[_index];	\
            if(rde_util_flag_has(_entry->state, RDE_HASH_MAP_STATE_EMPTY)) {    	\
                _out_value = false;                                             	\
                break;                                                          	\
            }                                                                   	\
            if((_hash_map)->cmp_fn((_key), &(_entry->key)) == 0) {              	\
                _out_value = true;                                              	\
                break;                                                          	\
            }                                                                   	\
            _index = (_index + 1) & ((_hash_map)->actual_size - 1);             	\
        }                                                                       	\
        _out_value = false;                                                     	\
    } while(0)

#define rde_hash_map_remove_entry(_hash_map, _ptr_key)                                  \
    do {                                                                                \
		rde_critical_error(_hash_map == NULL, "Tried to use a NULL _hash_map\n");		\
        typeof((_hash_map)->entries) _r_entry = NULL;                                   \
        rde_hash_map_find_whole_entry(_hash_map, _ptr_key, _r_entry);                   \
        uint _removed_index = _r_entry - (_hash_map)->entries;                          \
        (_hash_map)->used--;                                                            \
        uint _index = (_removed_index + 1) & ((_hash_map)->actual_size - 1);            \
                                                                                        \
        for(uint _i = 0; _i < (_hash_map)->actual_size; _i++) {                         \
            typeof((_hash_map)->entries) _entry = &((_hash_map)->entries[_index]);      \
            if(rde_util_flag_has(_entry->state, RDE_HASH_MAP_STATE_EMPTY) ||            \
                rde_util_flag_has(_entry->state, RDE_HASH_MAP_STATE_DELETED)) {         \
                break;                                                                  \
            }                                                                           \
                                                                                        \
            uint _entry_index = 0;                                                      \
            rde_hash_map_hash_to_index((_hash_map), _entry->key, _entry_index);         \
            uint _a = (_index - _entry_index) & ((_hash_map)->actual_size - 1);         \
            uint _b = (_removed_index - _entry_index) & ((_hash_map)->actual_size - 1); \
            if(_a > _b) {                                                               \
                *_r_entry = *_entry;                                                    \
                _removed_index = _index;                                                \
                _r_entry = _entry;                                                      \
            }                                                                           \
            _index = (_index + 1) & ((_hash_map)->actual_size - 1);                     \
        }                                                                               \
        _r_entry->state = RDE_HASH_MAP_STATE_DELETED;                                   \
    } while(0)
    
#define rde_hash_map_foreach(_hash_map, _out_ptr_key, _out_ptr_value, _block_of_code)   \
    do {                                                                                \
		rde_critical_error(_hash_map == NULL, "Tried to use a NULL _hash_map\n");		\
        for(uint _i = 0; _i < (_hash_map)->actual_size; _i++) {                         \
            typeof((_hash_map)->entries) _entry = &((_hash_map)->entries[_i]);          \
            if(!rde_util_flag_has(_entry->state, RDE_HASH_MAP_STATE_USED)) continue;    \
            (_out_ptr_key) = &(_entry->key);                                            \
            (_out_ptr_value) = &(_entry->value);                                        \
            _block_of_code                                                              \
        }                                                                               \
    } while(0)
	
// Macro: rde_str_new
// Creates a new dynamic string, allocating necessary data. This needs to be freed with <rde_str_free> once it is not longer needed.
//
// Parameters:
//	_rde_str - a rde_str variable.
//	_init_src_value - initial string value for the rde_str. Can be NULL.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	=================
#define rde_str_new(_rde_str, _init_src_value)											\
	do {																				\
		rde_critical_error(_rde_str != NULL, "rde_str_new -> Tried to allocate to _rde_str, but it src was not NULL, check if _rde_str.src has been initialized to NULL, otherwise it may be pointing to already allocated memory and cause a memory leak. \n"); \
																						\
		_rde_str->size = MAX(strlen(_init_src_value) + 1, RDE_STR_DEFAULT_SIZE);		\
		rde_calloc(_rde_str.src, char, _s->size);										\
		if(_init_src_value != NULL) {													\
			rde_strcpy(_rde_str.str, _rde_str.size, _init_src_value);					\
		}																				\
	} while(0)

// Macro: rde_str_resize
// Resized an existing rde_str, usually end-users won't need to call this.
//
// Parameters:
//	_rde_str - a rde_str variable.
//	_new_size - the new size to give to the rde_str.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	rde_str_resize(_str, 200);
//	=================
#define rde_str_resize(_rde_str, _new_size)												\
	do {																				\
		if(_new_size <= _rde_str.size) {												\
			break;																		\
		}																				\
		uint _old_size = _rde_str.size;													\
		while(_rde_str.size < _new_size) {												\
			_rde_str.size *= 2;															\
		}																				\
		rde_realloc(_rde_str.src, _rde_str.src, char, _rde_str.size);					\
		memset((void*)(_rde_str->str + _old_size), '\0', _rde_str->size - _old_size);	\
	} while(0)

// Macro: rde_str_append_str
// Appends a string to an existing rde_str.
//
// Parameters:
//	_rde_str - a rde_str variable.
//	_str - string to append.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	rde_str_append_str(_str, " And other animals!");
//	=================
#define rde_str_append_str(_rde_str, _str)																		\
	do {																										\
		rde_critical_error(_rde_str.src == NULL, "rde_str_append_str -> Tried to use a NULL _rde_str.src.\n");	\
		rde_critical_error(_str == NULL, "rde_str_append_str -> Tried to use a NULL _str in .\n");				\
		uint _rde_str_size = rde_str_size(_rde_str_size);														\
		rde_str_resize(_rde_str, _rde_str_size + strlen(_str));													\
		for(uint _i = 0; _i < strlen(_str); _i++) {																\
			_rde_str.str[_rde_str_size + _i] = _str[_i];														\
		}																										\
	} while(0)
	
// Macro: rde_str_append_rde_str
// Same as <rde_str_append_str> but appends a rde_str.
//
// Parameters:
//	_rde_str - a rde_str variable.
//	_other_rde_str - rde_str to append.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	rde_str _str_1 = {0};
//	rde_str_new(_str_1, "And other animals!");
//	rde_str_append_rde_rde_str(_str, _str_1);
//	=================
#define rde_str_append_rde_str(_rde_str, _other_rde_str) rde_str_append_str(_rde_str, _other_rde_str.src)

// Macro: rde_str_to_char_ptr
// Gets the inner char* of the rde_str.
//
// Parameters:
//	_rde_str - a rde_str variable.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	rde_log_level(RDE_LOG_LEVEL_INFO, "%s", rde_str_get_char_ptr(_str));
//	=================
#define rde_str_to_char_ptr(_rde_str) _rde_str.src

// Macro: rde_str_total_size
// Gets the full size of the char array, with used and unused parts.
//
// Parameters:
//	_rde_str - a rde_str variable.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	uint _size = rde_str_total_size(_str);
//	=================
#define rde_str_total_size(_rde_str) _rde_str.size
	
// Macro: rde_str_size
// Gets the size of the used part of the char array.
//
// Parameters:
//	_rde_str - a rde_str variable.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	uint _size = rde_str_total_size(_str);
//	=================
#define rde_str_size(_rde_str) strlen(_rde_str.src)
	
// Macro: rde_str_replace_sub_str
// Replaces a given string with another given string.
//
// Parameters:
//	_rde_str - a rde_str variable.
//	_old_str - string to be replaced.
//	_new_str - string that will replace _old_str.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	rde_str_replace_sub_str(_str, "Hello", "Bye");
//	=================
#define rde_str_replace_sub_str(_rde_str, _old_str, _new_str) 									\
	do {																						\
		char* _sub = rde_util_string_replace_sub_str(_rde_str.str, _old_str, _new_str, NULL);	\
		rde_critical_error(_sub == NULL, "rde_str_replace_sub_str -> error during replace \n");	\
		rde_str_free(_rde_str);																	\
		_rde_str.str = _sub;																	\
		uint _new_size = rde_str_len(_rde_str);													\
		if(_new_size > _rde_str.size) {															\
			_rde_str.size = _new_size;															\
		}																						\
	} while(0)

// Macro: rde_str_clear
// Clears inner char array, but does not touch the size or allocated memory.
//
// Parameters:
//	_rde_str - a rde_str variable.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	rde_str_clear(_str);
//	=================	
#define rde_str_clear(_rde_str)					\
	do {										\
		memset(_rde_str.str, 0, _rde_str.size);	\
	} while(0)
	
// Macro: rde_str_free
// Frees allocated memory and resets size to 0.
//
// Parameters:
//	_rde_str - a rde_str variable.
//
//	======= C =======
//	rde_str _str = {0};
//	rde_str_new(_str, "Hello, Duck!");
//	rde_str_free(_str);
//	=================
#define rde_str_free(_rde_str)		\
	do {							\
		_rde_str.size = 0;			\
		rde_free(_rde_str.str);		\
	} while(0)
	
// 											==============================================================================
// 											=									ENUMS					 	   	 		 =
// 											==============================================================================

// =================================================================== STATE ===================================================================

// Group: Enums
	
// Enum: RDE_INPUT_STATUS_
// Represents the status of some kind of input, it can be from a keyboard, mouse, controller, touch screen...
//
// RDE_INPUT_STATUS_ERROR 			-	State of error.
// RDE_INPUT_STATUS_UNINITIALIZED	-	Initial state, means it has not been used yet.
// RDE_INPUT_STATUS_JUST_PRESSED	-	It has been pressed, it only lasts one frame.
// RDE_INPUT_STATUS_KEEP_PRESSED	-	It is being pressed, last as many frames as the input is pressed.
// RDE_INPUT_STATUS_JUST_RELEASED	-	Has just been released, it only lasts one frame.
// RDE_INPUT_STATUS_KEEP_RELEASED	-	It is released, last as many frames as the input is not pressed again.
typedef enum {
	RDE_INPUT_STATUS_ERROR 			=-1,
	RDE_INPUT_STATUS_UNINITIALIZED 	= 0,
	RDE_INPUT_STATUS_JUST_PRESSED 	= 1,
	RDE_INPUT_STATUS_KEEP_PRESSED 	= 2,
	RDE_INPUT_STATUS_JUST_RELEASED 	= 3,
	RDE_INPUT_STATUS_KEEP_RELEASED 	= 4,
} RDE_INPUT_STATUS_;

// =================================================================== EVENTS ===================================================================

// Enum: RDE_EVENT_TYPE_
// Represents all of the usable events of the engine.
//
// RDE_EVENT_TYPE_NONE - Empty event, should never happen.
// RDE_EVENT_TYPE_WINDOW_BEGIN - This element should not be used as an actual event, it is used to indicate where in the enum the Window events start.
// RDE_EVENT_TYPE_WINDOW_CLOSED - Indicates a windows has been closed.
// RDE_EVENT_TYPE_WINDOW_RESIZED - Indicates a window has been resized.
// RDE_EVENT_TYPE_WINDOW_MOVED - Indicates a window has been moved.
// RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED - Indicates the mouse has entered a window.
// RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED - Indicates the mouse has exited a window.
// RDE_EVENT_TYPE_WINDOW_FOCUSED - Indicates a window has gain focus.
// RDE_EVENT_TYPE_WINDOW_UNFOCUSED - Indicates a window has lost focus.
// RDE_EVENT_TYPE_WINDOW_MINIMIZED - Indicates a window has been minimized.
// RDE_EVENT_TYPE_WINDOW_MAXIMIZED - Indicates a window has been maximized.
// RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED - Indicates a window has changed the display it was being rendered previously.
// RDE_EVENT_TYPE_WINDOW_END - This element should not be used as an actual event, it is used to indicate where in the enum the Window events ends.
// RDE_EVENT_TYPE_DISPLAY_BEGIN - This element should not be used as an actual event, it is used to indicate where in the enum the Display events start.
// RDE_EVENT_TYPE_DISPLAY_CONNECTED - Indicates a new display has been contected.
// RDE_EVENT_TYPE_DISPLAY_DISCONNECTED - Indicates a display previously connected has been disconnected.
// RDE_EVENT_TYPE_DISPLAY_CHANGED_ORIENTATION - Indicates a connected display has changed the orientation.
// RDE_EVENT_TYPE_DISPLAY_END - This element should not be used as an actual event, it is used to indicate where in the enum the Display events ends.
// RDE_EVENT_TYPE_GAME_FRAME_BEGIN - This element should not be used as an actual event, it is used to indicate where in the enum the Game Frame events start.
// RDE_EVENT_TYPE_GAME_FRAME - Indicates a frame has passed on the game.
// RDE_EVENT_TYPE_GAME_UPDATE - Indicates that the update is happening.
// RDE_EVENT_TYPE_GAME_RENDER - Indicates that the render is happening.
// RDE_EVENT_TYPE_GAME_FRAME_END - This element should not be used as an actual event, it is used to indicate where in the enum the Game Frame events end.
// RDE_EVENT_TYPE_KEY_BEGIN - This element should not be used as an actual event, it is used to indicate where in the enum the Keyboard events start.
// RDE_EVENT_TYPE_KEY_PRESSED - Indicates a keyboard key is pressed.
// RDE_EVENT_TYPE_KEY_RELEASED - Indicates a keyboard key is released.
// RDE_EVENT_TYPE_KEY_TYPED - Indicates a keyboard key is pressed and gives information about the data of the key.
// RDE_EVENT_TYPE_TEXT_TYPED - Indicates a keyboard key is pressed and gives information about the text being written.
// RDE_EVENT_TYPE_KEY_END - This element should not be used as an actual event, it is used to indicate where in the enum the Keyboard events end.
// RDE_EVENT_TYPE_MOUSE_BEGIN - This element should not be used as an actual event, it is used to indicate where in the enum the Mouse events start.
// RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED - Indicates a mouse button is pressed.
// RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED - Indicates a mouse button is released.
// RDE_EVENT_TYPE_MOUSE_SCROLLED - Indicates the mouse scroll has been scrolled.
// RDE_EVENT_TYPE_MOUSE_MOVED - Indicates the mouse has been moved.
// RDE_EVENT_TYPE_MOUSE_END - This element should not be used as an actual event, it is used to indicate where in the enum the Mouse events end.
// RDE_EVENT_TYPE_DRAG_AND_DROP_BEGIN - This element should not be used as an actual event, it is used to indicate where in the enum the Drag&Drop events star.
// RDE_EVENT_TYPE_DRAG_AND_DROP_FILE - Indicates a file has been dragged and droped onto a window.
// RDE_EVENT_TYPE_DRAG_AND_DROP_END - This element should not be used as an actual event, it is used to indicate where in the enum the Drag&Drop events end.
// RDE_EVENT_TYPE_CONTROLLER_BEGIN - This element should not be used as an actual event, it is used to indicate where in the enum the Controller events start.
// RDE_EVENT_TYPE_CONTROLLER_AXIS_MOVED - Indicates a controller joystick has moved.
// RDE_EVENT_TYPE_CONTROLLER_BUTTON_DOWN - Indicates a controller button is pressed.
// RDE_EVENT_TYPE_CONTROLLER_BUTTON_UP - Indicates a contrller button is released.
// RDE_EVENT_TYPE_CONTROLLER_END - This element should not be used as an actual event, it is used to indicate where in the enum the Controller events end.
// RDE_EVENT_TYPE_MOBILE_BEGIN - This element should not be used as an actual event, it is used to indicate where in the enum the Mobile events start.
// RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN - Indicates a finger has been layed down on the screen.
// RDE_EVENT_TYPE_MOBILE_TOUCH_UP - Indicates a finger has been released from the screen.
// RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED - Indicates a finger previously layed down, has been moved.
// RDE_EVENT_TYPE_MOBILE_MULTI_TOUCH - Indicates multiple fingers are being used on the screen.
// RDE_EVENT_TYPE_MOBILE_DOLLAR_GESTURE - Indicates a gesture with a finger on the screen.
// RDE_EVENT_TYPE_MOBILE_DOLLAR_RECORD - Indicates a gesture is being recorded with a finger on the screen.
// RDE_EVENT_TYPE_MOBILE_TERMINATING - Indicates that the application is being closed (naturally or by error).
// RDE_EVENT_TYPE_MOBILE_LOW_MEMORY - Indicates the device is low on memory.
// RDE_EVENT_TYPE_MOBILE_WILL_ENTER_BACKGROUND - Indicates on Android this corresponds to onPause, on iOS to applicationWillResignActive.
// RDE_EVENT_TYPE_MOBILE_DID_ENTER_BACKGROUND - Indicates on Android this corresponds to onPause, on iOS to applicationDidEnterBackground.
// RDE_EVENT_TYPE_MOBILE_WILL_ENTER_FOREGROUND - Indicates on Android this corresponds to onResume, on iOS to applicationWillEnterForeground.
// RDE_EVENT_TYPE_MOBILE_DID_ENTER_FOREGROUND - Indicates on Android this corresponds to onResume, on iOS to applicationDidBecomeActive.
// RDE_EVENT_TYPE_MOBILE_LOCALE_CHANGED - Indicates the language of the device has changed.
// RDE_EVENT_TYPE_MOBILE_END - This element should not be used as an actual event, it is used to indicate where in the enum the Mobile events end.
typedef enum {
	RDE_EVENT_TYPE_NONE, 
	
	RDE_EVENT_TYPE_WINDOW_BEGIN,
	RDE_EVENT_TYPE_WINDOW_CLOSED, RDE_EVENT_TYPE_WINDOW_RESIZED, RDE_EVENT_TYPE_WINDOW_MOVED, RDE_EVENT_TYPE_WINDOW_MOUSE_FOCUSED, RDE_EVENT_TYPE_WINDOW_MOUSE_UNFOCUSED,
	RDE_EVENT_TYPE_WINDOW_FOCUSED, RDE_EVENT_TYPE_WINDOW_UNFOCUSED, RDE_EVENT_TYPE_WINDOW_MINIMIZED, RDE_EVENT_TYPE_WINDOW_MAXIMIZED, RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED,
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

	RDE_EVENT_TYPE_DRAG_AND_DROP_BEGIN,
	RDE_EVENT_TYPE_DRAG_AND_DROP_FILE,
	RDE_EVENT_TYPE_DRAG_AND_DROP_END,

	RDE_EVENT_TYPE_CONTROLLER_BEGIN,
	RDE_EVENT_TYPE_CONTROLLER_AXIS_MOVED, RDE_EVENT_TYPE_CONTROLLER_BUTTON_DOWN, RDE_EVENT_TYPE_CONTROLLER_BUTTON_UP,
	RDE_EVENT_TYPE_CONTROLLER_END,
	
	RDE_EVENT_TYPE_MOBILE_BEGIN,
	RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN, RDE_EVENT_TYPE_MOBILE_TOUCH_UP, RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED, RDE_EVENT_TYPE_MOBILE_MULTI_TOUCH, RDE_EVENT_TYPE_MOBILE_DOLLAR_GESTURE, RDE_EVENT_TYPE_MOBILE_DOLLAR_RECORD,
	RDE_EVENT_TYPE_MOBILE_TERMINATING, RDE_EVENT_TYPE_MOBILE_LOW_MEMORY, 
	RDE_EVENT_TYPE_MOBILE_WILL_ENTER_BACKGROUND,
	RDE_EVENT_TYPE_MOBILE_DID_ENTER_BACKGROUND,
	RDE_EVENT_TYPE_MOBILE_WILL_ENTER_FOREGROUND,
	RDE_EVENT_TYPE_MOBILE_DID_ENTER_FOREGROUND,
	RDE_EVENT_TYPE_MOBILE_LOCALE_CHANGED,
	RDE_EVENT_TYPE_MOBILE_END

} RDE_EVENT_TYPE_;



// =================================================================== KEYBOARD ===================================================================

// Enum: RDE_KEYBOARD_KEY_
// Registered keys that can be used on the engine, matches SDL3 keys on the values (<https://wiki.libsdl.org/SDL2/SDL_Keycode>)
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



// =================================================================== MOUSE ===================================================================

// Enum: RDE_MOUSE_BUTTON_
// Registered mouse buttons that can be used on the engine, matches SDL3 mouse buttons on the values (<https://wiki.libsdl.org/SDL3/SDL_MouseButtonEvent>)
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



// =================================================================== CONTROLLER ===================================================================

// Enum: RDE_CONTROLLER_BUTTON_
// Registered controller buttons that can be used on the engine, matches SDL3 controller buttons on the values (<https://wiki.libsdl.org/SDL2/SDL_ControllerButtonEvent>)
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

// Enum: RDE_CONTROLLER_AXIS_
// Registered controller axis that can be used on the engine, matches SDL3 controller axis on the values (<https://wiki.libsdl.org/SDL2/SDL_ControllerButtonEvent>)
typedef enum {
	RDE_CONTROLLER_AXIS_LEFT,
	RDE_CONTROLLER_AXIS_RIGHT,
	RDE_CONTROLLER_AXIS_LT,
	RDE_CONTROLLER_AXIS_RT,
	RDE_CONTROLLER_AXIS_NONE
} RDE_CONTROLLER_AXIS_;



// =================================================================== PLATFORM ===================================================================

// Enum: RDE_PLATFORM_TYPE_
// Platform the engine is currently running on.
// 
// RDE_PLATFORM_TYPE_LINUX - Runngin on Linux.
// RDE_PLATFORM_TYPE_WINDOWS - Runngin on Windows.
// RDE_PLATFORM_TYPE_MAC - Runngin on MacOS.
// RDE_PLATFORM_TYPE_ANDROID - Runngin on Android.
// RDE_PLATFORM_TYPE_IOS - Runngin on iOS/iPad.
// RDE_PLATFORM_TYPE_EMSCRIPTEN - Runngin on WASM.
// RDE_PLATFORM_TYPE_UNSUPPORTE - If this happens, it is an unsupported platform, please open an issue on the repository.
typedef enum {
	RDE_PLATFORM_TYPE_LINUX,
	RDE_PLATFORM_TYPE_WINDOWS,
	RDE_PLATFORM_TYPE_MAC,
	RDE_PLATFORM_TYPE_ANDROID,
	RDE_PLATFORM_TYPE_IOS,
	RDE_PLATFORM_TYPE_EMSCRIPTEN,
	RDE_PLATFORM_TYPE_UNSUPPORTED
} RDE_PLATFORM_TYPE_;

// Enum: RDE_INPUT_TYPE_
// Supported input methods of the engine.
//
// RDE_INPUT_TYPE_WINDOW - Input from the window.
// RDE_INPUT_TYPE_MOUSE - Input from the mouse.
// RDE_INPUT_TYPE_KEYBOARD - Input from the keyboard.
// RDE_INPUT_TYPE_CONTROLLER - Input from one or more controllers.
// RDE_INPUT_TYPE_MOBILE - Input from device touch screen.
typedef enum {
	RDE_INPUT_TYPE_WINDOW,
	RDE_INPUT_TYPE_MOUSE,
	RDE_INPUT_TYPE_KEYBOARD,
	RDE_INPUT_TYPE_CONTROLLER,
	RDE_INPUT_TYPE_MOBILE
} RDE_INPUT_TYPE_;

// Enum: RDE_BATTERY_
// Battery status from any device that has one.
//
// RDE_BATTERY_UKNOWN- Returned by desktop computers.
// RDE_BATTERY_ON_BATTERY- If the device being used with the battery and not being charged.
// RDE_BATTERY_NO_BATTERY- If the devied is beig used, connected to a cable but there is no battery on the device.
// RDE_BATTERY_CHARGING- If the devied is charging.
// RDE_BATTERY_CHARGED- If the devied is fully charged.
typedef enum {
	RDE_BATTERY_UKNOWN,
	RDE_BATTERY_ON_BATTERY,
	RDE_BATTERY_NO_BATTERY,
	RDE_BATTERY_CHARGING,
	RDE_BATTERY_CHARGED
} RDE_BATTERY_;



// =================================================================== RENDERING ===================================================================

// Enum: RDE_UI_ANCHOR_
// UI flag enum that sets how the sprites will anchor to specific parts of the screen. Can be combined with <RDE_UI_STRETCH_>
//
// RDE_UI_ANCHOR_MIDDLE - Will anchor to the center of the screen as a pivot.
// RDE_UI_ANCHOR_LEFT - Will anchor to the left side of the screen as a pivot.
// RDE_UI_ANCHOR_RIGHT - Will anchor to the right side of the screen as a pivot.
// RDE_UI_ANCHOR_TOP - Will anchor to the top side of the screen as a pivot.
// RDE_UI_ANCHOR_BOTTOM - Will anchor to the bottom side of the screen as a pivot.
typedef enum {
	RDE_UI_ANCHOR_MIDDLE              = 1 << 1,
	RDE_UI_ANCHOR_LEFT                = 1 << 2,
	RDE_UI_ANCHOR_RIGHT               = 1 << 3,
	RDE_UI_ANCHOR_TOP                 = 1 << 4,
	RDE_UI_ANCHOR_BOTTOM              = 1 << 5
} RDE_UI_ANCHOR_;

// Enum: RDE_UI_STRETCH_
// UI flag enum that sets how the sprites will scale with different screen resolutions. Can be combined with <RDE_UI_ANCHOR_>
//
// RDE_UI_STRETCH_NO_STRETCH - Will not strech.
// RDE_UI_STRETCH_VERTICAL_STRETCH - Will strech on the Y axis.
// RDE_UI_STRETCH_HORIZONTAL_STRETCH - Will strech on the X axis.
typedef enum {
	RDE_UI_STRETCH_NO_STRETCH          = 1 << 6,
	RDE_UI_STRETCH_VERTICAL_STRETCH    = 1 << 7,
	RDE_UI_STRETCH_HORIZONTAL_STRETCH  = 1 << 8
} RDE_UI_STRETCH_;

// Enum: RDE_UNIFORM_FV_
// Types of uniforms that the engine supports for floats. WASM supports less than other platforms, be aware of this.
//
// RDE_UNIFORM_FV_1 - Vector of 1 float.
// RDE_UNIFORM_FV_2 - Vector of 2 float.
// RDE_UNIFORM_FV_3 - Vector of 3 float.
// RDE_UNIFORM_FV_4 - Vector of 4 float.
// RDE_UNIFORM_FV_MATRIX_2 - Matrix 2x2.
// RDE_UNIFORM_FV_MATRIX_3 - Matrix 3x3.
// RDE_UNIFORM_FV_MATRIX_4 - Matrix 4x4.
// RDE_UNIFORM_FV_MATRIX_2x3 - Matrix 2x3. Not available on WASM.
// RDE_UNIFORM_FV_MATRIX_3x2 - Matrix 3x2. Not available on WASM.
// RDE_UNIFORM_FV_MATRIX_4x2 - Matrix 4x2. Not available on WASM.
// RDE_UNIFORM_FV_MATRIX_2x4 - Matrix 2x4. Not available on WASM.
// RDE_UNIFORM_FV_MATRIX_4x3 - Matrix 4x3. Not available on WASM.
// RDE_UNIFORM_FV_MATRIX_3x4 - Matrix 3x4. Not available on WASM.
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

// Enum: RDE_UNIFORM_IV_
// Types of uniforms that the engine supports for integers.
//
// RDE_UNIFORM_IV_1 - Vector of 1 integers.
// RDE_UNIFORM_IV_2 - Vector of 2 integers.
// RDE_UNIFORM_IV_3 - Vector of 3 integers.
// RDE_UNIFORM_IV_4 - Vector of 4 integers.
typedef enum {
	RDE_UNIFORM_IV_1,
	RDE_UNIFORM_IV_2,
	RDE_UNIFORM_IV_3,
	RDE_UNIFORM_IV_4
} RDE_UNIFORM_IV_;

// Enum: RDE_UNIFORM_UIV_
// Types of uniforms that the engine supports for unsigned integers.
//
// RDE_UNIFORM_UIV_1 - Vector of 1 unsigned integers.
// RDE_UNIFORM_UIV_2 - Vector of 2 unsigned integers.
// RDE_UNIFORM_UIV_3 - Vector of 3 unsigned integers.
// RDE_UNIFORM_UIV_4 - Vector of 4 unsigned integers.
typedef enum {
	RDE_UNIFORM_UIV_1,
	RDE_UNIFORM_UIV_2,
	RDE_UNIFORM_UIV_3,
	RDE_UNIFORM_UIV_4
} RDE_UNIFORM_UIV_;

// Enum: RDE_CAMERA_TYPE_
// Types of cameras.
//
// RDE_CAMERA_TYPE_PERSPECTIVE - Perspective camera, ideal for 3D.
// RDE_CAMERA_TYPE_ORTHOGRAPHIC - Orthographic camera, ideal for 2D.
typedef enum {
	RDE_CAMERA_TYPE_PERSPECTIVE,
	RDE_CAMERA_TYPE_ORTHOGRAPHIC
} RDE_CAMERA_TYPE_;

// Enum: RDE_ANTIALIASING_
// Levels of multi sampled anti-aliasing. Keep in mind, the more anti-aliasing, the more resources it uses.
//
// RDE_ANTIALIASING_NONE - No antialiasing will be used.
// RDE_ANTIALIASING_X2 - Anti-aliasing with 2 levels.
// RDE_ANTIALIASING_X4 - Anti-aliasing with 4 levels.
// RDE_ANTIALIASING_X8 - Anti-aliasing with 8 levels.
// RDE_ANTIALIASING_X16 - Anti-aliasing with 16 levels.
// RDE_ANTIALIASING_X32 - Anti-aliasing with 32 levels.
typedef enum {
	RDE_ANTIALIASING_NONE = 0,
	RDE_ANTIALIASING_X2 = 2,
	RDE_ANTIALIASING_X4 = 4,
	RDE_ANTIALIASING_X8 = 8,
	RDE_ANTIALIASING_X16 = 16,
	RDE_ANTIALIASING_X32 = 32,
} RDE_ANTIALIASING_;

// Enum: RDE_TEXTURE_PARAMETER_TYPE_
// Parameters for texture loading.
//
// RDE_TEXTURE_PARAMETER_TYPE_FILTER_NEAREST - Nearest filter.
// RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR - Linear filter.
// RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT - Wrap repeat.
// RDE_TEXTURE_PARAMETER_TYPE_WRAP_CLAMP_TO_BORDER - Wrap clamp to border.
// RDE_TEXTURE_PARAMETER_TYPE_WRAP_CLAMP_TO_EDGE - Wrap clamp to edge.
// RDE_TEXTURE_PARAMETER_TYPE_WRAP_MIRRORED_REPEAT - Wrap mirrored repeat.
// RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NEAREST_MIN_FILTER_NEAREST - mipmap nearest nearest.
// RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_LINEAR_MIN_FILTER_NEAREST - mipmap linear nearest.
// RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NEAREST_MIN_FILTER_LINEAR - mimap nearest linear.
// RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_LINEAR_MIN_FILTER_LINEAR - mipmap linear linear
// RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NONE - mipmap none.
typedef enum {
	RDE_TEXTURE_PARAMETER_TYPE_FILTER_NEAREST = 0x2600,
	RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR = 0x2601,
	RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT = 0x2901,
	RDE_TEXTURE_PARAMETER_TYPE_WRAP_CLAMP_TO_BORDER = 0x812D,
	RDE_TEXTURE_PARAMETER_TYPE_WRAP_CLAMP_TO_EDGE = 0x812F,
	RDE_TEXTURE_PARAMETER_TYPE_WRAP_MIRRORED_REPEAT = 0x8370,
	RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NEAREST_MIN_FILTER_NEAREST = 0x2700,
	RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_LINEAR_MIN_FILTER_NEAREST = 0x2701,
	RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NEAREST_MIN_FILTER_LINEAR = 0x2702,
	RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_LINEAR_MIN_FILTER_LINEAR = 0x2703,
	RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_NONE = 0x2704,
} RDE_TEXTURE_PARAMETER_TYPE_;

// =================================================================== FILE SYSTEM ===================================================================

// Enum: RDE_FILE_MODE_
// Mode to open a file.
//
// RDE_FILE_MODE_READ - Read only file.
// RDE_FILE_MODE_WRITE - Write only file.
// RDE_FILE_MODE_APPEND - Write at end of file only.
// RDE_FILE_MODE_READ_AND_WRITE - Read and write.
// RDE_FILE_MODE_READ_AND_APPEND - Read and append.
typedef enum {
	RDE_FILE_MODE_READ,
	RDE_FILE_MODE_WRITE,
	RDE_FILE_MODE_APPEND,
	RDE_FILE_MODE_READ_AND_WRITE,
	RDE_FILE_MODE_READ_AND_APPEND,
} RDE_FILE_MODE_;

// =================================================================== LOG ===================================================================

// Enum: RDE_LOG_COLOR_
// Colors for printing to the console.
//
// RDE_LOG_COLOR_RED - Red.
// RDE_LOG_COLOR_GREEN - Green.
// RDE_LOG_COLOR_YELLOW - Yellow.
// RDE_LOG_COLOR_BLUE - Blue.
// RDE_LOG_COLOR_PURPLE - Purple.
// RDE_LOG_COLOR_CYAN - Cyan.
// RDE_LOG_COLOR_WHITE - White.
typedef enum { 
	RDE_LOG_COLOR_RED, 
	RDE_LOG_COLOR_GREEN, 
	RDE_LOG_COLOR_YELLOW, 
	RDE_LOG_COLOR_BLUE, 
	RDE_LOG_COLOR_PURPLE, 
	RDE_LOG_COLOR_CYAN, 
	RDE_LOG_COLOR_WHITE,
} RDE_LOG_COLOR_;

// Enum: RDE_LOG_LEVEL_
// Levels of severity to print to the console.
//
// RDE_LOG_LEVEL_DEBUG - Debug.
// RDE_LOG_LEVEL_INFO - Info.
// RDE_LOG_LEVEL_WARNING - Warning.
// RDE_LOG_LEVEL_ERROR - Error.
typedef enum { 
	RDE_LOG_LEVEL_DEBUG,
	RDE_LOG_LEVEL_INFO,
	RDE_LOG_LEVEL_WARNING,
	RDE_LOG_LEVEL_ERROR,
} RDE_LOG_LEVEL_;


// =================================================================== PHYSICS 3D ===================================================================

// Enum: RDE_PHYSICS_BODY_TYPE_
// Type of phisic bodies.
//
// RDE_PHYSICS_BODY_TYPE_STATIC - Static.
// RDE_PHYSICS_BODY_TYPE_KINEMATIC - Kinematic.
// RDE_PHYSICS_BODY_TYPE_DYNAMIC - Dynamic.
typedef enum {
	RDE_PHYSICS_BODY_TYPE_STATIC,
	RDE_PHYSICS_BODY_TYPE_KINEMATIC,
	RDE_PHYSICS_BODY_TYPE_DYNAMIC,
} RDE_PHYSICS_BODY_TYPE_;

// Enum: RDE_PHYSICS_SHAPE_TYPE_
// Types of shapes for the colliders.
//
// RDE_PHYSICS_SHAPE_TYPE_NONE - None.
// RDE_PHYSICS_SHAPE_TYPE_BOX - Box.
// RDE_PHYSICS_SHAPE_TYPE_SPHERE - Sphere.
// RDE_PHYSICS_SHAPE_TYPE_CYLINDER - Cylinder.
// RDE_PHYSICS_SHAPE_TYPE_CAPSULE - Capsule.
// RDE_PHYSICS_SHAPE_TYPE_PYRAMID - Pyramid.
// RDE_PHYSICS_SHAPE_TYPE_MESH - Mesh.
typedef enum {
	RDE_PHYSICS_SHAPE_TYPE_NONE,
	RDE_PHYSICS_SHAPE_TYPE_BOX,
	RDE_PHYSICS_SHAPE_TYPE_SPHERE,
	RDE_PHYSICS_SHAPE_TYPE_CYLINDER,
	RDE_PHYSICS_SHAPE_TYPE_CAPSULE,
	RDE_PHYSICS_SHAPE_TYPE_PYRAMID,
	RDE_PHYSICS_SHAPE_TYPE_MESH
} RDE_PHYSICS_SHAPE_TYPE_;

// 											==============================================================================
// 											=									STRUCTS					 	   	 		 =
// 											==============================================================================

// Group: Structs & Callbacks

// =================================================================== MATH ===================================================================

// Type: rde_vec_2I
// Vector2 of int. See <RDE_SPECIALIZED_VEC2>.
RDE_SPECIALIZED_VEC2(int, 			rde_vec_2I);

// Type: rde_vec_2UI
// Vector2 of unsigned int. See <RDE_SPECIALIZED_VEC2>.
RDE_SPECIALIZED_VEC2(uint, 			rde_vec_2UI);

// Type: rde_vec_2F
// Vector2 of float. See <RDE_SPECIALIZED_VEC2>.
RDE_SPECIALIZED_VEC2(float, 		rde_vec_2F);

// Type: rde_vec_2D
// Vector2 of double. See <RDE_SPECIALIZED_VEC2>.
RDE_SPECIALIZED_VEC2(double,		rde_vec_2D);

// Type: rde_vec_2L
// Vector2 of long. See <RDE_SPECIALIZED_VEC2>.
RDE_SPECIALIZED_VEC2(long,			rde_vec_2L);

// Type: rde_vec_2UL
// Vector2 of unsigned long. See <RDE_SPECIALIZED_VEC2>.
RDE_SPECIALIZED_VEC2(unsigned long, rde_vec_2UL);

// Type: rde_vec_2ST
// Vector2 of size_t. See <RDE_SPECIALIZED_VEC2>.
RDE_SPECIALIZED_VEC2(size_t, 		rde_vec_2ST);

// Type: rde_vec_3I
// Vector3 of int. See <RDE_SPECIALIZED_VEC3>.
RDE_SPECIALIZED_VEC3(int, 			rde_vec_3I);

// Type: rde_vec_3UI
// Vector3 of uint. See <RDE_SPECIALIZED_VEC3>.
RDE_SPECIALIZED_VEC3(uint, 			rde_vec_3UI);

// Type: rde_vec_3F
// Vector3 of float. See <RDE_SPECIALIZED_VEC3>.
RDE_SPECIALIZED_VEC3(float, 		rde_vec_3F);

// Type: rde_vec_3D
// Vector3 of double. See <RDE_SPECIALIZED_VEC3>.
RDE_SPECIALIZED_VEC3(double,		rde_vec_3D);

// Type: rde_vec_3L
// Vector3 of long. See <RDE_SPECIALIZED_VEC3>.
RDE_SPECIALIZED_VEC3(long,			rde_vec_3L);

// Type: rde_vec_3UL
// Vector3 of unsigned long. See <RDE_SPECIALIZED_VEC3>.
RDE_SPECIALIZED_VEC3(unsigned long, rde_vec_3UL);

// Type: rde_vec_3ST
// Vector3 of size_t. See <RDE_SPECIALIZED_VEC3>.
RDE_SPECIALIZED_VEC3(size_t, 		rde_vec_3ST);

// Type: rde_vec_4I
// Vector4 of int. See <RDE_SPECIALIZED_VEC4>.
RDE_SPECIALIZED_VEC4(int, 			rde_vec_4I);

// Type: rde_vec_4UI
// Vector4 of uint. See <RDE_SPECIALIZED_VEC4>.
RDE_SPECIALIZED_VEC4(uint, 			rde_vec_4UI);

// Type: rde_vec_4F
// Vector4 of float. See <RDE_SPECIALIZED_VEC4>.
RDE_SPECIALIZED_VEC4(float, 		rde_vec_4F);

// Type: rde_vec_4D
// Vector4 of double. See <RDE_SPECIALIZED_VEC4>.
RDE_SPECIALIZED_VEC4(double,		rde_vec_4D);

// Type: rde_vec_4L
// Vector4 of long. See <RDE_SPECIALIZED_VEC4>.
RDE_SPECIALIZED_VEC4(long,			rde_vec_4L);

// Type: rde_vec_4UL
// Vector4 of unsigned long. See <RDE_SPECIALIZED_VEC4>.
RDE_SPECIALIZED_VEC4(unsigned long, rde_vec_4UL);

// Type: rde_vec_4ST
// Vector4 of size_t. See <RDE_SPECIALIZED_VEC4>.
RDE_SPECIALIZED_VEC4(size_t, 		rde_vec_4ST);

// Type: rde_mat_2I
// Matrix 2x2 of int. See <RDE_SPECIALIZED_MAT2>.
RDE_SPECIALIZED_MAT2(int, 			rde_mat_2I);

// Type: rde_mat_2UI
// Matrix 2x2 of uint. See <RDE_SPECIALIZED_MAT2>.
RDE_SPECIALIZED_MAT2(uint, 			rde_mat_2UI);

// Type: rde_mat_2F
// Matrix 2x2 of float. See <RDE_SPECIALIZED_MAT2>.
RDE_SPECIALIZED_MAT2(float, 		rde_mat_2F);

// Type: rde_mat_2D
// Matrix 2x2 of double. See <RDE_SPECIALIZED_MAT2>.
RDE_SPECIALIZED_MAT2(double,		rde_mat_2D);

// Type: rde_mat_2L
// Matrix 2x2 of long. See <RDE_SPECIALIZED_MAT2>.
RDE_SPECIALIZED_MAT2(long,			rde_mat_2L);

// Type: rde_mat_2UL
// Matrix 2x2 of unsigned long. See <RDE_SPECIALIZED_MAT2>.
RDE_SPECIALIZED_MAT2(unsigned long, rde_mat_2UL);

// Type: rde_mat_2ST
// Matrix 2x2 of size_t. See <RDE_SPECIALIZED_MAT2>.
RDE_SPECIALIZED_MAT2(size_t, 		rde_mat_2ST);

// Type: rde_mat_3I
// Matrix 3x3 of int. See <RDE_SPECIALIZED_MAT3>.
RDE_SPECIALIZED_MAT3(int, 			rde_mat_3I);

// Type: rde_mat_3UI
// Matrix 3x3 of uint. See <RDE_SPECIALIZED_MAT3>.
RDE_SPECIALIZED_MAT3(uint, 			rde_mat_3UI);

// Type: rde_mat_3F
// Matrix 3x3 of float. See <RDE_SPECIALIZED_MAT3>.
RDE_SPECIALIZED_MAT3(float, 		rde_mat_3F);

// Type: rde_mat_3D
// Matrix 3x3 of double. See <RDE_SPECIALIZED_MAT3>.
RDE_SPECIALIZED_MAT3(double,		rde_mat_3D);

// Type: rde_mat_3L
// Matrix 3x3 of long. See <RDE_SPECIALIZED_MAT3>.
RDE_SPECIALIZED_MAT3(long,			rde_mat_3L);

// Type: rde_mat_3UL
// Matrix 3x3 of unsigned long. See <RDE_SPECIALIZED_MAT3>.
RDE_SPECIALIZED_MAT3(unsigned long, rde_mat_3UL);

// Type: rde_mat_3ST
// Matrix 3x3 of size_t. See <RDE_SPECIALIZED_MAT3>.
RDE_SPECIALIZED_MAT3(size_t, 		rde_mat_3ST);

// Type: rde_mat_4I
// Matrix 4x4 of int. See <RDE_SPECIALIZED_MAT4>.
RDE_SPECIALIZED_MAT4(int, 			rde_mat_4I);

// Type: rde_mat_4UI
// Matrix 4x4 of uint. See <RDE_SPECIALIZED_MAT4>.
RDE_SPECIALIZED_MAT4(uint, 			rde_mat_4UI);

// Type: rde_mat_4F
// Matrix 4x4 of float. See <RDE_SPECIALIZED_MAT4>.
RDE_SPECIALIZED_MAT4(float, 		rde_mat_4F);

// Type: rde_mat_4D
// Matrix 4x4 of double. See <RDE_SPECIALIZED_MAT4>.
RDE_SPECIALIZED_MAT3(double,		rde_mat_4D);

// Type: rde_mat_4L
// Matrix 4x4 of long. See <RDE_SPECIALIZED_MAT4>.
RDE_SPECIALIZED_MAT4(long,			rde_mat_4L);

// Type: rde_mat_4UL
// Matrix 4x4 of unsigned long. See <RDE_SPECIALIZED_MAT4>.
RDE_SPECIALIZED_MAT4(unsigned long, rde_mat_4UL);

// Type: rde_mat_4ST
// Matrix 4x4 of size_t. See <RDE_SPECIALIZED_MAT4>.
RDE_SPECIALIZED_MAT4(size_t, 		rde_mat_4ST);

// Type: rde_quaternion
// Represents a mathematical quaternion for rotations.
//
// Fields:
// 	x - (float) x rotation value.
// 	y - (float) y rotation value.
// 	z - (float) z rotation value.
// 	w - (float) w rotation value. This is the real part.
typedef struct rde_quaternion rde_quaternion;
struct rde_quaternion {
	float x;
	float y;
	float z;
	float w;
};

// Type: rde_probability
// Represents a mathematical probability.
//
// Fields:
// 	probability_rolled - (float) value in the range [0, 1].
// 	happened - (bool) if the probability_rolled was in the range of the chosen probability.
typedef struct rde_probability rde_probability;
struct rde_probability {
	float probability_rolled;
	bool happened;
};

// =================================================================== ENGINE ===================================================================

// Type: rde_engine
// Struct of the whole engine. This is just a pointer, implementation is in the source file. 
typedef struct rde_engine rde_engine;

// Type: rde_window
// Represents a window drawn on the screen. This is just a pointer, implementation is in the source file. 
typedef struct rde_window rde_window;

// Type: rde_file_handle
// Represents an open file pointer handle. It is needed for any file system operation. This is just a pointer, implementation is in the source file. 
typedef struct rde_file_handle rde_file_handle;

// Type: rde_transform
// Represents the position, rotation and scale of an element. It also contains infomation about the children and parent. This is just a pointer, implementation is in the source file. 
typedef struct rde_transform rde_transform;

// Type: rde_shader
// Represents a shader, containing important OpenGL data. This is just a pointer, implementation is in the source file.
typedef struct rde_shader rde_shader;

// Type: rde_atlas
// Represents a sprite sheet, and contains OpenGL data and info about the sprite sheet. This is just a pointer, implementation is in the source file.
typedef struct rde_atlas rde_atlas;

// Type: rde_font
// Represents a font sprite sheet, and contains OpenGL data and info about the font. This is just a pointer, implementation is in the source file.
typedef struct rde_font rde_font;

// Type: rde_render_texture
// Represents a special type of texture. We can enable it while rendering our scene and the scene will be rendered onto this render texture, then we can render the render texture wherever is needed. This is just a pointer, implementation is in the source file.
typedef struct rde_render_texture rde_render_texture;

// Type: rde_texture
// Represents a texture loaded from an image, and contains OpenGL data and info about the image. This is just a pointer, implementation is in the source file.
typedef struct rde_texture rde_texture;

// Type: rde_model
// Represents a 3D model, and contains OpenGL data and info about the meshes of the model. This is just a pointer, implementation is in the source file.
typedef struct rde_model rde_model;

// Type: rde_skybox_id
// This is just an id to locate the skyboxes.
typedef uint rde_skybox_id;

// Type: rde_mesh
// Represents a mesh, may be part of a model or not. This is just a pointer, implementation is in the source file.
typedef struct rde_mesh rde_mesh;



// =================================================================== UTIL ====================================================================

// Type: rde_str
// Struct representing a heap dynamic string.
//
// Fields:
//	str - char array representing the main string.
//	size - total size of the string, representing the used and non-used parts.
typedef struct rde_str rde_str;
struct rde_str {
    char* str;
    unsigned int size;
};

// =================================================================== CAMERA ===================================================================

// Type: rde_camera
// Struct that contains the information to let the renderer know what is being seen, so it can render properly.
//
// Fields:
//	id - (uint) id of the camera, unique.
//	zoom - (float) zoom of the camera.
//	fov - (float) field of view in degrees.
//	transform - (<rde_transform>) spatial information of the camera.
//	direction - (<rde_vec_3F>) where the camera is looking to.
//	up - (<rde_vec_3F>) what axis is Up, usually (0, 1, 0).
// 	near_far - (<rde_vec_2F) near and far clip of the camera. Anything behind near or beyond far won't be rendered.
//	camera_type - (<RDE_CAMERA_TYPE_>) Orthographic (2D) or Perspective (3D.
//	enabled - (bool) if the camera is in use.
typedef struct rde_camera rde_camera;
struct rde_camera {
	uint id;
	float zoom;
	float fov;
	rde_transform* transform;
	rde_vec_3F direction;
	rde_vec_3F up;
	rde_vec_2F near_far;
	RDE_CAMERA_TYPE_ camera_type;
	bool enabled;
};

#ifdef RDE_PHYSICS_MODULE
#include "JoltC/rde_joltc.h"
#endif

#ifdef RDE_IMGUI_MODULE
#include "rde_imgui.h"
#endif

// =================================================================== EVENTS ===================================================================

// Type: rde_event_window
// Contains all info related to a window event.
//
// Fields:
//	position - (<rde_vec_2I>) where the window is located.
//	size - (<rde_vec_2I>) size of the window.
//	display_index - (int) id of the display where the window is located.
//	minimized - (bool) if the window is minimized.
typedef struct {
	rde_vec_2I position;
	rde_vec_2I size;
	int display_index;
	bool minimized;
} rde_event_window;

// Type: rde_event_display
// Contains all info related to a display event.
//
// Fields:
//	orientation - (int) orientation of the display.
//	display_index - (int) id of the display.
typedef struct {
	int orientation;
	int display_index;
} rde_event_display;

// Type: rde_event_key
// Contains all info related to a keyboard key event.
//
// Fields:
//	key - (<RDE_KEYBOARD_KEY_>) type of key.
//	typed_char (char) char associeted the the key.
//	typed_text - (char*) text chain of the typed keys.
//	amount_of_times_pressed - (int) how many times the key was pressed.
typedef struct {
	RDE_KEYBOARD_KEY_ key;
	char typed_char;
	const char* typed_text;
	int amount_of_times_pressed;
} rde_event_key;

// Type: rde_event_mouse
// Contains all info related to a mouse event.
//
// Fields:
//	position - (<rde_vec_2I>) position the mouse is at.
//	scrolled - (<rde_vec_2F>) amount of scroll with the scroll wheels.
//	button - (<RDE_MOUSE_BUTTON_>) type of mouse button.
//	amount_of_times_pressed - (int) how many times the button was pressed.
typedef struct {
	rde_vec_2I position;
	rde_vec_2F scrolled;
	RDE_MOUSE_BUTTON_ button;
	int amount_of_times_pressed;
} rde_event_mouse;

// Type: rde_event_controller
// Contains all info related to a controller event.
//
// Fields:
//	controller_id - (int) id of the controller.
//	left_joystick - (<rde_vec_2F>) position of the left joystic, in the range [-1, 1].
//	right_joystick - (<rde_vec_2F>) position of the right joystic, in the range [-1, 1].
//	button - (<RDE_CONTROLLER_BUTTON_>) type of button.
//	axis - (<RDE_CONTROLLER_AXIS_>) type of axis.
typedef struct {
	int controller_id;
	rde_vec_2F left_joystick;
	rde_vec_2F right_joystick;
	rde_vec_2F back_buttons;
	RDE_CONTROLLER_BUTTON_ button;
	RDE_CONTROLLER_AXIS_ axis;
} rde_event_controller;

// Type: rde_event_mobile_pinch
// Contains all info related to a mobile pinch event.
//
// Fields:
//	rotation_of_fingers - (float) rotation of 2 fingers, in degrees.
//	distance_moved_between_fingers - (float) distanced that 2 fingers has moved since last position.
//	num_fingers_used - (uint) number of fingers used to perform pinch.
typedef struct {
	float rotation_of_fingers;
	float distance_moved_between_fingers;
	uint num_fingers_used;
} rde_event_mobile_pinch;

// Type: rde_event_mobile
// Contains all info related to a mobile event.
//
// Fields:
//	init_touch_position - (<rde_vec_2I>) initial position of the finger.
//	moved_touch_position - (<rde_vec_2I>) amount that the finger has moved since last position.
//	end_touch_position - (<rde_vec_2I>) last position of the finger before lifting it up.
//	pressure - (float) amount of pressure used on the tap.
//	finger_id - (uint) id of the finger.
//	pinch - (<rde_event_mobile_pinch>) see rde_event_mobile_pinch.
typedef struct {
	rde_vec_2I init_touch_position;
	rde_vec_2I moved_touch_position;
	rde_vec_2I end_touch_position;
	float pressure;
	uint finger_id;
	rde_event_mobile_pinch pinch;
} rde_event_mobile;

// Type: rde_event_drag_and_drop
// Contains all info related to a drag&drop event.
//
// Fields:
//	window_id - (uint) id of the window where the drop happened.
//	file_path - (char*) path of the dropped file.
typedef struct {
	uint window_id;
	char* file_path;
} rde_event_drag_and_drop;

// Type: rde_event_data
// Contains all info related to an event.
//
// Fields:
//	window_event_data - (<rde_event_window>) see rde_event_window.
//	key_event_data - (<rde_event_key>) see rde_event_key.
//	mouse_event_data - (<rde_event_mouse>) see rde_event_mouse.
//	controller_event_data - (<rde_event_controller>) see rde_event_controller.
//	mobile_event_data - (<rde_event_mobile>) see rde_event_mobile.
//	display_event_data - (<rde_event_display>) see rde_event_display.
//	drag_and_drop_data - (<rde_event_drag_and_drop>) see rde_event_drag_and_drop.
typedef struct {
	rde_event_window window_event_data;
	rde_event_key key_event_data;
	rde_event_mouse mouse_event_data;
	rde_event_controller controller_event_data;
	rde_event_mobile mobile_event_data;
	rde_event_display display_event_data;
	rde_event_drag_and_drop drag_and_drop_data;
} rde_event_data;

// Type: rde_event
// Struct to store data of each available event.
//
// Fields:
// 	type - (<RDE_EVENT_TYPE_>) type of the event.
//	time_stamp - (ulong) when the event happened.
//	window_id - (uint) window associeted with the event.
//	handled - (bool) if other event handled it first.
//	data - (<rde_event_data>) specific data of each event. Only one of the sub-structs is filled.
//	sdl_native_event - (void*) pointer to the native SDL3 event.
typedef struct rde_event rde_event;
struct rde_event {
	RDE_EVENT_TYPE_ type;
	ulong time_stamp;
	uint window_id;
	bool handled;
	rde_event_data data;
	void* sdl_native_event;
};

// =================================================================== CALLBACKS AND FUNCTION POINTERS ===================================================================

// Callback: rde_event_func
// Function pointer (callback) for the main event function, so the user can respond to the needed events.
//
// Parameters:
//	_event - the current event to be handled.
//	_window - the window where the event happened.
typedef void (*rde_event_func)(rde_event*, rde_window*);

// Callback: rde_update_func
// Function pointer (callback) valid for update, fixed_update and late_update functions. Users need one of these callbacks per function.
//
// Parameters:
//	_dt - delta time passed between frames.
typedef void (*rde_update_func)(float);

// Callback: rde_render_func
// Function pointer (callback) valid for render function. Users need one of these callbacks per function.
//
// Parameters:
//	_dt - delta time passed between frames.
//	_window - the pointer to the window that will render the scene.
typedef void (*rde_render_func)(float, rde_window*);

// Type: rde_end_user_mandatory_callbacks
// All callbacks that end user must provide for the engine to work.
//
// Fields:
//	on_update - (<rde_update_func>) see rde_update_func.
//	on_fixed_update - (<rde_update_func>) see rde_update_func.
//	on_late_update - (<rde_update_func>) see rde_update_func.
//	on_render - (<rde_render_func>) see rde_render_func.
typedef struct {
	rde_update_func on_update;
	rde_update_func on_fixed_update;
	rde_update_func on_late_update;
	rde_render_func on_render;
} rde_end_user_mandatory_callbacks;

// =================================================================== ENGINE ===================================================================

// Type: rde_engine_heap_allocs_config
// Struct that contains values to allocate on heap when the engine is loading for the first time. In general, those are the only allocations the engine will do internally during the whole program's lifetime.
//
// Fields:
//	max_amount_of_windows - (uint) max number of windows to be instantiated.
//	max_amount_of_vertices_per_batch - (uint) max number of vertices that a drawcall can batch before sending to GPU.
//	max_amount_of_shaders - (uint) max number of shaders that the engine can load.
//	max_amount_of_textures - (uint) max number of individual textures that the engine can load.
//	max_amount_of_atlases - (uint) max number of textures atlases that the engine can load.
//	max_amount_of_fonts - (uint) max number of font textures that the engine can load.
//	max_amount_of_models - (uint) max number of 3D models that the engine can load.
//	max_amount_of_models_textures - (uint) max number of 3D model textures that the engine can load.
//	max_amount_of_sounds - (uint) This one is only used if RDE_AUDIO_MODULE is defined. Max number of sounds that the engine can load.
typedef struct rde_engine_heap_allocs_config rde_engine_heap_allocs_config;
struct rde_engine_heap_allocs_config {
	uint max_amount_of_windows;
	uint max_amount_of_vertices_per_batch;
	uint max_amount_of_shaders;
	
	uint max_amount_of_textures;
	uint max_amount_of_atlases;
	uint max_amount_of_fonts;
	uint max_amount_of_models;
	uint max_amount_of_models_textures;

#ifdef RDE_AUDIO_MODULE
	uint max_amount_of_sounds;
#endif
};

// Type: rde_illumination_config
// Defines the maximum amount of each type of light that the engine can work with.
//
// Fields:
//	max_amount_of_point_lights - (uint) max number of point lights.
//	max_amount_of_spot_lights - (uint) max number of spot lights.
typedef struct rde_illumination_config rde_illumination_config;
struct rde_illumination_config {
	uint max_amount_of_point_lights;
	uint max_amount_of_spot_lights;
};

// Type: rde_physics_3d_config
// Defines initialization info for Jolt Physics Engine.
//
// Fields:
//	temp_allocator_bytes - (uint) memory arena size so the engine does not need to allocate during runtime. Recommended at least 10-15Mb, if not set, malloc/free will be used on runtime.
//	max_amout_of_allowed_jobs - (uint) max number of parallel jobs.
//	max_amount_of_physics_barriers - (uint) max number of physical barriers.
//	max_amount_of_threads - (int) max number of threads.
//	max_amount_of_bodies - (uint) max number of bodies that can get into the simulation.
//	max_amount_of_mutexes - (uint) max number of mutexes.
//	max_amount_of_contact_constraints - (uint) max number of contact constrations.
typedef struct rde_physics_3d_config rde_physics_3d_config;
struct rde_physics_3d_config {
	uint temp_allocator_bytes; // 
	uint max_amout_of_allowed_jobs;
	uint max_amount_of_physics_barriers;
	int max_amount_of_threads;
	uint max_amount_of_bodies;
	uint max_amount_of_mutexes;
	uint max_amount_of_contact_constraints;
};

// Type: rde_engine_init_info
// Struct that joins all the different areas initials info.
//
// Fields:
//	heap_allocs_config - (<rde_engine_heap_allocs_config>) see <rde_engine_heap_allocs_config>.
//	illumination_config - (<rde_illumination_config>) see <rde_illumination_config>.
//	jolt_config - (<rde_jolt_init_config>) This one is only available if RDE_PHYSICS_MODULE is defined. See <rde_jolt_init_config>.
typedef struct rde_engine_init_info rde_engine_init_info;
struct rde_engine_init_info {
	rde_engine_heap_allocs_config heap_allocs_config;
	
	rde_illumination_config illumination_config;

#ifdef RDE_PHYSICS_MODULE
	rde_jolt_init_config jolt_config;
#endif

};

// =================================================================== RENDERING ===================================================================

// Type: rde_shader_data
// Contains OpenGL data of the shader, basically Ids.
//
// Fields:
//	vertex_program_id - (uint) id of the vertex shader.
//	fragment_program_id - (uint) id of the fragment shader.
//	compiled_program_id - (uint) id of the compiled shader.
typedef struct {
	uint vertex_program_id;
	uint fragment_program_id;
	int compiled_program_id;
} rde_shader_data;

// Type: rde_texture_data
// Information about a texture.
//
// Fields:
//	opengl_texture_id - (int) id of the texture.
//	size - (<rde_vec_2UI>) size of the texture. If it is a subtexture from an atlas, it is the size of the subtexture.
//	position - (<rde_vec_2UI) position of the texture in an atlas, or if the texture is not an atlas, (0, 0).
//	channels - (int) amount of channels (usually 3 or 4 if it has alpha).
//	file_path - (char*) path where the file is located.
//	atlas_texture_id - (int) id of the texture atlas parent, or if it is not a subtexture, -1.
typedef struct {
	int opengl_texture_id;
	rde_vec_2UI size;
	rde_vec_2UI position;
	int channels;
	const char* file_path;
	int atlas_texture_id;
} rde_texture_data;

// Type: rde_texture_parameters
// Information about how to load a texture.
//
// Fields:
//	min_filter - (RDE_TEXTURE_PARAMETER_TYPE_) min filter of the texture.
//	mag_filter - (RDE_TEXTURE_PARAMETER_TYPE_) mag filter of the texture.
//	wrap_s - (RDE_TEXTURE_PARAMETER_TYPE_) wrapS of the texture.
//	wrap_t - (RDE_TEXTURE_PARAMETER_TYPE_) wrapT of the texture.
//	mipmap_min_filter - (RDE_TEXTURE_PARAMETER_TYPE_) mipmap min filter of the texture.
typedef struct {
	bool generate_mipmap;
	RDE_TEXTURE_PARAMETER_TYPE_ min_filter;
	RDE_TEXTURE_PARAMETER_TYPE_ mag_filter;
	RDE_TEXTURE_PARAMETER_TYPE_ wrap_s;
	RDE_TEXTURE_PARAMETER_TYPE_ wrap_t;
	RDE_TEXTURE_PARAMETER_TYPE_ mipmap_min_filter;
} rde_texture_parameters;

// Type: rde_atlas_data
// Information about a texture atlas.
//
// Fields:
//	texture_data - (<rde_texture_data>) texture data.
//	amount_of_subtextures - (int) amount of subtextures that the atlas has.
typedef struct {
	rde_texture_data texture_data;
	int amount_of_subtextures;
} rde_atlas_data;

// Type: rde_font_data
// Information about a font atlas.
//
// Fields:
//	texture_data - (<rde_texture_data>) texture data.
//	amount_of_chars - (int) amount of characters that the font has.
typedef struct {
	rde_texture_data texture_data;
	int amount_of_chars;
} rde_font_data;

// Type: rde_material_light_data
// Information about a directional light.
//
// Fields:
//	shininess - (float) how shiny the light is.
//	ka - (<rde_vec_3F>) factor of ambient light;
//	kd - (<rde_vec_3F>) factor of diffuse light;
//	ks - (<rde_vec_3F>) factor of specular light;
typedef struct {
	float shininess;
	rde_vec_3F ka;
	rde_vec_3F kd;
	rde_vec_3F ks;
} rde_material_light_data;

// Type: rde_material
// Information about a material.
//
// Fields:
//	map_ka - (<rde_texture>) texture containing the ambient light map.
//	map_kd - (<rde_texture>) texture containing the diffuse light map.
//	map_ks - (<rde_texture>) texture containing the specular light map.
//	map_bump - (<rde_texture>) texture containing the bump light map.
//	render_texture - (<rde_texture>) optional texture to render instead of directly to the screen.
//	material_light_data - (<rde_material_light_data>) light information about the material.
typedef struct {
	rde_texture* map_ka;
	rde_texture* map_kd;
	rde_texture* map_ks;
	rde_texture* map_bump;
	rde_render_texture* render_texture;
	rde_material_light_data material_light_data;
} rde_material;

// Type: rde_mesh_gen_data
// Information about the generation of the vertices, normals and texture coordinates of a mesh.
//
// Fields:
//	name - (char*) name of the mesh.
//	vertex_count - (uint) amount of vertices of the mesh.
//	positions - (float*) array of pairs of 3 elements.
//	positions_size - (uint) amount of elements on the array.
//	texcoords - (float*) array of pairs of 2 elements.
//	texcoords_size - (uint) amount of elements on the array.
//	normals - (float*) array of pairs of 3 elements.
//	normals_size - (uint) amount of elements on the array.
//	material - (<rde_material>) material associated to the mesh.
typedef struct {
	char* name;
	uint vertex_count;

	float* positions;
	uint positions_size;
	
	float* texcoords;
	uint texcoords_size;

	float* normals;
	uint normals_size;

	rde_material material;
} rde_mesh_gen_data;

// Type: rde_mesh_data
// Information about a mesh.
//
// Fields:
//	amount_of_vertices - (uint) amount of vertices a mesh has.
//	vertex_buffer_objects_ids - (int[4]) array of ids [positions, colors, normals, texture]
typedef struct {
	uint amount_of_vertices;
	int vertex_buffer_objects_ids[4];
} rde_mesh_data;

// Type: rde_model_data
// Information about a mesh.
//
// Fields:
//	amount_of_meshes - (uint) amount of meshes a 3D model has.
//	meshes - (<rde_mesh>**) array of pointers of rde_mesh. This field MUST be freed by the user.
typedef struct {
	uint amount_of_meshes;
	rde_mesh** meshes;
} rde_model_data;

// Type: rde_directional_light
// Data about the colors and direction of the directional light. This type of light illuminates everything on its direction and has infinite range.
//
// Fields:
//	direction - (<rde_vec_3F>) where the light is pointing at.
//	position - (<rde_vec_3F>) this is just used for shadowing purpose, a position for a directional light makes not real sense.
//	ambient_color - (<rde_vec_3F>) color of the ambient light.
//	diffuse_color - (<rde_vec_3F>) color of the diffuse light.
//	specular_color - (<rde_vec_3F>) color of the specular light.
typedef struct {
	rde_vec_3F direction;
	rde_vec_3F position;
	rde_vec_3F ambient_color;
	rde_vec_3F diffuse_color;
	rde_vec_3F specular_color;
} rde_directional_light;

// Type: rde_point_light
// Data about the colors and position of the point light. This type of light illuminates in all directions around it, but has an action radius.
//
// Fields:
//	position - (<rde_vec_3F>) this is just used for shadowing purpose, a position for a directional light makes not real sense.
//	ambient_color - (<rde_vec_3F>) color of the ambient light.
//	diffuse_color - (<rde_vec_3F>) color of the diffuse light.
//	specular_color - (<rde_vec_3F>) color of the specular light.
//	constant - (float) constant value.
//	linear - (float) linear value.
//	quadratic - (float) quadratic value.
typedef struct {
	rde_vec_3F position;
	rde_vec_3F ambient_color;
	rde_vec_3F diffuse_color;
	rde_vec_3F specular_color;
	float constant;
	float linear;
	float quadratic;
} rde_point_light;

// Type: rde_point_light
// Data about the colors and position of the point light. This type of light illuminates in a specific direction in a lantern way.
//
// Fields:
//	direction - (<rde_vec_3F>) where the light is pointing at.
//	position - (<rde_vec_3F>) this is just used for shadowing purpose, a position for a directional light makes not real sense.
//	cut_off - (float) inner radius.
//	outer_cut_off - (float) outer radius.
//	ambient_color - (<rde_vec_3F>) color of the ambient light.
//	diffuse_color - (<rde_vec_3F>) color of the diffuse light.
//	specular_color - (<rde_vec_3F>) color of the specular light.
//	constant - (float) constant value.
//	linear - (float) linear value.
//	quadratic - (float) quadratic value.
typedef struct {
	rde_vec_3F position;
	rde_vec_3F direction;
	float cut_off;
	float outer_cut_off;
	rde_vec_3F ambient_color;
	rde_vec_3F diffuse_color;
	rde_vec_3F specular_color;
	float constant;
	float linear;
	float quadratic;
} rde_spot_light;

// Type: rde_color
// Color representation, values go from [0, 255].
//
// Fields:
//	r - (unsigned char) red channel.
//	g - (unsigned char) green channel.
//	b - (unsigned char) blue channel.
//	a - (unsigned char) alpha channel.
typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} rde_color;

// Type: rde_polygon
// 2D polygon for shape rendering or other operations.
//
// Fields:
//	vertices - (<rde_vec_2I>*) array of vertices.
//	vertex_count - (uint) amount of vertices.
typedef struct {
	rde_vec_2I* vertices;
	uint vertices_count;
} rde_polygon;

// =================================================================== AUDIO ===================================================================

#ifdef RDE_AUDIO_MODULE
typedef struct rde_sound rde_sound;
typedef struct {
	void* user_data;
	unsigned short channels;
	uint rate;
} rde_sound_config;
RDE_FUNC rde_sound_config rde_struct_create_audio_config();
#endif

// 											==============================================================================
// 											=								GLOBAL VARIABLES					 	   	 =
// 											==============================================================================

// Group: Constants

// Constant: RDE_COLO_WHITE
// White color [255, 255, 255, 255]
const rde_color RDE_COLOR_WHITE 			= { 255, 255, 255, 255 };

// Constant: RDE_COLOR_BLACK
// White color [   0,   0,   0, 255]
const rde_color RDE_COLOR_BLACK 			= {   0,   0,   0, 255 };

// Constant: RDE_COLOR_BLUE
// White color [   0,   0, 255, 255]
const rde_color RDE_COLOR_BLUE 				= {   0,   0, 255, 255 };

// Constant: RDE_COLOR_RED
// White color [255,   0,   0, 255]
const rde_color RDE_COLOR_RED 				= { 255,   0,   0, 255 };

// Constant: RDE_COLOR_YELLOW
// White color [255, 255, 255, 255]
const rde_color RDE_COLOR_YELLOW			= { 255, 255,   0, 255 };

// Constant: RDE_COLOR_MAGENTA
// White color [255,   0, 255, 255]
const rde_color RDE_COLOR_MAGENTA			= { 255,   0, 255, 255 };

// Constant: RDE_COLOR_ORANGE
// White color [255,  69,   0, 255]
const rde_color RDE_COLOR_ORANGE 			= { 255,  69,   0, 255 };

// Constant: RDE_COLOR_GREEN
// White color [   0, 255,   0, 255]
const rde_color RDE_COLOR_GREEN 			= {   0, 255,   0, 255 };

// Constant: RDE_COLOR_BROWN
// White color [139,  69,  19, 255]
const rde_color RDE_COLOR_BROWN 			= { 139,  69,  19, 255 };

// Constant: RDE_COLOR_PURPLE
// White color [128,   0, 128, 255]
const rde_color RDE_COLOR_PURPLE 			= { 128,   0, 128, 255 };

// Constant: RDE_COLOR_GRAY
// White color [128, 128, 128, 255]
const rde_color RDE_COLOR_GRAY				= { 128, 128, 128, 255 };

// Constant: RDE_COLOR_TRANSPARENT
// White color [  0,   0,   0,   0]
const rde_color RDE_COLOR_TRANSPARENT		= {   0,   0,   0,   0 };

// Constant: RDE_COLOR_DISABLED_GRAY
// White color [220, 220, 220, 255]
const rde_color RDE_COLOR_DISABLED_GRAY		= { 220, 220, 220, 255 };

// Constant: RDE_COLOR_PLACEHOLDER_TEXT
// White color [220, 220, 220, 128]
const rde_color RDE_COLOR_PLACEHOLDER_TEXT	= { 220, 220, 220, 128 };

// Constant: RDE_COLOR_RDE_DUCK_YELLOW
// White color [255, 213,  81, 255]
const rde_color RDE_COLOR_RDE_DUCK_YELLOW	= { 255, 213,  81, 255 };

// Constant: RDE_COLOR_GOLD
// White color [255, 213,  81, 255]
const rde_color RDE_COLOR_GOLD				= { 255, 213,  81, 255 };

// Constant: RDE_COLOR_PINK
// White color [255, 109, 194, 255]
const rde_color RDE_COLOR_PINK				= { 255, 109, 194, 255 };

// Constant: RDE_COLOR_NO_TEXTURE
// White color [193,  84, 193, 255]
const rde_color RDE_COLOR_NO_TEXTURE		= { 193,  84, 193, 255 };

// Constant: RDE_DEFAULT_INIT_INFO
// Default information to init the engine.
const rde_engine_init_info RDE_DEFAULT_INIT_INFO = {
	.heap_allocs_config = (rde_engine_heap_allocs_config) {
		RDE_MAX_AMOUNT_OF_WINDOWS,
		RDE_MAX_VERTICES_PER_BATCH,
		RDE_MAX_LOADABLE_SHADERS,
		RDE_MAX_LOADABLE_TEXTURES,
		RDE_MAX_LOADABLE_ATLASES,
		RDE_MAX_LOADABLE_FONTS,
		RDE_MAX_LOADABLE_MODELS,
		RDE_MAX_LOADABLE_MODELS_TEXTURES,

	#ifdef RDE_AUDIO_MODULE
		RDE_MAX_LOADABLE_SOUNDS
	#endif
	},
	.illumination_config = {
		0,
		0
	}

#ifdef RDE_PHYSICS_MODULE
	,
	.jolt_config = {
		.temp_allocator_size = 10 * 1024 * 1024,
		.max_amount_of_physics_jobs = 2048,
		.max_amount_of_physics_barriers = 8,
		.max_amount_of_bodies = 65536,
		.max_amount_of_body_mutexes = 0,
		.max_amount_of_body_pairs = 65536,
		.max_amount_of_contact_constraints = 10240,
		.max_amount_of_threads = -1,
		.collision_steps_per_update = 1
	}
#endif
};

// Constant: RDE_DEFAULT_TEXTURE_PARAMETERS
// Default data for loading a texture.
const rde_texture_parameters RDE_DEFAULT_TEXTURE_PARAMETERS = {
	.min_filter = RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR,
	.mag_filter = RDE_TEXTURE_PARAMETER_TYPE_FILTER_LINEAR,
	.wrap_s = RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT,
	.wrap_t = RDE_TEXTURE_PARAMETER_TYPE_WRAP_REPEAT,
	.mipmap_min_filter = RDE_TEXTURE_PARAMETER_TYPE_MIPMAP_LINEAR_MIN_FILTER_LINEAR
};

// 											==============================================================================
// 											=									FUNCTIONS					 	   	 	 =
// 											==============================================================================

// Group: Constructors

// =================================================================== CONSTRUCTORS ===================================================================

// Constructor: rde_struct_create_probability
RDE_FUNC rde_probability rde_struct_create_probability();

// Constructor: rde_struct_create_camera
// 
// Parameters:
//	_camera_type - type of camera.
RDE_FUNC rde_camera rde_struct_create_camera(RDE_CAMERA_TYPE_ _camera_type);

// Constructor: rde_struct_create_event_window
RDE_FUNC rde_event_window rde_struct_create_event_window();

// Constructor: rde_struct_create_event_display
RDE_FUNC rde_event_display rde_struct_create_event_display();

// Constructor: rde_struct_create_event_key
RDE_FUNC rde_event_key rde_struct_create_event_key();

// Constructor: rde_struct_create_event_mouse
RDE_FUNC rde_event_mouse rde_struct_create_event_mouse();

// Constructor: rde_struct_create_event_controller
RDE_FUNC rde_event_controller rde_struct_create_event_controller();

// Constructor: rde_struct_create_event_mobile_pinch
RDE_FUNC rde_event_mobile_pinch rde_struct_create_event_mobile_pinch();

// Constructor: rde_struct_create_event_mobile
RDE_FUNC rde_event_mobile rde_struct_create_event_mobile();

// Constructor: rde_struct_create_event_drag_and_drop
RDE_FUNC rde_event_drag_and_drop rde_struct_create_event_drag_and_drop();

// Constructor: rde_struct_create_event_data
RDE_FUNC rde_event_data rde_struct_create_event_data();

// Constructor: rde_struct_create_event
RDE_FUNC rde_event rde_struct_create_event();

// Constructor: rde_struct_create_end_user_mandatory_callbacks
RDE_FUNC rde_end_user_mandatory_callbacks rde_struct_create_end_user_mandatory_callbacks();

// Constructor: rde_struct_create_texture_parameters
RDE_FUNC rde_texture_parameters rde_struct_create_texture_parameters();

// Constructor: rde_struct_create_material_light_data
RDE_FUNC rde_material_light_data rde_struct_create_material_light_data();

// Constructor: rde_struct_create_material
RDE_FUNC rde_material rde_struct_create_material();

// Constructor: rde_struct_create_directional_light
RDE_FUNC rde_directional_light rde_struct_create_directional_light();

// Constructor: rde_struct_create_point_light
RDE_FUNC rde_point_light rde_struct_create_point_light();

// Constructor: rde_struct_create_spot_light
RDE_FUNC rde_spot_light rde_struct_create_spot_light();

// Constructor: rde_struct_create_color
RDE_FUNC rde_color rde_struct_create_color();

// Constructor: rde_struct_create_polygon
RDE_FUNC rde_polygon rde_struct_create_polygon();

// =================================================================== LOG ===================================================================

// Group: Log Functions

//  Function: rde_log_color
//
//	Prints to the console on a specific color.
//
//	Parameters:
//		_color - Color to print to the console.
//		_fmt   - formatted C style string to print.
//
//	About:
//
//		This is an inner function, do not use. Use macro <rde_log_color> instead.
RDE_FUNC void rde_log_color_inner(RDE_LOG_COLOR_ _color, const char* _fmt, ...);

//  Function: rde_log_level
//
//	Prints to the console with a specific severity (which includes an automatic color).
//
//	Parameters:
//		_level - level of severity.
//		_fmt   - formatted C style string to print.
//
//	About:
//
//		This is an inner function, do not use. Use macro <rde_log_level> instead.
RDE_FUNC void rde_log_level_inner(RDE_LOG_LEVEL_ _level, const char* _fmt, ...);

// =================================================================== UTIL ===================================================================

// Group: Util Functions

RDE_SAFE_ARR_ACCESS(int)
RDE_SAFE_ARR_ACCESS(uint)
RDE_SAFE_ARR_ACCESS(size_t)
RDE_SAFE_ARR_ACCESS(short)
RDE_SAFE_ARR_ACCESS(float)
RDE_SAFE_ARR_ACCESS(double)

RDE_SAFE_ARR_SET(int)
RDE_SAFE_ARR_SET(uint)
RDE_SAFE_ARR_SET(size_t)
RDE_SAFE_ARR_SET(short)
RDE_SAFE_ARR_SET(float)
RDE_SAFE_ARR_SET(double)

// Function: rde_util_file_get_file_extension
// Gets the extension of a file given a file path.
//
// Parameters:
//	_file_name - path to the file.
RDE_FUNC const char* rde_util_file_get_file_extension(const char* _file_name);

// Function: rde_util_file_sanitaize_path
// Transforms a path that may contain Windows backslashes into a Linux-like path.
//
// Parameters:
//	_path - original path.
//	_sanitized - array with enough space to save the _path sanitized.
//	_sanitized_size - size of the _sanitized array.
RDE_FUNC void rde_util_file_sanitaize_path(const char* _path, char* _sanitized, uint _sanitized_size);

// Function: rde_util_font_get_string_width
// Gets the width (in pixels) that a string will occupy.
//
// Parameters:
//	_str - string to messure.
//	_font - font that will render it.
RDE_FUNC uint rde_util_font_get_string_width(const char* _str, const rde_font* _font);

// Function: rde_util_font_get_string_size
// Gets the width and height (in pixels) that a string will occupy.
//
// Parameters:
//	_str - string to messure.
//	_font - font that will render it.
RDE_FUNC rde_vec_2I rde_util_font_get_string_size(const char* _str, const rde_font* _font);

// Function: rde_util_string_trim
// Removes spaces from the beginning and end of the string. It makes no allocations.
//
// Parameters:
//	_str - string to trim.
RDE_FUNC char* rde_util_string_trim(char* _str);

// Function: rde_util_string_starts_with
// Tells if a string starts with a given prefix. It makes no allocations.
//
// Parameters:
//	_str - input string.
//	_prefix - prefix to check.
RDE_FUNC bool rde_util_string_starts_with(const char* _str, const char* _prefix);

// Function: rde_util_string_ends_with
// Tells if a string ends with a given suffix. It makes no allocations.
//
// Parameters:
//	_str - input string.
//	_suffix - suffix to check.
RDE_FUNC bool rde_util_string_ends_with(const char* _str, const char* _suffix);

// Function: rde_util_string_contains_substring
// Tells if a string contains a given substring. It makes no allocations.
//
// Parameters:
//	_str - input string.
//	_sub_str - substring to check.
//	_case_sensitive - enables or disables case sensitivity.
RDE_FUNC bool rde_util_string_contains_sub_str(const char* _str, const char* _sub_str, bool _case_sensitive);

// Function: rde_util_string_char_appearences
// Returns how many times a char appears on a string. It makes no allocations.
//
// Parameters:
//	_str - input string.
//	_char - char to check.
RDE_FUNC uint rde_util_string_char_appearences(const char* _str, char _char);

// Function: rde_util_string_format
// Formats a given string with values. It makes no allocations.
//
// Parameters:
//	_dst - container to save the formatted string.
//	_size - size of the string container.
//	_fmt - formatted string.
RDE_FUNC void rde_util_string_format(char* _dst, uint _dst_size, const char* _fmt, ...);

// Function: rde_util_string_to_lower
// Transforms the full string into lower case into a new container. It makes no allocations.
//
// Parameters:
//	_dst - container to save the lower-cased string.
//	_str - original string.
RDE_FUNC void rde_util_string_to_lower(char* _dst, const char* _str);

// Function: rde_util_string_to_lower_itself
// Transforms the full string into lower case. It makes no allocations.
//
// Parameters:
//	_str - original string.
RDE_FUNC void rde_util_string_to_lower_itself(char* _str);

// Function: rde_util_string_to_upper
// Transforms the full string into upper case into a new container. It makes no allocations.
//
// Parameters:
//	_dst - container to save the upper-cased string.
//	_str - original string.
RDE_FUNC void rde_util_string_to_upper(char* _dst, const char* _str);

// Function: rde_util_string_to_upper_itself
// Transforms the full string into upper case. It makes no allocations.
//
// Parameters:
//	_str - original string.
RDE_FUNC void rde_util_string_to_upper_itself(char* _str);

// Function: rde_util_string_replace_char
// Replace the first appearence of a char by another one. It makes no allocations.
//
// Parameters:
//	_str - original string.
//	_old_char - char to be replaced.
//	_new_char - char that will replace _old_char.
RDE_FUNC void rde_util_string_replace_char(char* _str, char _old_char, char _new_char);

// Function: rde_util_string_replace_chars_all
// Replace every appearence of a char by another one. It makes no allocations.
//
// Parameters:
//	_str - original string.
//	_old_char - char to be replaced.
//	_new_char - char that will replace _old_char.
RDE_FUNC void rde_util_string_replace_chars_all(char* _str, char _old_char, char _new_char);

// Function: rde_util_string_replace_sub_str
// Replace every appearence of a substring by another one. Returned value MUST be freed by the user.
//
// Parameters:
//	_str - original string.
//	_old_str - string to be replaced.
//	_new_str - string that will replace _old_char.
//	_output_appearences - output parameter that will give the amount of times the _old_string was replaced.
RDE_FUNC char* rde_util_string_replace_sub_str(char* _str, char* _old_str, char* _new_str, int* _output_appearences);

// Function: rde_util_string_split
// Splits a string into a given array of char*. Returns how many splits the array has. Each of the entries of the array of char* MUST be freed by the user.
//
// Parameters:
//	_str - original string.
//	_split_array - pointer to an array of char* that will hold the amount of splits.
//	_split_mark - char to split by.
RDE_FUNC uint rde_util_string_split(char* _str, char*** _split_array, char _split_mark);

// Function: rde_util_hash_map_hash_default
// Transforms data into a hash. This is not typically used by end-users, it is used internally when hashing with hashmap functions.
//
// Parameters:
//	_key - poiner to some data.
//	_size - size of real type of _key
RDE_FUNC uint rde_util_hash_map_hash_default(const void* _key, size_t _size);

// Function: rde_util_hash_map_int_hash
// Transforms an int into a hash.
//
// Parameters:
//	_key - poiner to int value.
RDE_FUNC uint rde_util_hash_map_int_hash(int* _key);

// Function: rde_util_hash_map_str_hash
// Transforms an const char* into a hash.
//
// Parameters:
//	_key - poiner to const char* value.
RDE_FUNC uint rde_util_hash_map_str_hash(const char** _key);

// =================================================================== MATH ===================================================================

// Function: rde_math_set_random_seed
// Sets the seed for randomness. Using -1 will set it to a random value according to the current time. Fixed values will produce fixed results over different runs.
//
// Parameters:
//	_seed - sets the randomness
RDE_FUNC void rde_math_set_random_seed(long _seed);

// Function: rde_math_get_random_int
// Returns a random number int between [_min_included, _max_included] (both sides are included).
//
// Parameters:
//	_min_included - minimum value the random number can be.
//	_max_included - maximum value the random number can be.
RDE_FUNC int rde_math_get_random_int(int _min_included, int _max_included);

// Function: rde_math_get_random_float
// Returns a random number float between [_min_included, _max_included] (both sides are included).
//
// Parameters:
//	_min_included - minimum value the random number can be.
//	_max_included - maximum value the random number can be.
RDE_FUNC float rde_math_get_random_float(float _min_included, float _max_included);

// Function: rde_math_get_probability
// Rolls a probability and returns if it happened.
//
// Parameters:
//	_normalized_chance_to_happen - [0, 1] value of the chance to happen.
RDE_FUNC rde_probability rde_math_get_probability(float _normalized_chance_to_happen);

// Function: rde_math_convert_world_position_to_screen_coordinates_3d
// Transforms a position on the screen into a 3D position on the scene coordinates.
//
// Parameters:
//	_window - window where the calculation happens.
//	_vec - in/out vector to save the calculated position.
RDE_FUNC void rde_math_convert_world_position_to_screen_coordinates_3d(rde_window* _window, rde_vec_3F* _vec);

// Function: rde_math_convert_world_position_to_screen_coordinates_2d
// Transforms a scene 3D coordinate into the corresponding 2D screen coordinate.
//
// Parameters:
//	_window - window where the calculation happens.
//	_vec - in/out vector to save the calculated position.
RDE_FUNC void rde_math_convert_world_position_to_screen_coordinates_2d(rde_window* _window, rde_vec_2F* _vec);

// Function: rde_math_convert_world_size_to_screen_size
// Transforms a scene 2D size into a normalized [-1, 1] screen position.
//
// Prameters:
//	_window - window where the calculation happens.
//	_vec - in/out vector to save the calculated size.
RDE_FUNC void rde_math_convert_world_size_to_screen_size(rde_window* _window, rde_vec_2F* _vec);

// Function: rde_math_cross_product
// Calculates the cross product of two 3D vectors.
//
// Parameters:
//	_vec_0 - first vector.
//	_vec_1 - second vector.
RDE_FUNC rde_vec_3F rde_math_cross_product(rde_vec_3F _vec_0, rde_vec_3F _vec_1);

// Function: rde_math_normalize
// Normalizes a vector.
//
// Parameters:
//	_vec_0 - in/out vector to normalized.
RDE_FUNC void rde_math_normalize(rde_vec_3F* _vec);

// Function: rde_math_clamp_
// Group of functions that clamps a value between other 2 values. Available for int, uint, float, double, long and ulong.
//
// Parameters:
//	_value - value to clamp.
//	_min - min value, of _value < _min, then _value = min.
//	_max - max value, of _value > _max, then _value = max.
RDE_DECLARE_CLAMP_FUNCS(int)
RDE_DECLARE_CLAMP_FUNCS(uint)
RDE_DECLARE_CLAMP_FUNCS(float)
RDE_DECLARE_CLAMP_FUNCS(double)
RDE_DECLARE_CLAMP_FUNCS(long)
RDE_DECLARE_CLAMP_FUNCS(ulong)

// Function: rde_math_radians_to_degrees
// Transforms a radian value into degrees.
//
// Parameters:
//	_radians - value to convert to degrees.
RDE_FUNC float rde_math_radians_to_degrees(float _radians);

// Function: rde_math_degrees_to_radians
// Transforms a degree value into radians.
//
// Parameters:
//	_degrees - value to convert to radians.
RDE_FUNC float rde_math_degrees_to_radians(float _degrees);

// Function: rde_math_quaternion_to_euler_degrees
// Transforms a <rde_quaternion> value into <rde_vec_3F> of degrees (x, y , z).
//
// Parameters:
//	_quaternion - value to convert to rde_vec_3F of degrees.
RDE_FUNC rde_vec_3F rde_math_quaternion_to_euler_degrees(rde_quaternion _quaternion);

// Function: rde_math_euler_degrees_to_quaternion
// Transforms a <rde_vec_3F> of degrees (x, y , z) value into a <rde_quaternion>.
//
// Parameters:
//	_euler_degrees - value to convert to <rde_quaternion>.
RDE_FUNC rde_quaternion rde_math_euler_degrees_to_quaternion(rde_vec_3F _euler_degrees);

// Function: rde_math_easing_
// Group of functions that applies easing functions.
// Available for in_linear, in_quadratic, out_quadratic, in_out_quadratic, in_cubic, out_cubic, in_out_cubic, in_quartic, out_quartic, in_out_quartic,
// in_quintic, out_quintic, in_out_quintic, in_sine, out_sine, in_out_sine, in_exponential, out_exponential, in_out_exponential, in_circular, out_circular,
// in_out_circular.
//
// Parameters:
//	_current_time - current time.
//	_start_value - start value.
//	_change_in_value - change in time.
//	_duration - duration.
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

// =================================================================== ENGINE ===================================================================

// Function: rde_engine_create_engine
// Initializes the engine with a specific configuration file. It returns the default created window. This function MUST be called before using the engine.
//
// Parameters:
//	_argc - amount of arguments passed to the program. Can be 0.
//	_argv - vector of arguments passed to the program. Can be NULL.
//	_config_path - path to the config file. This file path must exist as the engine won't start without it.
RDE_FUNC rde_window* rde_engine_create_engine(int _argc, char** _argv, const char* _config_path);

// Function: rde_engine_load_config
// Loads the initial configuration of the engine.
//
// Parameters:
//	_config_path - path to the config file.
RDE_FUNC rde_engine_init_info rde_engine_load_config(const char* _config_path);

// Function: rde_setup_initial_info
// Configures the mandatory callbacks that the user must provide.
//
// Parameters:
//	_end_user_callbacks - group of callbacks that the user must fill.
RDE_FUNC void rde_setup_initial_info(rde_end_user_mandatory_callbacks _end_user_callbacks); /// Sets up some basic info the engine needs, call this BEFORE rde_engine_create_engine

// Function: rde_engine_set_event_user_callback
// Optional function callback for custom event handling.
//
// Parameters:
//	_user_event_callback - callback for custom event handling.
RDE_FUNC void rde_engine_set_event_user_callback(rde_event_func _user_event_callback);

// Function: rde_engine_logs_supressed
// Returns if the logs are supressed.
RDE_FUNC bool rde_engine_logs_supressed();

// Function: rde_engine_supress_logs
// Enables or disables logging on the console.
//
// Parameters:
//	_supress - if the logs should or not be supressed.
RDE_FUNC void rde_engine_supress_logs(bool _supress);

// Function: rde_engine_get_platform
// Returns the current platform the engine is running on.
RDE_FUNC RDE_PLATFORM_TYPE_ rde_engine_get_platform();

// Function: rde_engine_get_fixed_delta
// Returns the fixed update physics delta.
RDE_FUNC float rde_engine_get_fixed_delta();

// Function: rde_engine_set_fixed_delta
// Sets the fixed update physics delta.
//
// Parameters:
//	_fixed_dt - fixed update physics delta.
RDE_FUNC void rde_engine_set_fixed_delta(float _fixed_dt);

// Function: rde_engine_on_run
// Starts the engine main loop and also initializes some inner engine data. Will be running until the application is closed or crashes.
RDE_FUNC void rde_engine_on_run();

// Function: rde_engine_is_running
// Returns if the engine is currently running.
RDE_FUNC bool rde_engine_is_running();

// Function: rde_engine_set_running
// Makes the engine stop if set to false.
//
// Parameters:
//	_running - if false, the engine will close.
RDE_FUNC void rde_engine_set_running(bool _running);

// Function: rde_engine_get_display_size
// Returns the size of the display where the current window is at.
RDE_FUNC rde_vec_2I rde_engine_get_display_size();

// Function: rde_engine_get_available_display_ids
// Fills an array of uints with the ids of all the available displays.
//
// Parameters:
//	_out_ids - array of uints where the ids will be stored. Must be at least as big as the amount of displays.
RDE_FUNC void rde_engine_get_available_display_ids(uint* _out_ids);

// Function: rde_engine_switch_window_display
// Moves a current window to the selected display.
//
// Parameters:
//	_window - window to move.
//	_new_display - id of the display to move the window.
RDE_FUNC void rde_engine_switch_window_display(rde_window* _window, uint _new_display);

// Function: rde_engine_get_focused_window
// Returnst the currently focused window.
RDE_FUNC rde_window* rde_engine_get_focused_window();

// Function: rde_engine_is_vsync_active
// Returns if VSync is active or not.
RDE_FUNC bool rde_engine_is_vsync_active();

// Function: rde_engine_set_vsync_active
// Enables or disables VSync.
// 
// Parameters:
//	_vsync - enable or disable vsync.
RDE_FUNC void rde_engine_set_vsync_active(bool _vsync);

// Function: rde_engine_show_message_box
// Creates a system dialog box with some messages and buttons. It will block the main thread until dismissed.
//
// Parameters:
//	_level - type of message.
//	_title - title of the message box.
//	_content - body message of the box.
//	_window - window to show the message box.
RDE_FUNC void rde_engine_show_message_box(RDE_LOG_LEVEL_ _level, const char* _title, const char* _content, rde_window* _window);

// Function: rde_engine_destroy_engine
// Destroys the engine and cleans every allocation and releases any asset used by the engine.
RDE_FUNC void rde_engine_destroy_engine();

// Function: rde_transform_load
// Returns a <rde_transform> from the preloaded pool of transforms. Transforms are not allowed to be created nor modify by end-users as
// there are engine-internal operations that must be notified when modifying a <rde_transform> fields, mainly for parenting purposes.
RDE_FUNC rde_transform* rde_transform_load();

// Function: rde_transform_unload
// Unloads a previously loaded <rde_transform> and returns it to the pool.
//
// Parameters:
//	_transform - transform to return to the pool.
RDE_FUNC void rde_transform_unload(rde_transform* _transform);

// Function: rde_transform_get_position
// Returns the position of a transform.
//
// Parameters:
//	_transform - transform to get the position.
RDE_FUNC rde_vec_3F rde_transform_get_position(rde_transform* _transform);

// Function: rde_transform_set_position
// Sets the position of a transform.
//
// Parameters:
//	_transform - transform to set the position.
//	_position - position of the transform.
RDE_FUNC void rde_transform_set_position(rde_transform* _transform, rde_vec_3F _position);

// Function: rde_transform_get_rotation_degs
// Returns the rotation in degrees of a transform.
//
// Parameters:
//	_transform - transform to get the rotation.
RDE_FUNC rde_vec_3F rde_transform_get_rotation_degs(rde_transform* _transform);

// Function: rde_transform_set_rotation
// Sets the rotation in degrees of a transform.
//
// Parameters:
//	_transform - transform to set the position.
//	_rotation_degs - rotation in degrees of the transform.
RDE_FUNC void rde_transform_set_rotation(rde_transform* _transform, rde_vec_3F _rotation_degs);

// Function: rde_transform_get_scale
// Returns the scale of a transform.
//
// Parameters:
//	_transform - transform to get the scale.
RDE_FUNC rde_vec_3F rde_transform_get_scale(rde_transform* _transform);

// Function: rde_transform_set_scale
// Sets the scale of a transform.
//
// Parameters:
//	_transform - transform to set the position.
//	_scale - scale of the transform.
RDE_FUNC void rde_transform_set_scale(rde_transform* _transform, rde_vec_3F _scale);

// Function: rde_transform_get_parent
// Returns the parent of a transform. If no parent, NULL will be returned.
//
// Parameters:
//	_transform - transform to get the parent.
RDE_FUNC rde_transform* rde_transform_get_parent(rde_transform* _transform);

// Function: rde_transform_set_parent
// Sets the parent of a transform. Doing this will apply parent transformations to the child transform.
//
// Parameters:
//	_transform - transform to set the parent.
//	_parent - parent of the _transform.
RDE_FUNC void rde_transform_set_parent(rde_transform* _transform, rde_transform* _parent);

// Function: rde_transform_get_children_count
// Returns the amount of children that a transform has.
//
// Parameters:
//	_transform - transform to get the amount of children.
RDE_FUNC uint rde_transform_get_children_count(rde_transform* _transform);

// =================================================================== WINDOW ===================================================================

// Function: rde_window_create_window_os
// Creates a new window ready to work on any supported platform. This window must be destroyed by the user when not needed anymore.
RDE_FUNC rde_window* rde_window_create_window_os();

// Function: rde_window_get_window_size
// Returns the size of a window.
//
// Parameters:
//	_window - window to get the size.
RDE_FUNC rde_vec_2I	rde_window_get_window_size(rde_window* _window);

// Function: rde_window_set_window_size
// Sets the new size of a window. Will trigger same events as resizing manually with the mouse.
//
// Parameters:
//	_window - window to resize.
//	_size - new size of the window.
RDE_FUNC void rde_window_set_window_size(rde_window* _window, rde_vec_2I _size);


// Function: rde_window_get_window_position
// Returns the current position of a window.
//
// Parameters:
//	_window - window to get the position.
RDE_FUNC rde_vec_2I	rde_window_get_window_position(rde_window* _window);

// Function: rde_window_set_window_position
// Sets the new position of a window.
//
// Parameters:
//	_window - window to move.
//	_position - position to move the window.
RDE_FUNC void rde_window_set_window_position(rde_window* _window, rde_vec_2I _position);

// Function: rde_window_get_window_title
// Returns the title of a window.
//
// Parameters:
//	_window - window to get the title.
RDE_FUNC const char* rde_window_get_window_title(rde_window* _window);

// Function: rde_window_set_window_title
// Sets the title of a window.
//
// Parameters:
//	_window - window to set the title.
//	_title - new title of the window.
RDE_FUNC void rde_window_set_window_title(rde_window* _window, const char* _title);

// Function: rde_window_orientation_is_horizontal
// Returns if the width of a window is greater than its height.
//
// Parameters:
//	_window - window to get the orientation.
RDE_FUNC bool rde_window_orientation_is_horizontal(rde_window* _window);

// Function: rde_window_take_screenshot
// Takes a screenshot of a window and saves it to a file.
//
// Parameters:
//	_window - window to take the screenshot.
//	_position - position of the center of the square to take the screenshoot (being (0, 0) the middle of the window).
//	_size_of_rectangle - size of the rectangle to take the screenshot (width and height is the full width and height of the rectangle).
//	_file_name_with_extension - full path to the image to save, png is prefered.
RDE_FUNC void rde_window_take_screenshot(rde_window* _window, rde_vec_2I _position, rde_vec_2I _size_of_rectangle, const char* _file_name_with_extension);

// Function: rde_window_get_area_of_screen_pixels
// Returns a dynamic array of the pixels in a RGBA format (so 4 elements per pixel). User MUST free the returned array.
//
// Parameters:
//	_window - window to take the pixels from.
//	_position - position of the center of the square to take the pixels (being (0, 0) the middle of the window).
//	_size - size of the rectangle to take the pixels (width and height is the full width and height of the rectangle).
RDE_FUNC unsigned char* rde_window_get_area_of_screen_pixels(rde_window* _window, rde_vec_2I _position, rde_vec_2I _size);

// Function: rde_window_get_aspect_ratio
// Returns the aspect ratio (width / height) of a window.
//
// Parameters:
//	_window - window to get the aspect ration.
RDE_FUNC float rde_window_get_aspect_ratio(rde_window* _window);

// Function: rde_window_is_mouse_out_of_window_allowed
// Returns if the mouse is allowed to move outside of a window.
RDE_FUNC bool rde_window_is_mouse_out_of_window_allowed();

// Function: rde_window_allow_mouse_out_of_window
// Sets if the mouse is allowed to move outside of a window.
//
// Parameters:
//	_allow_mouse_out_of_window - if it is allowed or not.
RDE_FUNC void rde_window_allow_mouse_out_of_window(bool _allow_mouse_out_of_window);

// Function: rde_window_set_window_icon
// Sets the icon of a window's title bar. File must be .ico format.
//
// Parameters:
//	_window - window to set the icon.
//	_path_to_icon - full path to the .ico file.
RDE_FUNC void rde_window_set_window_icon(rde_window* _window, const char* _path_to_icon);

// Function: rde_window_get_native_sdl_window_handle
// Returns a pointer to the native SDL window.
//
// Parameters:
//	_window - window to get the native window from.
RDE_FUNC void* rde_window_get_native_sdl_window_handle(rde_window* _window);

// Function: rde_window_get_native_sdl_gl_context_handle
// Returns a pointer to the native SDL OpenGL window.
//
// Parameters:
//	_window - window to get the native OpenGL hanfle from.
RDE_FUNC void* rde_window_get_native_sdl_gl_context_handle(rde_window* _window);

// Function: rde_window_destroy_window
// Destroys the window, releasing any allocated memory and also SDL and OpenGL context data. Will set _data to NULL automatically.
//
// Parameters:
//	_window - window to be destroyed.
RDE_FUNC void rde_window_destroy_window(rde_window* _window);

// =================================================================== EVENTS ===================================================================

// Function: rde_events_window_consume_events
// Takes and tries to handle window events for a window. If the event has been handled, its flag will be set to true.
//
// Parameters:
//	_event - event to be handled.
//	_window - window where the event happened.
RDE_FUNC void rde_events_window_consume_events(rde_event* _event, rde_window* _window);

// Function: rde_events_display_consume_events
// Takes and tries to handle display events for a window. If the event has been handled, its flag will be set to true.
//
// Parameters:
//	_event - event to be handled.
//	_window - window where the event happened.
RDE_FUNC void rde_events_display_consume_events(rde_event* _event, rde_window* _window);

// Function: rde_events_keyboard_consume_events
// Takes and tries to handle keyboard events for a window. If the event has been handled, its flag will be set to true.
//
// Parameters:
//	_event - event to be handled.
//	_window - window where the event happened.
RDE_FUNC void rde_events_keyboard_consume_events(rde_event* _event, rde_window* _window);

// Function: rde_events_mouse_consume_events
// Takes and tries to handle mouse events for a window. If the event has been handled, its flag will be set to true.
//
// Parameters:
//	_event - event to be handled.
//	_window - window where the event happened.
RDE_FUNC void rde_events_mouse_consume_events(rde_event* _event, rde_window* _window);

// Function: rde_events_drag_and_drop_consume_events
// Takes and tries to handle drag & drop events for a window. If the event has been handled, its flag will be set to true.
//
// Parameters:
//	_event - event to be handled.
//	_window - window where the event happened.
RDE_FUNC void rde_events_drag_and_drop_consume_events(rde_event* _event, rde_window* _window);

// Function: rde_events_mobile_consume_events
// Takes and tries to handle mobile events for a window. If the event has been handled, its flag will be set to true.
//
// Parameters:
//	_event - event to be handled.
//	_window - window where the event happened.
RDE_FUNC void rde_events_mobile_consume_events(rde_event* _event, rde_window* _window);

// Function: rde_events_is_key_just_pressed
// Returns true if a key on a window has just been pressed, this function will only return true during 1 frame
//
// Parameters:
//	_window - window to check the event.
//	_key - key to check.
RDE_FUNC bool rde_events_is_key_just_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key);

// Function: rde_events_is_key_pressed
// Returns true if a key on a window is pressed, this function will return true for 1 or more frames.
//
// Parameters:
//	_window - window to check the event.
//	_key - key to check.
RDE_FUNC bool rde_events_is_key_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key);

// Function: rde_events_is_key_just_released
// Returns true if a key on a window has just been released, this function will only return for 1 frame.
//
// Parameters:
//	_window - window to check the event.
//	_key - key to check.
RDE_FUNC bool rde_events_is_key_just_released(rde_window* _window, RDE_KEYBOARD_KEY_ _key);

// Function: rde_events_is_mouse_button_just_pressed
// Returns true if a mouse button on a window has just been pressed, this function will only return for 1 frame.
//
// Parameters:
//	_window - window to check the event.
//	_button - mouse button to check.
RDE_FUNC bool rde_events_is_mouse_button_just_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button);

// Function: rde_events_is_mouse_button_pressed
// Returns true if a mouse button on a window is pressed, this function will return true for 1 or more frames.
//
// Parameters:
//	_window - window to check the event.
//	_button - mouse button to check.
RDE_FUNC bool rde_events_is_mouse_button_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button);

// Function: rde_events_is_mouse_button_just_released
// Returns true if a mouse button on a window has just been released, this function will only return for 1 frame.
//
// Parameters:
//	_window - window to check the event.
//	_button - mouse button to check.
RDE_FUNC bool rde_events_is_mouse_button_just_released(rde_window* _window, RDE_MOUSE_BUTTON_ _button);

// Function: rde_events_mouse_get_scrolled
// Returns a vector with the scrolled amount on a window.
//
// Parameters:
//	_window - window where the scroll happened.
RDE_FUNC rde_vec_2F rde_events_mouse_get_scrolled(rde_window* _window);

// Function: rde_events_mouse_get_position
// Returns the position of the mouse on a window, position (0, 0) is centered around the middle of the screen.
//
// Parameters:
//	_window - window where the scroll happened.
RDE_FUNC rde_vec_2I rde_events_mouse_get_position(rde_window* _window);

// Function: rde_events_is_mobile_touch_just_pressed
// Returns true if a screen on a window has just been pressed, this function will only return true during 1 frame.
//
// Parameters:
//	_window - window to check the event.
//	_finger_id - finger to check.
RDE_FUNC bool rde_events_is_mobile_touch_just_pressed(rde_window* _window, uint _finger_id);

// Function: rde_events_is_mobile_touch_pressed
// Returns true if a screen on a window is pressed, this function will return true for 1 or more frames.
//
// Parameters:
//	_window - window to check the event.
//	_finger_id - finger to check.
RDE_FUNC bool rde_events_is_mobile_touch_pressed(rde_window* _window, uint _finger_id);

// Function: rde_events_is_mobile_touch_released
// Returns true if a screen on a window has just been released, this function will only return true during 1 frame.
//
// Parameters:
//	_window - window to check the event.
//	_finger_id - finger to check.
RDE_FUNC bool rde_events_is_mobile_touch_released(rde_window* _window, uint _finger_id);

// Function: rde_events_mobile_get_finger_amount
// Returns the total amount of fingers touching the screen on a window.
//
// Parameters:
//	_window - window to check the event.
RDE_FUNC uint rde_events_mobile_get_finger_amount(rde_window* _window);

// =================================================================== RENDERING ===================================================================

// Function: rde_rendering_shader_load
// Loads a shader into memory given its name (to find it later), vertex shader and fragment shader and returns a pointer to it.
//
// Parameters:
//	_name - name of the shader, must be unique between the shader names.
//	_vertex_code - glsl vertex shader code.
//	_fragment_code - glsl fragment shader code.
//	_geometry_code - glsl geometry shader code.
//	_tessellation_code - array of 2 elements. cs and es glsl tessellation shader code.
RDE_FUNC rde_shader* rde_rendering_shader_load(const char* _name, const char* _vertex_code, const char* _fragment_code, const char* _geometry_code, const char** _tessellation_code);

// Function: rde_rendering_shader_set_uniform_value_float
// Sets a float uniform value to a specific shader.
//
// Parameters:
//	_shader - shader to set the value.
//	_uniform_name - name of the uniform in the shader.
//	_type - type of float uniform.
//	_data - array of values. If it is a uniform of just 1 value, it can be passed as &.
//	_transpose - in case it is a matrix uniform, if the matrix is transposed.
RDE_FUNC void rde_rendering_shader_set_uniform_value_float(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_FV_ _type, float* _data, bool _transpose);

// Function: rde_rendering_shader_set_uniform_value_int
// Sets a int uniform value to a specific shader.
//
// Parameters:
//	_shader - shader to set the value.
//	_uniform_name - name of the uniform in the shader.
//	_type - type of int uniform.
//	_data - array of values. If it is a uniform of just 1 value, it can be passed as &.
RDE_FUNC void rde_rendering_shader_set_uniform_value_int(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_IV_ _type, int* _data);

// Function: rde_rendering_shader_set_uniform_value_uint
// Sets a uint uniform value to a specific shader.
//
// Parameters:
//	_shader - shader to set the value.
//	_uniform_name - name of the uniform in the shader.
//	_type - type of uint uniform.
//	_data - array of values. If it is a uniform of just 1 value, it can be passed as &.
RDE_FUNC void rde_rendering_shader_set_uniform_value_uint(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_UIV_ _type, uint* _data);

// Function: rde_rendering_shader_get_data
// Returns read-only data of a shader.
//
// Parameters:
//	_shader - shader to get the data from.
RDE_FUNC rde_shader_data rde_rendering_shader_get_data(rde_shader* _shader);

// Function: rde_rendering_shader_get_by_name
// Returns a pointer to a shader given its name.
//
// Parameters:
//	_name - name of the shader to find.
RDE_FUNC rde_shader* rde_rendering_shader_get_by_name(const char* _name);

// Function: rde_rendering_shader_unload
// Unloads a shader and returns it to the pool. Pointer is set to NULL automatically.
//
// Parameters:
//	_shader - shader to unload.
RDE_FUNC void rde_rendering_shader_unload(rde_shader* _shader);

// Function: rde_rendering_texture_load
// Loads a texture into the GPU and returns a pointer to it.
//
// Parameters:
//	_file_path - path to the image to load.
//	_params - data to load the image correctly, NULL can be passed for default values.
RDE_FUNC rde_texture* rde_rendering_texture_load(const char* _file_path, const rde_texture_parameters* _params);

// Function: rde_rendering_texture_text_load
// Loads a text atlas into the GPU and returns a pointer to it.
//
// Parameters:
//	_file_path - path to the text atlas to load.
RDE_FUNC rde_texture* rde_rendering_texture_text_load(const char* _file_path);

// Function: rde_rendering_texture_get_data
// Returns read-only data of a texture.
//
// Parameters:
//	_texture - texture to get the data from.
RDE_FUNC rde_texture_data rde_rendering_texture_get_data(rde_texture* _texture);

// Function: rde_rendering_texture_unload
// Unloads a texture and returns it to the pool. Sets pointer to NULL automatically.
//
// Parameters:
//	_texture - texture to unload.
RDE_FUNC void rde_rendering_texture_unload(rde_texture* _texture);

// Function: rde_rendering_atlas_load
// Loads a texture atlas into the GPU and returns a pointer to it. Atlases must be generated with RDE's atlas tool. This will generate 2 file with the same name, but different extensions.
//
// Parameters:
//	_texture_path - path to the texture atlas to load. The name of the file (RDE's atlas tool generates 2 files) must be the name of the 2 files, without extension.
RDE_FUNC rde_atlas* rde_rendering_atlas_load(const char* _texture_path);

// Function: rde_rendering_atlas_get_subtexture
// Returns a specific texture inside a texture atlas.
//
// Parameters:
//	_atlas - atlas to get the sub texture from.
//	_texture_name - name of the subtexture. This name is the same name as the image had when creating the atlas with RDE's tool.
RDE_FUNC rde_texture* rde_rendering_atlas_get_subtexture(rde_atlas* _atlas, const char* _texture_name);

// Function: rde_rendering_atlas_get_data
// Returns read-only data of a texture atlas.
//
// Parameters:
//	_atlas - texture atlas to get the data from.
RDE_FUNC rde_atlas_data rde_rendering_atlas_get_data(rde_atlas* _atlas);

// Function: rde_rendering_atlas_unload
// Unloads a texture atlas and returns it to the pool. Pointer is set to NULL automatically.
//
// Parameters:
//	_atlas - texture atlas to unload.
RDE_FUNC void rde_rendering_atlas_unload(rde_atlas* _atlas);

// Function: rde_rendering_memory_texture_create
// Creates a texture on CPU memory, than later can be uploaded to GPU. Data is stored until the texture is free, so it can be modified as needed. This must be freed by end-user.
//
// Parameters:
//	_width - width of the texture.
//	_height - height of the texture.
//	_channels - amount of channels, typically 4 is used.
RDE_FUNC rde_texture* rde_rendering_memory_texture_create(uint _width, uint _height, int _channels);

// Function: rde_rendering_memory_texture_set_pixel
// Sets a memory texture's pixel to a specific color
//
// Parameters:
//	_memory_texture - memory texture to modify.
//	_position - position of the pixel to modify, being (0, 0) top-left of the image.
//	_color - color to set on the pixel.
RDE_FUNC void rde_rendering_memory_texture_set_pixel(rde_texture* _memory_texture, rde_vec_2I _position, rde_color _color);

// Function: rde_rendering_memory_texture_get_pixel
// Gets a memory texture's pixel color
//
// Parameters:
//	_memory_texture - memory texture to get info.
//	_position - position of the pixel to get, being (0, 0) top-left of the image.
RDE_FUNC rde_color rde_rendering_memory_texture_get_pixel(rde_texture* _memory_texture, rde_vec_2I _position);

// Function: rde_rendering_memory_texture_gpu_upload
// Uploads all the data to the GPU, so it can be rendered by the GPU.
//
// Parameters:
//	_memory_texture - memory texture to upload the data.
RDE_FUNC void rde_rendering_memory_texture_gpu_upload(rde_texture* _memory_texture);

// Function: rde_rendering_memory_texture_get_pixels
// Returns a pointer to the memory textures data. Modifying this pointer will modify the memorys data directly, but won't modify it on GPU, call <rde_rendering_memory_texture_gpu_upload> for this.
//
// Parameters:
//	_memory_texture - memory texture to get the pixels from.
RDE_FUNC unsigned char* rde_rendering_memory_texture_get_pixels(rde_texture* _memory_texture);

// Function: rde_rendering_memory_texture_destroy
// Destroys a memory texture, freeing any allocated data (any referencs to it will be invalid). Also frees OpenGL data. Sets the pointer to NULL automatically.
//
// Parameters:
//	_memory_texture - memory texture to desroy.
RDE_FUNC void rde_rendering_memory_texture_destroy(rde_texture* _memory_texture);

// Function: rde_rendering_render_texture_create
// Creates a render texture. This type of texture is used when users want to render something onto a texture and not directly to the screen. Can later be used to render onto geometry. This needs to be freed by end-users.
//
// Parameters:
//	_width - width of the render texture.
//	_height - height of the render texture.
RDE_FUNC rde_render_texture* rde_rendering_render_texture_create(uint _width, uint _height);

// Function: rde_rendering_render_texture_enable
// Everything rendered from this function until reaching <rde_rendering_render_texture_disable> will be rendered onto _render_texture and not to the screen. Must be a closing <rde_rendering_render_texture_disable>.
//
// Parameters:
//	_render_texture - render texture to draw the scene to.
RDE_FUNC void rde_rendering_render_texture_enable(rde_render_texture* _render_texture);

// Function: rde_rendering_render_texture_disable
// Flushes the current scene drawing onto the previously selected render texture. Must be an opening <rde_rendering_render_texture_enable>.
RDE_FUNC void rde_rendering_render_texture_disable();

// Function: rde_rendering_render_texture_update
// Allows to resize the render texture to a different size.
//
// Parameters:
//	_render_texture - render texture to modify the size.
//	_width - new width of the texture, can be greater or lower.
//	_height - new height of the texture, can be greater or lower.
RDE_FUNC void rde_rendering_render_texture_update(rde_render_texture* _render_texture, uint _width, uint _height);

// Function: rde_rendering_render_texture_destroy
// Destroys a render texture, freeing all the allocated data and OpeGL resources. Sets the pointer to NULL automatically.
RDE_FUNC void rde_rendering_render_texture_destroy(rde_render_texture* _render_texture);

// Function: rde_rendering_font_load
// Loads a font into the GPU and returns a pointer to it. Atlases must be generated with RDE's atlas tool. This will generate 2 file with the same name, but different extensions.
//
// Parameters:
//	_font_path - path to the font to load. The name of the file (RDE's atlas tool generates 2 files) must be the name of the 2 files, without extension.
RDE_FUNC rde_font* rde_rendering_font_load(const char* _font_path);

// Function: rde_rendering_font_get_data
// Returns read-only data of a font.
//
// Parameters:
//	_font - font to get the data from.
RDE_FUNC rde_font_data rde_rendering_font_get_data(rde_font* _font);

// Function: rde_rendering_font_unload
// Unloads a font and returns it to the pool. Pointer is set to NULL automatically.
//
// Parameters:
//	_font - font to unload.
RDE_FUNC void rde_rendering_font_unload(rde_font* _font);

// Function: rde_rendering_set_background_color
// Sets the clear coloring background color.
//
// Parameters:
//	_color - color to set to the background.
RDE_FUNC void rde_rendering_set_background_color(const rde_color _color);

// Function: rde_rendering_2d_begin_drawing
// Starts a 2D drawing batch. Must be called before rendering anything 2D. More than one call can be done on a frame. No nested calls to this function can be made. Any kind of texture can be rendered with
// a single call to this method, if textures cannot be batched, this function and <rde_rendering_2d_end_drawing> handle automatic flushes. Only RDE_CAMERA_TYPE_ORTHOGRAPHIC is allowed.
//
// Parameters:
//	_camera - camera that will render the scene.
//	_window - window where the scene will be rendered.
// _is_hud - if set to true, then rendered elements won't move if the camera moves around the world. Perfect for UI.
RDE_FUNC void rde_rendering_2d_begin_drawing(rde_camera* _camera, rde_window* _window, bool _is_hud);

// Function: rde_rendering_2d_draw_point
// Draws a point on the screen.
//
// Parameters:
//	_position - position of the point. (0, 0) is the middle of the screen.
//	_color - color of the point.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_2d_draw_point(rde_vec_2F _position, rde_color _color, rde_shader* _shader);

// Function: rde_rendering_2d_draw_line
// Draws a line on the screen.
//
// Parameters:
//	_init - starting position of the line. (0, 0) is the middle of the screen.
//	_end - ending position of the line. (0, 0) is the middle of the screen.
//	_color - color of the line.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_2d_draw_line(rde_vec_2F _init, rde_vec_2F _end, rde_color _color, rde_shader* _shader);

// Function: rde_rendering_2d_draw_triangle
// Draws a triangle on the screen.
//
// Parameters:
//	_vertex_a - first vertex of the triangle. (0, 0) is the middle of the screen.
//	_vertex_b - second vertex of the triangle. (0, 0) is the middle of the screen.
//	_vertex_c - third vertex of the triangle. (0, 0) is the middle of the screen.
//	_color - color of the triangle.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_2d_draw_triangle(rde_vec_2F _vertex_a, rde_vec_2F _vertex_b, rde_vec_2F _vertex_c, rde_color _color, rde_shader* _shader);

// Function: rde_rendering_2d_draw_rectangle
// Draws a triangle on the screen.
//
// Parameters:
//	_bottom_left - bottom-left corner of the rectangle. (0, 0) is the middle of the screen.
//	_top_right - top-right corner of the rectangle. (0, 0) is the middle of the screen.
//	_color - color of the rectangle.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_2d_draw_rectangle(rde_vec_2F _bottom_left, rde_vec_2F _top_right, rde_color _color, rde_shader* _shader);

// Macro: rde_rendering_2d_draw_rectangle_1
// "Overload" for <rde_rendering_2d_draw_rectangle>.
//
// Parameters:
//	_center - center of the rectangle. (0, 0) is the middle of the screen.
//	_width - full width of the rectangle. Half will expand to the left and the other to the right.
//	_height - full height of the rectangle. Half will expand to the top and the other to the bottom.
//	_color - color of the rectangle.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
#define rde_rendering_2d_draw_rectangle_1(_center, _width, _height, _color, _shader) rde_rendering_2d_draw_rectangle((rde_vec_2F) { .x = _center.x - _width * 0.5f, .y = _center.y - _height.y }, 	\
																												   (rde_vec_2F) { .x = _center.x + _width * 0.5f, .y = _center.y + _height.y },		\
																												   _color, _shader)

// Function: rde_rendering_2d_draw_circle
// Draws a circle on the screen.
//
// Parameters:
//	_position - Center of the circle. (0, 0) is the middle of the screen.
//	_radius - radius of the circle.
//	_color - color of the circle.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_2d_draw_circle(rde_vec_2F _position, float _radius, rde_color _color, rde_shader* _shader);

// Function: rde_rendering_2d_draw_polygon
// Draws a polygon on the screen.
//
// Parameters:
//	_transform - transform of the polygon.
//	_polygon - polygon data.
//	_color - color of the polygon.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_2d_draw_polygon(const rde_transform* _transform, const rde_polygon* _polygon, rde_color _color, const rde_shader* _shader);

// Function: rde_rendering_2d_draw_texture
// Draws a texture on the screen.
//
// Parameters:
//	_transform - transform of the texture.
//	_texture - texture data.
//	_tint_color - color to tint the texture. Use RDE_COLOR_WHITE if no tint is needed.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_2d_draw_texture(const rde_transform* _transform, const rde_texture* _texture, rde_color _tint_color, rde_shader* _shader);

// Function: rde_rendering_2d_draw_memory_texture
// Draws a memory texture on the screen.
//
// Parameters:
//	_transform - transform of the memory texture.
//	_texture - texture data.
//	_tint_color - color to tint the memory texture. Use RDE_COLOR_WHITE if no tint is needed.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_2d_draw_memory_texture(const rde_transform* _transform, rde_texture* _texture, rde_color _tint_color, rde_shader* _shader);


// Function: rde_rendering_2d_draw_text
// Draws a text on the screen.
//
// Parameters:
//	_transform - transform of the memory texture.
//	_font - font data.
//	_text - text to draw.
//	_tint_color - color to tint the text. Use RDE_COLOR_WHITE if no tint is needed.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_2d_draw_text(const rde_transform* _transform, const rde_font* _font, const char* _text, rde_color _tint_color, rde_shader* _shader);

// Function: rde_rendering_2d_end_drawing
// Ends a drawing batch. A previous <rde_rendering_2d_begin_drawing> is mandatory to use this function. Flushes everything into the GPU.
RDE_FUNC void rde_rendering_2d_end_drawing();

// Function: rde_struct_memory_mesh_create
// Creates a 3D mesh with the specified data. For example, a quad mesh will have 4 vertices and 6 indices and uploads to GPU. This must be freed by the end-user.
//
// Parameters:
//	_data - data specific for the new 3D mesh.
RDE_FUNC rde_mesh* rde_struct_memory_mesh_create(rde_mesh_gen_data* _data);

// Function: rde_rendering_mesh_create_cube
// Creates a 3D cube mesh. This must be freed by the end-user.
//
// Parameters:
//	_size - size of the cube.
//	_material - material that the cube will use.
RDE_FUNC rde_mesh* rde_rendering_mesh_create_cube(float _size, rde_material* _material);

// Function: rde_rendering_mesh_create_prism
// Creates a 3D prism mesh. This must be freed by the end-user.
//
// Parameters:
//	_size - size of the prism.
//	_material - material that the prism will use.
RDE_FUNC rde_mesh* rde_rendering_mesh_create_prism(rde_vec_3F _size, rde_material* _material);

// Function: rde_rendering_mesh_create_sphere
// Creates a 3D sphere mesh. This must be freed by the end-user.
//
// Parameters:
//	_radius - radius of the sphere.
//	_material - material that the sphere will use.
RDE_FUNC rde_mesh* rde_rendering_mesh_create_sphere(float _radius, rde_material* _material);

// Function: rde_rendering_mesh_create_triangular_pyramid
// Creates a 3D triangular pyramid mesh. This must be freed by the end-user.
//
// Parameters:
//	_size - size of the triangular pyramid.
//	_material - material that the triangular pyramid will use.
RDE_FUNC rde_mesh* rde_rendering_mesh_create_triangular_pyramid(rde_vec_2F _size, rde_material* _material);

// Function: rde_rendering_mesh_create_pyramid
// Creates a 3D pyramid mesh. This must be freed by the end-user.
//
// Parameters:
//	_size - size of the pyramid.
//	_material - material that the pyramid will use.
RDE_FUNC rde_mesh* rde_rendering_mesh_create_pyramid(rde_vec_3F _size, rde_material* _material);

// Function: rde_rendering_mesh_get_data
// Returns read-only data of a mesh.
//
// Parameters:
//	_mesh - mesh to get the data from.
RDE_FUNC rde_mesh_data rde_rendering_mesh_get_data(rde_mesh* _mesh);

// Function: rde_rendering_mesh_destroy
// Destroys a 3D mesh and frees allocations and OpenGL data. Sets pointer to NULL automatically.
RDE_FUNC void rde_rendering_mesh_destroy(rde_mesh* _mesh, bool _delete_allocated_buffers);

#if defined(RDE_OBJ_MODULE) || defined(RDE_FBX_MODULE)
// Function: rde_rendering_model_load
// Loads a 3D model into the GPU. The accepted formats depends on the modules compiled with RDE library. By default, OBJ, FBX and GLTF
//
// Parameters:
//	_model_path - path to the 3D model.
RDE_FUNC rde_model* rde_rendering_model_load(const char* _model_path);

// Function: rde_rendering_model_get_vertices_count
// Returns the total amount of vertices. If the model is made by several sub-meshes, all sub-meshes vertices are count.
//
// Parameters:
//	_model - model to count vertices.
RDE_FUNC uint rde_rendering_model_get_vertices_count(rde_model* _model);

// Function: rde_rendering_model_set_light_data
// Sets the lighting data of the model.
//
// Parameters:
//	_model - model to set the lighting data.
//	_light_data - group of directional, diffuse and specular light data.
RDE_FUNC void rde_rendering_model_set_light_data(rde_model* _model, rde_material_light_data _light_data);

// Function: rde_rendering_model_get_light_data
// Returns the current light data configuration.
//
// Parameters:
//	_model - model to get the light data.
RDE_FUNC rde_material_light_data rde_rendering_model_get_light_data(rde_model* _model);

// Function: rde_rendering_model_get_data
// Returns read-only data of a model. An allocation is made by this method, to fill an array, this must be freed by the end-user.
//
// Parameters:
//	_model - model to get the data from.
RDE_FUNC rde_model_data rde_rendering_model_get_data(rde_model* _model);

// Function: rde_rendering_model_unload
// Unloads a model and returns it to the pool. Pointer is set to NULL automatically.
//
// Parameters:
//	_model - model to get the data from.
RDE_FUNC void rde_rendering_model_unload(rde_model* _model);
#endif

// Function: rde_rendering_3d_begin_drawing
// Starts a 3D drawing batch. Must be called before rendering anything 3D. More than one call can be done on a frame. No nested calls to this function can be made. Any kind of texture can be rendered with
// a single call to this method, if models or meshes cannot be batched, this function and <rde_rendering_3d_end_drawing> handle automatic flushes. Only RDE_CAMERA_TYPE_PERSPECTIVE is allowed.
//
// Parameters:
//	_camera - camera that will render the scene.
//	_window - window where the scene will be rendered.
// _draw_wireframe_over_mesh - if set to true, a wireframe is rendered on top of the models. Might affect performance.
RDE_FUNC void rde_rendering_3d_begin_drawing(rde_camera* _camera, rde_window* _window, bool _draw_wireframe_over_mesh);

// Function: rde_rendering_3d_draw_point
// Draws a 3D point on the screen.
//
// Parameters:
//	_position - position of the point. (0, 0, 0) is the middle of the screen.
//	_color - color of the point.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_3d_draw_point(rde_vec_3F _position, rde_color _color, rde_shader* _shader);

// Function: rde_rendering_3d_draw_line
// Draws a 3D line on the screen.
//
// Parameters:
//	_init - starting position of the line. (0, 0, 0) is the middle of the screen.
//	_end - ending position of the line. (0, 0, 0) is the middle of the screen.
//	_color - color of the line.
//	_thickness - thickness of the line. Minimum is 1.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_3d_draw_line(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, unsigned short _thickness, rde_shader* _shader);

// Function: rde_rendering_3d_draw_mesh
// Draws a 3D mesh on the screen.
//
// Parameters:
//	_transform - transform of the texture.
//	_mesh - mesh data.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_3d_draw_mesh(const rde_transform* _transform, rde_mesh* _mesh, rde_shader* _shader);

// Function: rde_rendering_3d_draw_model
// Draws a 3D model on the screen.
//
// Parameters:
//	_transform - transform of the texture.
//	_model - model data.
//	_shader - shader that will be used to render. NULL can be passed and then the default shader will be used.
RDE_FUNC void rde_rendering_3d_draw_model(const rde_transform* _transform, rde_model* _model, rde_shader* _shader);

// Function: rde_rendering_3d_draw_model
// Draws a skybox. This function is called outside of the block <rde_rendering_3d_begin_drawing>/<rde_rendering_3d_end_drawing>.
//
// Parameters:
//	_camera - camera that will draw the skybox.
RDE_FUNC void rde_rendering_3d_draw_skybox(rde_camera* _camera);

// Function: rde_rendering_3d_end_drawing
// Ends a drawing batch. A previous <rde_rendering_3d_begin_drawing> is mandatory to use this function. Flushes everything into the GPU.
RDE_FUNC void rde_rendering_3d_end_drawing();

// Function: rde_rendering_lighting_set_directional_light
// Sets the directional light data. Directiona light illuminates in all directions, infinitely.
//
// Parameters:
//	_directional_light - directional light data.
RDE_FUNC void rde_rendering_lighting_set_directional_light(rde_directional_light _directional_light);

// Function: rde_rendering_lighting_add_point_light
// Adds a new point light to the scene. Point lights illuminates in all directions, but with a maximum radius.
//
// Parameters:
//	_point_light - point light to add.
RDE_FUNC void rde_rendering_lighting_add_point_light(rde_point_light* _point_light);

// Function: rde_rendering_lighting_add_spot_light
// Adds a new spot light to the scene. Spot lights illuminates like a lantern.
//
// Parameters:
//	_spot_light - spot light to add.
RDE_FUNC void rde_rendering_lighting_add_spot_light(rde_spot_light* _spot_light);

// Function: rde_rendering_lighting_get_directional_light
// Returns the current data of the directional light.
RDE_FUNC rde_directional_light rde_rendering_lighting_get_directional_light();

// Function: rde_rendering_skybox_load
// Loads a skybox into GPU. Returns an id to use it.
//
// Parameters:
//	_texture_paths - array of strings containing the paths to the 6 needed images. Order is right, left, bottom, top, front, back.
RDE_FUNC rde_skybox_id rde_rendering_skybox_load(const char* _texture_paths[6]);

// Function: rde_rendering_skybox_use
// Enables a specific skybox to render. Only one skybox can be enable at a time. This must be freed by the end-user.
//
// Parameters:
//	_skybox_id - skybox to enable.
RDE_FUNC void rde_rendering_skybox_use(rde_skybox_id _skybox_id);

// Function: rde_rendering_skybox_unload
// Unloads a skybox from memory and frees allocations and OpenGL data.
//
// Parameters:
//	_skybox_id - skybox to unload.
RDE_FUNC void rde_rendering_skybox_unload(rde_skybox_id _skybox_id);

// Function: rde_rendering_set_antialiasing
// Enables antialiasing with an specific level. This function CANNOT BE CALLED on on_render callback. Some devices may not support this feature. The greater the MSAA, the more resources it consumes.
//
// Parameters:
//	_window - window to make calculations.
//	_antialiasing - antialiasing level.
RDE_FUNC void rde_rendering_set_antialiasing(rde_window* _window, RDE_ANTIALIASING_ _antialiasing);

// Function: rde_rendering_get_current_antialiasing
// Returns the level of antialiasing.
RDE_FUNC RDE_ANTIALIASING_ rde_rendering_get_current_antialiasing();

// Function: rde_rendering_shadows_begin
// Works like <rde_rendering_3d_begin_drawing> but it renders shadows of everything inside.
//
// Parameters:
//	_window - window to render the scene to.
//	_camera - camera to render the scene with.
RDE_FUNC void rde_rendering_shadows_begin(rde_window* _window, rde_camera* _camera);

// Function: rde_rendering_shadows_end
// Works like <rde_rendering_3d_end_drawing> but it renders shadows of everything inside.
RDE_FUNC void rde_rendering_shadows_end();

// =================================================================== AUDIO ===================================================================

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

// =================================================================== FILE SYSTEM ===================================================================

// Function: rde_file_open
// Opens a file in a specific mode and returns a handle to the file. This must be closed by the end-user.
//
// Parameters:
//	_file_path - path to the file to open.
//	_file_mode - mode to open the file.
RDE_FUNC rde_file_handle* rde_file_open(const char* _file_path, RDE_FILE_MODE_ _file_mode);

// Function: rde_file_read_full_file
// Reads a the full file (as text) provided by the file handle. Returned text must be freed by end-users.
//
// Parameters:
//	_file_handle - opened file handle.
//	_output_file_size - output parameter, will fill the size of the file. Can be NULL.
RDE_FUNC char* rde_file_read_full_file(rde_file_handle* _file_handle, uint* _output_file_size);

// Function: rde_file_read_full_file_bytes
// Reads a the full file (as bytes) provided by the file handle. Returned text must be freed by end-users.
//
// Parameters:
//	_file_handle - opened file handle.
//	_output_file_size - output parameter, will fill the size of the file. Can be NULL.
RDE_FUNC unsigned char* rde_file_read_full_file_bytes(rde_file_handle* _file_handle, uint* _output_file_size);

// Function: rde_file_read_line
// Reads a specific line provided by the file handle. Returned text must be freed by end-users.
//
// Parameters:
//	_file_handle - opened file handle.
//	_line - line of the file to read. Index starts at 0 and must be >= 0.
RDE_FUNC char* rde_file_read_line(rde_file_handle* _file_handle, uint _line);

// Function: rde_file_read_chunk
// Reads a specific chunck provided by the file handle. Returned text must be freed by end-users.
//
// Parameters:
//	_file_handle - opened file handle.
//	_begin_byte - beginning of the chunk, included. Index starts at 0 and must be >= 0.
//	_end_byte - end of the chunk, included. Index starts at 0 and must be >= 0.
RDE_FUNC char* rde_file_read_chunk(rde_file_handle* _file_handle, uint _begin_byte, uint _end_byte);

// Function: rde_file_write
// Writes data to the provided file handle. This will erase previous contents.
//
// Parameters:
//	_file_handle - opened file handle.
//	_bytes - amount of bytes to write.
//	_data - text to write.
RDE_FUNC void rde_file_write(rde_file_handle* _file_handle, uint _bytes, const char* _data);

// Function: rde_file_write_to_line
// Writes data to a specific line of the provided file handle. This will erase previous contents of the line.
//
// Parameters:
//	_file_handle - opened file handle.
//	_bytes - amount of bytes to write.
//	_data - text to write.
//	_line - line of the file to write. Index starts at 0 and must be >= 0.
RDE_FUNC void rde_file_write_to_line(rde_file_handle* _file_handle, uint _bytes, const char* _data, uint _line);

// Function: rde_file_append
// Appends data to a specific point of the provided file handle.
//
// Parameters:
//	_file_handle - opened file handle.
//	_append_byte - point to insert data. -1 means at the end of the file.
//	_bytes - amount of bytes to write.
//	_data - text to write.
RDE_FUNC void rde_file_append(rde_file_handle* _file_handle, uint _append_byte, uint _bytes, const char* _data);

// Function: rde_file_clear_content
// Erases all the contents of a file.
//
// Parameters:
//	_file_handle - opened file handle to clear.
RDE_FUNC void rde_file_clear_content(rde_file_handle* _file_handle);

// Function: rde_file_exists
// Returns if a file exists on a specific path.
//
// Parameters:
//	_file_path - path to the file.
RDE_FUNC bool rde_file_exists(const char* _file_path);

// Function: rde_file_delete
// Deletes a file on a specific path.
//
// Parameters:
//	_file_path - path to the file.
RDE_FUNC void rde_file_delete(const char* _file_path);

// Function: rde_file_move
// Moves a file on a specific path.
//
// Parameters:
//	_file_path - path to the file.
RDE_FUNC void rde_file_move(const char* _file_path, const char* _new_file_path);

// Function: rde_file_close
// Closes an opened file and releases allocated data.
//
// Parameters:
//	_file_path - path to the file.
RDE_FUNC void rde_file_close(rde_file_handle* _file_handle);

// Function: rde_file_free_read_text
// Frees any form of read text by rde_file_ functions.
//
// Parameters:
//	_file_handle - opened file handle.
RDE_FUNC void rde_file_free_read_text(rde_file_handle* _file_handle);

// Function: rde_file_free_read_bytes
// Frees any form of read bytes by rde_file_ functions.
//
// Parameters:
//	_file_handle - opened file handle.
RDE_FUNC void rde_file_free_read_bytes(rde_file_handle* _file_handle);



// =================================================================== PHYSICS ===================================================================

#ifdef RDE_PHYSICS_MODULE
// Function: rde_jolt_draw_debug_shapes
// Draws debug shapes of the colliders of the physics system. This function is not optmized and is just for debugging purposes. This function must be called
// OUTSIDE of a block <rde_rendering_3d_begin_drawing>/<rde_rendering_3d_end_drawing>.
//
// Parameters:
//	_window - window to render the debug shapes.
//	_camera - camera to render the debug shapes with.
RDE_FUNC void rde_jolt_draw_debug_shapes(rde_window* _window, rde_camera* _camera);
#endif



// =================================================================== ERROR ===================================================================

// Function: rde_critical_error
// If the condition is not true, makes the application crash. After making it crash, it cleans all the resources. Will print a stacktrace to the error.
//
// Parameters:
//	_condition - condition to meet, must be true for the application not to crash.
//	_fmt - formatted error message.
RDE_FUNC void rde_critical_error(bool _condition, const char* _fmt, ...);



// =================================================================== ANDROID ===================================================================

#if RDE_IS_ANDROID()
// Function: rde_android_get_native_window
// Returns the Android native window.
RDE_FUNC ANativeWindow* rde_android_get_native_window();
#endif


// =================================================================== OTHER ===================================================================

#ifdef RDE_GOD
#include "private_structs.c"
#endif

#ifdef RDE_NO_NAMESPACES
#define create_probability() 					rde_struct_create_probability()
#define create_camera(_camera_type) 			rde_struct_create_camera(_camera_type)
#define create_event_window() 					rde_struct_create_event_window()
#define create_event_display() 					rde_struct_create_event_display()
#define create_event_key() 						rde_struct_create_event_key()
#define create_event_mouse() 					rde_struct_create_event_mouse()
#define create_event_controller() 				rde_struct_create_event_controller()
#define create_event_mobile_pinch() 			rde_struct_create_event_mobile_pinch()
#define create_event_mobile() 					rde_struct_create_event_mobile()
#define create_event_drag_and_drop() 			rde_struct_create_event_drag_and_drop()
#define create_event_data() 					rde_struct_create_event_data()
#define create_event() 							rde_struct_create_event()
#define create_end_user_mandatory_callbacks() 	rde_struct_create_end_user_mandatory_callbacks()
#define create_texture_parameters() 			rde_struct_create_texture_parameters()
#define create_material_light_data() 			rde_struct_create_material_light_data()
#define create_material() 						rde_struct_create_material()
#define create_directional_light() 				rde_struct_create_directional_light()
#define create_point_light() 					rde_struct_create_point_light()
#define create_spot_light 						rde_struct_create_spot_light()
#define create_color() 							rde_struct_create_color()
#define create_polygon() 						rde_struct_create_polygon()

#define log_color_inner(_color, _fmt, ...) 		rde_log_color_inner(_color, _fmt, __VA_ARGS__)
#define log_level_inner(_level, _fmt, ...)		rde_log_level_inner(_level, _fmt, __VA_ARGS__)

#define get_int_type(_index, _arr, _arr_size, _fmt, ...) 			rde_arr_s_get_int_type(_index, _arr, _arr_size, _fmt, __VA_ARGS__)
#define get_uint_type(_index, _arr, _arr_size, _fmt, ...) 			rde_arr_s_get_uint_type(_index, _arr, _arr_size, _fmt, __VA_ARGS__)
#define get_size_t_type(_index, _arr, _arr_size, _fmt, ...) 		rde_arr_s_get_size_t_type(_index, _arr, _arr_size, _fmt, __VA_ARGS__)
#define get_short_type(_index, _arr, _arr_size, _fmt, ...) 			rde_arr_s_get_short_type(_index, _arr, _arr_size, _fmt, __VA_ARGS__)
#define get_float_type(_index, _arr, _arr_size, _fmt, ...) 			rde_arr_s_get_float_type(_index, _arr, _arr_size, _fmt, __VA_ARGS__)
#define get_double_type(_index, _arr, _arr_size, _fmt, ...) 		rde_arr_s_get_double_type(_index, _arr, _arr_size, _fmt, __VA_ARGS__)
#define set_int_type(_index, _value, _arr, _arr_size, _fmt, ...) 	rde_arr_s_set_int_type(_index, _value, _arr, _arr_size, _fmt, __VA_ARGS__)
#define set_uint_type(_index, _value, _arr, _arr_size, _fmt, ...) 	rde_arr_s_set_uint_type(_index, _value, _arr, _arr_size, _fmt, __VA_ARGS__)
#define set_size_t_type(_index, _value, _arr, _arr_size, _fmt, ...) rde_arr_s_set_size_t_type(_index, _value, _arr, _arr_size, _fmt, __VA_ARGS__)
#define set_short_type(_index, _value, _arr, _arr_size, _fmt, ...) 	rde_arr_s_set_short_type(_index, _value, _arr, _arr_size, _fmt, __VA_ARGS__)
#define set_float_type(_index, _value, _arr, _arr_size, _fmt, ...) 	rde_arr_s_set_float_type(_index, _value, _arr, _arr_size, _fmt, __VA_ARGS__)
#define set_double_type(_index, _value, _arr, _arr_size, _fmt, ...) rde_arr_s_set_double_type(_index, _value, _arr, _arr_size, _fmt, __VA_ARGS__)

#define get_file_extension(_file_name) 											rde_util_file_get_file_extension(_file_name)
#define sanitaize_path(_path, _sanitized, _sanitized_size) 						rde_util_file_sanitaize_path(_path, _sanitized, _sanitized_size)
#define get_string_width(_str, _font) 											rde_util_font_get_string_width(_str, _font)
#define get_string_size(_str, _font) 											rde_util_font_get_string_size(_str, _font)
#define string_trim(_str) 														rde_util_string_trim(_str)
#define string_starts_with(_str, _prefix) 										rde_util_string_starts_with(_str, _prefix)
#define string_ends_with(_str, _suffix) 										rde_util_string_ends_with(_str, _suffix)
#define string_contains_sub_str(_str, _sub_str, _case_sensitive)				rde_util_string_contains_sub_str(_str, _sub_str, _case_sensitive)
#define string_char_appearences(_str, _char) 									rde_util_string_char_appearences(_str, _char)
#define string_format(_dst, _dst_size, _fmt, ...) 								rde_util_string_format(_dst, _dst_size, _fmt, __VA_ARGS__)
#define string_to_lower(_dst, _str) 											rde_util_string_to_lower(_dst, _str)
#define string_to_lower_itself(_str) 											rde_util_string_to_lower_itself(_str)
#define string_to_upper(_dst, _str) 											rde_util_string_to_upper(_dst, _str)
#define string_to_upper_itself(_str) 											rde_util_string_to_upper_itself(_str)
#define string_replace_char(_str, _old_char, _new_char) 						rde_util_string_replace_char(_str, _old_char, _new_char)
#define string_replace_chars_all(_str, _old_char, _new_char) 					rde_util_string_replace_chars_all(_str, _old_char, _new_char)
#define string_replace_sub_str(_str, _old_str, _new_str, _output_appearences) 	rde_util_string_replace_sub_str(_str, _old_str, _new_str, _output_appearences)
#define string_split(_str, _split_array, _split_mark) 							rde_util_string_split(_str, _split_array, _split_mark)

#define hash_map_hash_default(_key, _size) rde_util_hash_map_hash_default(_key, _size)
#define hash_map_int_hash(_key) rde_util_hash_map_int_hash(_key)
#define hash_map_str_hash(_key) rde_util_hash_map_str_hash(_key)

#define  set_random_seed(_seed)							rde_math_set_random_seed(_seed)
#define  get_random_int(_min_included, _max_included)	rde_math_get_random_int(_min_included, _max_included)
#define  get_random_float(_min_included, _max_included)	rde_math_get_random_float(_min_included, _max_included)
#define  get_probability(_normalized_chance_to_happen)	rde_math_get_probability(_normalized_chance_to_happen)

#define world_position_to_screen_coordinates_3d(_window, _vec) 	rde_math_convert_world_position_to_screen_coordinates_3d(_window, _vec)
#define world_position_to_screen_coordinates_2d(_window, _vec) 	rde_math_convert_world_position_to_screen_coordinates_2d(_window, _vec)
#define world_size_to_screen_size(_window, _vec) 				rde_math_convert_world_size_to_screen_size(_window, _vec)

#define cross_product(_vec_0, _vec_1) 	rde_math_cross_product(_vec_0, _vec_1);
#define normalize(_vec) 				rde_math_normalize(_vec);

#define clamp_int_type(_value, _min, _max) 		rde_math_clamp_int_type(_value, _min, _max)
#define clamp_uint_type(_value, _min, _max) 	rde_math_clamp_uint_type(_value, _min, _max)
#define clamp_float_type(_value, _min, _max) 	rde_math_clamp_float_type(_value, _min, _max)
#define clamp_double_type(_value, _min, _max) 	rde_math_clamp_double_type(_value, _min, _max)
#define clamp_ulong_type(_value, _min, _max) 	rde_math_clamp_ulong_type(_value, _min, _max)

#define radians_to_degrees(_radians) rde_math_radians_to_degrees(_radians)
#define deegres_to_radians(_degrees) rde_math_degrees_to_radians(_degrees)

#define quaternion_to_euler_degrees(_quaternion) 	rde_math_quaternion_to_euler_degrees(_quaternion)
#define euler_degrees_to_quaternion(_euler_degrees) rde_math_euler_degrees_to_quaternion(_euler_degrees)

#define easing_in_linear(_current_time, _start_value, _change_in_value, _duration) 			rde_math_easing_in_linear(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_quadratic(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_in_quadratic(_current_time, _start_value, _change_in_value, _duration)
#define easing_out_quadratic(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_out_quadratic(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_out_quadratic(_current_time, _start_value, _change_in_value, _duration) 	rde_math_easing_in_out_quadratic(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_cubic(_current_time, _start_value, _change_in_value, _duration) 			rde_math_easing_in_cubic(_current_time, _start_value, _change_in_value, _duration)
#define easing_out_cubic(_current_time, _start_value, _change_in_value, _duration) 			rde_math_easing_out_cubic(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_out_cubic(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_in_out_cubic(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_quartic(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_in_quartic(_current_time, _start_value, _change_in_value, _duration)
#define easing_out_quartic(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_out_quartic(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_out_quartic(_current_time, _start_value, _change_in_value, _duration) 	rde_math_easing_in_out_quartic(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_quintic(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_in_quintic(_current_time, _start_value, _change_in_value, _duration)
#define easing_out_quintic(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_out_quintic(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_out_quintic(_current_time, _start_value, _change_in_value, _duration) 	rde_math_easing_in_out_quintic(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_sine(_current_time, _start_value, _change_in_value, _duration) 			rde_math_easing_in_sine(_current_time, _start_value, _change_in_value, _duration)
#define easing_out_sine(_current_time, _start_value, _change_in_value, _duration) 			rde_math_easing_out_sine(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_out_sine(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_in_out_sine(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_exponential(_current_time, _start_value, _change_in_value, _duration) 	rde_math_easing_in_exponential(_current_time, _start_value, _change_in_value, _duration)
#define easing_out_exponential(_current_time, _start_value, _change_in_value, _duration) 	rde_math_easing_out_exponential(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_out_exponential(_current_time, _start_value, _change_in_value, _duration) rde_math_easing_in_out_exponential(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_circular(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_in_circular(_current_time, _start_value, _change_in_value, _duration)
#define easing_out_circular(_current_time, _start_value, _change_in_value, _duration) 		rde_math_easing_out_circular(_current_time, _start_value, _change_in_value, _duration)
#define easing_in_out_circular(_current_time, _start_value, _change_in_value, _duration) 	rde_math_easing_in_out_circular(_current_time, _start_value, _change_in_value, _duration)

#define create_engine(_argc, _argv, _config_path) 								rde_engine_create_engine(_argc, _argv, _config_path)
#define load_config(_config_path) 												rde_engine_load_config(_config_path)
#define setup_initial_info(_end_user_callbacks) 								rde_setup_initial_info(_end_user_callbacks)
#define set_event_user_callback(_user_event_callback) 							rde_engine_set_event_user_callback(_user_event_callback)
#define logs_supressed() 														rde_engine_logs_supressed()
#define supress_logs(_supress) 													rde_engine_supress_logs(_supress)
#define get_platform() 															rde_engine_get_platform()
#define get_fixed_delta()											 			rde_engine_get_fixed_delta()
#define set_fixed_delta(_fixed_dt) 												rde_engine_set_fixed_delta(_fixed_dt)
#define on_run() 																rde_engine_on_run()
#define is_running() 															rde_engine_is_running()
#define set_running(_running) 													rde_engine_set_running(_running)
#define get_display_size() 														rde_engine_get_display_size()
#define get_available_display_ids(_out_ids) 									rde_engine_get_available_display_ids(_out_ids)
#define switch_window_display(_window, _new_display) 							rde_engine_switch_window_display(_window, _new_display)
#define get_focused_window() 													rde_engine_get_focused_window()
#define is_vsync_active() 														rde_engine_is_vsync_active()
#define set_vsync_active(_vsync) 												rde_engine_set_vsync_active(_vsync)
#define show_message_box(_level, _title, _content, _window) 					rde_engine_show_message_box(_level, _title, _content, _window)
#define destroy_engine()														rde_engine_destroy_engine()

#define transform_load() 														rde_transform_load()
#define transform_unload() 														rde_transform_unload(_transform)
#define transform_get_position(_transform) 										rde_transform_get_position(_transform)
#define transform_set_position(_transform, _position) 							rde_transform_set_position(_transform, _position)
#define transform_get_rotation_degs(_transform) 								rde_transform_get_rotation_degs(_transform)
#define transform_set_rotation(_transform, _rotation_degs) 						rde_transform_set_rotation(_transform, _rotation_degs)
#define transform_get_scale(_transform) 										rde_transform_get_scale(_transform)
#define transform_set_scale(_transform, _scale) 								rde_transform_set_scale(_transform, _scale)
#define transform_get_parent(_transform)	 									rde_transform_get_parent(_transform)
#define transform_set_parent(_transform, _parent) 								rde_transform_set_parent(_transform, _parent)
#define transform_get_children_count(_transform) 								rde_transform_get_children_count(_transform)

#define create_windowos() 																	rde_window_create_window_os()
#define get_window_size(_window) 															rde_window_get_window_size(_window)
#define set_window_size(_window, _size) 													rde_window_set_window_size(_window, _size)
#define get_window_position(_window) 														rde_window_get_window_position(_window)
#define set_window_position(_window, _position) 											rde_window_set_window_position(_window, _position)
#define get_window_title(_window) 															rde_window_get_window_title(_window)
#define set_window_title(_widow, _title) 													rde_window_set_window_title(_window, _title)
#define orientation_is_horizontal(_window) 													rde_window_orientation_is_horizontal(_window)
#define take_screenshot(_window, _position, _size_of_rectangle, _file_name_with_extension) 	rde_window_take_screenshot(_window, _position, _size_of_rectangle, _file_name_with_extension)
#define get_area_of_screen_pixels(_window, _position, _size) 								rde_window_get_area_of_screen_pixels(_window, _position, _size)
#define get_aspect_ratio(_window) 															rde_window_get_aspect_ratio(_window)
#define is_mouse_out_of_window_allowed() 													rde_window_is_mouse_out_of_window_allowed()
#define allow_mouse_out_of_window(_allow) 													rde_window_allow_mouse_out_of_window(_allow_mouse_out_of_window)
#define set_window_icon(_window, _path_to_icon) 											rde_window_set_window_icon(_window, _path_to_icon)
#define get_native_sdl_window_handle(_window) 												rde_window_get_native_sdl_window_handle(_window)
#define get_native_sdl_gl_context_handle(_window) 											rde_window_get_native_sdl_gl_context_handle(_window)
#define destroy_window(_window) 															rde_window_destroy_window(_window)

#define window_consume_events(_event, _window) 												rde_events_window_consume_events(_event, _window)
#define display_consume_events(_event, _window) 											rde_events_display_consume_events(_event, _window)
#define keyboard_consume_events(_event, _window) 											rde_events_keyboard_consume_events(_event, _window)
#define mouse_consume_events(_event, _window) 												rde_events_mouse_consume_events(_event, _window)
#define drag_and_drop_consume_event(_event, _window) 										rde_events_drag_and_drop_consume_events(_event, _window)
#define mobile_consume_events(_event, _window) 												rde_events_mobile_consume_events(_event, _window)
#define is_key_just_pressed(_window, _key) 													rde_events_is_key_just_pressed(_window, _key)
#define is_key_pressed(_window, _key) 														rde_events_is_key_pressed(_window, _key)
#define is_key_just_released(_window, _key)	 												rde_events_is_key_just_released(_window, _key)
#define is_mouse_button_just_pressed(_window, _button) 										rde_events_is_mouse_button_just_pressed(_window, _button)
#define is_mouse_button_pressed(_window, _button) 											rde_events_is_mouse_button_pressed(_window, _button)
#define is_mouse_button_just_released(_window, _button) 									rde_events_is_mouse_button_just_released(_window, _button)
#define mouse_get_scrolled(_window) 														rde_events_mouse_get_scrolled(_window)
#define mouse_get_position(_window) 														rde_events_mouse_get_position(_window)
#define is_mobile_touch_just_pressed(_window, _finger_id) 									rde_events_is_mobile_touch_just_pressed(_window, _finger_id)
#define is_mobile_touch_pressed(_window, _finger_id) 										rde_events_is_mobile_touch_pressed(_window, _finger_id)
#define is_mobile_touch_released(_window, _finger_id) 										rde_events_is_mobile_touch_released(_window, _finger_id)
#define mobile_get_finger_amount(_window) 													rde_events_mobile_get_finger_amount(_window)

#define shader_load(_name, _vertex_code, _fragment_code, _geometry_code, _tessellation_code)rde_rendering_shader_load(_name, _vertex_code, _fragment_code, _geometry_code, _tessellation_code)
#define shader_set_uniform_value_float(_shader, _uniform_name, _type, _data, _transpose) 	rde_rendering_shader_set_uniform_value_float(_shader, _uniform_name, _type, _data, _transpose)
#define shader_set_uniform_value_int(_shader, _uniform_name, _type, _data) 					rde_rendering_shader_set_uniform_value_int(_shader, _uniform_name, _type, _data)
#define shader_set_uniform_value_uint(_shader, _uniform_name, _type, _data) 				rde_rendering_shader_set_uniform_value_uint(_shader, _uniform_name, _type, _data)
#define shader_get_data(_shader) 															rde_rendering_shader_get_data(_shader)
#define shader_get_by_name(_name) 															rde_rendering_shader_get_by_name(_name)
#define shader_unload(_shader) 																rde_rendering_shader_unload(_shader)
#define texture_load(_file_path, _params) 													rde_rendering_texture_load(_file_path, _params)
#define texture_text_load(_file_path) 														rde_rendering_texture_text_load(_file_path)
#define texture_get_data(_texture) 															rde_rendering_texture_get_data(_texture)
#define texture_unload(_texture) 															rde_rendering_texture_unload(_texture)
#define atlas_load(_texture_path) 															rde_rendering_atlas_load(_texture_path)
#define atlas_get_subtexture(_atlas, _texture_name) 										rde_rendering_atlas_get_subtexture(_atlas, _texture_name)
#define atlas_get_data(_atlas) 																rde_rendering_atlas_get_data(_atlas)
#define atlas_unload(_atlas) 																rde_rendering_atlas_unload(_atlas)
#define memory_texture_create(_width, _height, _channels) 									rde_rendering_memory_texture_create(_width, _height, _channels)
#define memory_texture_set_pixel(_memory_texture, _position, _color) 						rde_rendering_memory_texture_set_pixel(_memory_texture, _position, _color)
#define memory_texture_get_pixel(_memory_texture, _position) 								rde_rendering_memory_texture_get_pixel(_memory_texture, _position)
#define memory_texture_gpu_upload(_memory_texture) 											rde_rendering_memory_texture_gpu_upload(_memory_texture)
#define memory_texture_get_pixels(_memory_texture) 											rde_rendering_memory_texture_get_pixels(_memory_texture)
#define memory_texture_destroy(_memory_texture) 											rde_rendering_memory_texture_destroy(_memory_texture)
#define render_texture_create(_width, _height) 												rde_rendering_render_texture_create(_width, _height)
#define render_texture_enable(_render_texture) 												rde_rendering_render_texture_enable(_render_texture)
#define render_texture_disable() 															rde_rendering_render_texture_disable()
#define render_texture_update(_render_texture, _width, _height) 							rde_rendering_render_texture_update(_render_texture, _width, _height)
#define render_texture_destroy(_render_texture) 											rde_rendering_render_texture_destroy(_render_texture)
#define font_load(_font_path) 																rde_rendering_font_load(_font_path)
#define font_get_data(_font) 																rde_rendering_font_get_data(_font)
#define font_unload(_font) 																	rde_rendering_font_unload(_font)
#define set_background_color(_color) 														rde_rendering_set_background_color(_color)
#define 2d_begin_drawing(_camera, _window, _is_hud) 										rde_rendering_2d_begin_drawing(_camera, _window, _is_hud)
#define 2d_draw_point(_position, _color, _shader) 											rde_rendering_2d_draw_point(_position, _color, _shader)
#define 2d_draw_line(_init, _end, _color, _shader) 											rde_rendering_2d_draw_line(_init, _end, _color, _shader)
#define 2d_draw_triangle(_verte_a, _vertex_b, _vertex_c, _color, _shader) 					rde_rendering_2d_draw_triangle(_verte_a, _vertex_b, _vertex_c, _color, _shader)
#define 2d_draw_rectangle(_bottom_left, _top_right, _color, _shader) 						rde_rendering_2d_draw_rectangle(_bottom_left, _top_right, _color, _shader)
#define 2d_draw_rectangle_1(_center, _width, _height, _color, _shader)						rde_rendering_2d_draw_rectangle_1(_center, _width, _height, _color, _shader)
#define 2d_draw_circle(_position, _radius, _color, _shader) 								rde_rendering_2d_draw_circle(_position, _radius, _color, _shader)
#define 2d_draw_polygon(_transform, _polygon, _color, _shader) 								rde_rendering_2d_draw_polygon(_transform, _polygon, _color, _shader)
#define 2d_draw_texture(_transform, _texture, _tint_color, _shader) 						rde_rendering_2d_draw_texture(_transform, _texture, _tint_color, _shader)
#define 2d_draw_memory_texture(_transform, _texture, _tint_color, _shader)					rde_rendering_2d_draw_memory_texture(_transform, _texture, _tint_color, _shader)
#define 2d_draw_text(_transform, _font, _text, _tint_color, _shader) 						rde_rendering_2d_draw_text(_transform, _font, _text, _tint_color, _shader)
#define 2d_end_drawing() 																	rde_rendering_2d_end_drawing()
#define memory_mesh_create(_data) 															rde_struct_memory_mesh_create(_data)
#define mesh_create_cube(_size, _material) 													rde_rendering_mesh_create_cube(_size, _material)
#define mesh_create_prism(_size, _material) 												rde_rendering_mesh_create_prism(_size, _material)
#define mesh_create_sphere(_radius, _material) 												rde_rendering_mesh_create_sphere(_radius, _material)
#define mesh_create_triangular_pyramid(_size, _material) 									rde_rendering_mesh_create_triangular_pyramid(_size, _material)
#define mesh_create_pyramid(_size, _material) 												rde_rendering_mesh_create_pyramid(_size, _material)
#define mesh_get_data(_mesh) 																rde_rendering_mesh_get_data(_mesh)
#define mesh_destroy(_mesh, _delete_allocated_buffers) 										rde_rendering_mesh_destroy(_mesh, _delete_allocated_buffers)

#if defined(RDE_OBJ_MODULE) || defined(RDE_FBX_MODULE)
#define model_load(_model_path) 															rde_rendering_model_load(_model_path)
#define model_get_vertices_count(_model) 													rde_rendering_model_get_vertices_count(_model)
#define model_set_light_data(_model, _light_data) 											rde_rendering_model_set_light_data(_model, _light_data)
#define model_get_light_data(_model)														rde_rendering_model_get_light_data(_model)
#define model_get_data(_model) 																rde_rendering_model_get_data(_model)
#define model_unload(_model) 																rde_rendering_model_unload(_model)
#endif

#define 3d_begin_drawing(_camera, _window, _draw_wireframe_over_mesh) 						rde_rendering_3d_begin_drawing(_camera, _window, _draw_wireframe_over_mesh)
#define 3d_draw_point(_position, _color, _shader) 											rde_rendering_3d_draw_point(_position, _color, _shader)
#define 3d_draw_line(_init, _end, _color, _thickness, _shader) 								rde_rendering_3d_draw_line(_init, _end, _color, _thickness, _shader)
#define 3d_draw_mesh(_transform, _mesh, _shader) 											rde_rendering_3d_draw_mesh(_transform, _mesh, _shader)
#define 3d_draw_model(_transform, _model, _shader) 											rde_rendering_3d_draw_model(_transform, _model, _shader)
#define 3d_draw_skybox(_camera)																rde_rendering_3d_draw_skybox(_camera)
#define 3d_end_drawing()																	rde_rendering_3d_end_drawing()
#define lighting_set_directional_light(_directional_light)									rde_rendering_lighting_set_directional_light(_directional_light)
#define lighting_add_point_light(_point_light)												rde_rendering_lighting_add_point_light(_point_light)
#define lighting_add_spot_light(_spot_light)												rde_rendering_lighting_add_spot_light(_spot_light)
#define lighting_get_directional_light()													rde_rendering_lighting_get_directional_light()
#define skybox_load(_texture_paths)															rde_rendering_skybox_load(_texture_paths)
#define skybox_use(_skybox_id)																rde_rendering_skybox_use(_skybox_id)
#define skybox_unload(_skybox_id)															rde_rendering_skybox_unload(_skybox_id)
#define set_antialiasing(_window, _antialiasing)											rde_rendering_set_antialiasing(_window, _antialiasing)
#define get_current_antialiasing()															rde_rendering_get_current_antialiasing()
#define shadows_begin(_window, _camera)														rde_rendering_shadows_begin(_window, _camera)
#define shadows_end()																		rde_rendering_shadows_end()

#ifdef RDE_AUDIO_MODULE
#define audio_init(_config) 						rde_audio_init(_config)
#define load_sound(_sound_path) 					rde_audio_load_sound(_sound_path)
#define unload_sound(_sound) 						rde_audio_unload_sound(_sound)
#define play_sound(_sound) 							rde_audio_play_sound(_sound)
#define pause_sound(_sound) 						rde_audio_pause_sound(_sound)
#define stop_sound(_sound) 							rde_audio_stop_sound(_sound)
#define restart_sound(_sound) 						rde_audio_restart_sound(_sound)
#define is_sound_playing(_sound) 					rde_audio_is_sound_playing(_sound)
#define set_sound_volume(_sound, _volume) 			rde_audio_set_sound_volume(_sound, _volume)
#define audio_end() 								rde_audio_end()
#endif

#define file_open(_file_path, _file_mode) 							rde_file_open(_file_path, _file_mode)
#define read_full_file(_file_handler, _output_file_size) 			rde_file_read_full_file(_file_handler, _output_file_size)
#define read_full_file_bytes(_file_handler, _output_file_size) 		rde_file_read_full_file_bytes(_file_handler, _output_file_size)
#define read_line(_file_handler, _line) 							rde_file_read_line(_file_handler, _line)
#define read_chunk(_file_handler, _begin_byte, _end_byte) 			rde_file_read_chunk(_file_handler, _begin_byte, _end_byte)
#define write(_file_handler, _bytes, _data) 						rde_file_write(_file_handler, _bytes, _data)
#define write_to_line(_file_handler, _bytes, _data, _line) 			rde_file_write_to_line(_file_handler, _bytes, _data, _line)
#define append(_file_handler, _append_byte, _bytes, _data) 			rde_file_append(_file_handler, _append_byte, _bytes, _data)
#define clear_content(_file_handler) 								rde_file_clear_content(_file_handler)
#define exists(_file_path) 											rde_file_exists(_file_path)
#define delete(_file_path) 											rde_file_delete(_file_path)
#define move(_file_path, _new_file_path) 							rde_file_move(_file_path, _new_file_path)
#define close(_file_handler) 										rde_file_close(_file_handler)
#define free_read_text(_file_handle)							 	rde_file_free_read_text(_file_handle)
#define free_read_bytes(_file_handle) 								rde_file_free_read_bytes(_file_handle)

#ifdef RDE_PHYSICS_MODULE
#define jolt_draw_debug_shapes(_window, _camera)					rde_jolt_draw_debug_shapes(_window, _camera)
#endif

#define critical_error(_condition, _fmt, ...) 						rde_critical_error(_condition, _fmt, __VA_ARGS__)

#if RDE_IS_ANDROID()
#define android_get_native_window()									rde_android_get_native_window();
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
