#ifndef RDE_H
#define RDE_H

// This is needed because of C++ name mangling, if this lib is linked to a C++ project and is compiled
// without extern "C" won't work.
#ifdef __cplusplus
extern "C" {
#endif

// **************************************************************************************
// *							GENERAL INFO ABOUT THE ENGINE							*
// **************************************************************************************
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
//	 us 'rde_rendering_model', whit would be rde::rendering::model and then all methods related to 
//	 models, such as 'rde_rendering_model_load' or 'rde_rendering_model_unload'
//
// - Any function that has _inner_ in the name is just part of the internal computation of the engine,
//	 and will not be exposed to the user API.

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

/// *************************************************************************************************
/// *                                		DEFINES                         						*
/// *************************************************************************************************

/// ========================= NUMERIC CONSTANTS ============================

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
// Default capacity when creating a new dynamic array, by default 10
#define RDE_ARR_DEFAULT_CAPACITY 10
	
/// ============================== SHADERS =================================

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
// Name for the shadow map, it is used to search this shader with <rde_rendering_shader_get_by_name>. By default is 'shadows_shader'.
#define RDE_SHADER_SHADOWS "shadows_shader"

/// ====================== COMPILATION AND EXPORT ==========================

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

#ifdef RDE_NO_NAMESPACES
// Macro: RDE_N
// Creates the C "namespace" system of the library. Can be disabled when compiling the library. This will only expose the short name of the functions.
#define RDE_N(_namespace, _function) _function
#else
#define RDE_N(_namespace, _function) _namespace##_##_function
// Example of usage RDE_FUNC void RDE_N(rde_my_namespace, my_function)(int _a, int _b);
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

/// ====================== PLATFORM SPECIFIC TYPES ==========================

typedef unsigned long ulong;
typedef unsigned int uint;

/// ============================== UTIL ====================================

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

/// ======================= WARNING SILENCER ===============================

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


/// ==================== GENERIC FUNCS AND STRUCTS ==========================

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
	size_t type_size;		\
	_type default_value;	\
} rde_arr_##_type

// Macro: rde_arr_init
// Inits a dynamic array inner fields to the default values.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//	_default_value - the default value that elements should have when created.
//
//	======= C =======
//	rde_arr_A _arr;
//	A _a = {0};
// 	rde_arr_init(_arr, _a);
//	=================
#define rde_arr_init(_dyn_arr, _default_value) 	\
	do {										\
		_dyn_arr.memory = NULL;					\
		_dyn_arr.capacity = 0;					\
		_dyn_arr.used = 0;						\
		_dyn_arr.type_size = 0;					\
		_dyn_arr.default_value = _default_value;\
	} while(0)

// Macro: rde_arr_new_with_capacity
// Allocates the data for the dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//	_type - the type of data that the array will hold.
//	_capacity - the initial amount of elements of the array.
//
//	======= C =======
//	rde_arr_new_with_capacity(_arr, A, 10);
//	=================
#define rde_arr_new_with_capacity(_dyn_arr, _type, _capacity) 																			\
	do {																																\
		if(_dyn_arr.memory != NULL) {																									\
			free(_dyn_arr.memory);																										\
		}																																\
																																		\
		_dyn_arr.memory = (_type*)malloc(sizeof(_type) * _capacity);																	\
		rde_critical_error(_dyn_arr.memory == NULL, "Bytes %d could not be allocated for %s", sizeof(_type) * _capacity, "rde_arr new");\
		_dyn_arr.used = 0;																												\
		_dyn_arr.capacity = _capacity;																									\
		_dyn_arr.type_size = sizeof(_type);																								\
	} while(0)

// Macro: rde_arr_new
// Allocates the data for the dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//	_type - the type of data that the array will hold.
//
//	======= C =======
//	rde_arr_new(_arr, A);
//	=================
#define rde_arr_new(_dyn_arr, _type) rde_arr_new_with_capacity(_dyn_arr, _type, RDE_ARR_DEFAULT_CAPACITY)
	
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
#define rde_arr_add(_dyn_arr, _element)																												\
	do {																																			\
		rde_critical_error(_dyn_arr.memory == NULL, "Uninitialized rde_arr");																		\
		if(_dyn_arr.used + 1 == _dyn_arr.capacity) {																								\
			_dyn_arr.capacity = _dyn_arr.capacity RDE_ARR_OPERAND_INCREASE RDE_ARR_AMOUNT_INCREASE;													\
			void* _new_memory = (void*)realloc(_dyn_arr.memory, _dyn_arr.type_size * _dyn_arr.capacity);											\
			if(_new_memory == NULL) {																												\
				free(_dyn_arr.memory);																												\
				rde_critical_error(true, "Bytes %d could not be allocated for %s", sizeof(_dyn_arr.type_size) * _dyn_arr.capacity, "rde_arr add");	\
			}																																		\
			_dyn_arr.memory = _new_memory;																											\
		}																																			\
		_dyn_arr.memory[_dyn_arr.used++] = _element;																								\
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
#define rde_arr_remove(_dyn_arr, _index)																							\
	do {																															\
		rde_critical_error(_dyn_arr.memory == NULL, "Uninitialized rde_arr");														\
		rde_critical_error(_index < 0 || _index >= _dyn_arr.capacity, "Arr[%d] out of bounds, max %d", _index, _dyn_arr.capacity);	\
		memmove(_dyn_arr.memory + _index, _dyn_arr.memory + (_index + 1), _dyn_arr.capacity - _index);								\
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
#define rde_arr_get_element(_dyn_arr, _index, _value) 																				\
	do {																															\
		rde_critical_error(_dyn_arr.memory == NULL, "Uninitialized rde_arr");														\
		rde_critical_error(_index < 0 || _index >= _dyn_arr.capacity, "Arr[%d] out of bounds, max %d", _index, _dyn_arr.capacity);	\
		_value = _dyn_arr.memory[_index];																							\
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
#define rde_arr_get_element_ptr(_dyn_arr, _index, _value) 																			\
	do {																															\
		rde_critical_error(_dyn_arr.memory == NULL, "Uninitialized rde_arr");														\
		rde_critical_error(_index < 0 || _index >= _dyn_arr.capacity, "Arr[%d] out of bounds, max %d", _index, _dyn_arr.capacity);	\
		_value = &_dyn_arr.memory[_index];																							\
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
#define rde_arr_get_total_capacity(_dyn_arr) _dyn_arr.capacity
	
// Macro: rde_arr_get_used_capacity
// Returns the used capacity of the dynamic array.
//
// Parameters:
//	_dyn_arr - the dynamic array.
//
//	======= C =======
//	ulong  _used_capacity = rde_arr_get_used_capacity(_arr);
//	=================
#define rde_arr_get_used_capacity(_dyn_arr) _dyn_arr.used

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
#define rde_arr_set_element(_dyn_arr, _index, _element)																				\
	do {																															\
		rde_critical_error(_dyn_arr.memory == NULL, "Uninitialized rde_arr");														\
		rde_critical_error(_index < 0 || _index >= _dyn_arr.capacity, "Arr[%d] out of bounds, max %d", _index, _dyn_arr.capacity);	\
		_dyn_arr.memory[_index] = _element;																							\
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
		rde_critical_error(_dyn_arr.memory == NULL, "Uninitialized rde_arr");	\
		for(unsigned int _i = 0; _i < _dyn_arr.capacity; _i++) {				\
			_dyn_arr.memory[_i] = _dyn_arr.defaul_value;						\
		}																		\
		_dyn_arr.used = 0;														\
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
		if(_dyn_arr.memory != NULL) {	\
			free(_dyn_arr.memory);		\
			_dyn_arr.memory = NULL;		\
		}								\
		_dyn_arr.capacity = 0;			\
		_dyn_arr.used = 0;				\
		_dyn_arr.type_size = 0;			\
	} while(0)
	
/// *************************************************************************************************
/// *                                		  ENUMS                         						*
/// *************************************************************************************************

/// ================================= STATE =================================

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

/// ================================ EVENTS ================================

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



/// =============================== KEYBOARD ================================

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



/// ================================ MOUSE ==================================

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



/// ============================== CONTROLLER ===============================

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



/// =============================== PLATFORM ================================

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



/// ============================== RENDERING ================================

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

/// ============================= FILE SYSTEM ===============================

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

/// =============================== LOG ============================

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


/// =============================== PHYSICS 3D ============================

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

/// *************************************************************************************************
/// *                                		STRUCSTS                         						*
/// *************************************************************************************************

// Group: Structs & Callbacks

/// ================================= MATH ==================================

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

/// ================================= ENGINE ===========================

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

/// ============================== CAMERA ===================================

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

/// ============================== EVENTS ===================================

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

/// ================== CALLBACKS AND FUNCTION POINTERS ======================

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

/// ============================== ENGINE ===================================

typedef struct {
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

} rde_engine_heap_allocs_config;

typedef struct {
	uint max_amount_of_point_lights;
	uint max_amount_of_spot_lights;
} rde_illumination_config;

typedef struct {
	uint temp_allocator_bytes; // recommended at least 10-15Mb, if not set, malloc/free will be used on runtime
	uint max_amout_of_allowed_jobs;
	uint max_amount_of_physics_barriers;
	int max_amount_of_threads;
	uint max_amount_of_bodies;
	uint max_amount_of_mutexes;
	uint max_amount_of_contact_constraints;
} rde_physics_3d_config;

typedef struct {
	rde_engine_heap_allocs_config heap_allocs_config;
	
	rde_illumination_config illumination_config;

#ifdef RDE_PHYSICS_MODULE
	rde_jolt_init_config jolt_config;
#endif

} rde_engine_init_info;

/// ============================ RENDERING ==================================

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
// Data about the colors and direction of the directional light.
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

typedef struct {
	rde_vec_3F position;
	rde_vec_3F ambient_color;
	rde_vec_3F diffuse_color;
	rde_vec_3F specular_color;
	float constant;
	float linear;
	float quadratic;
} rde_point_light;

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

/// ============================ AUDIO ==================================

#ifdef RDE_AUDIO_MODULE
typedef struct rde_sound rde_sound;
typedef struct {
	void* user_data;
	unsigned short channels;
	uint rate;
} rde_sound_config;
RDE_FUNC rde_sound_config rde_struct_create_audio_config();
#endif

/// *************************************************************************************************
/// *                                GLOBAL VARIABLES                         						*
/// *************************************************************************************************

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

/// *************************************************************************************************
/// *                                		FUNCTIONS                         						*
/// *************************************************************************************************

// Group: Constructors

/// ============================ CONSTRUCTORS ===============================

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

// Constructor: rde_struct_texture_parameters
RDE_FUNC rde_texture_parameters rde_struct_texture_parameters();

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

/// ============================ LOG ========================================

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

/// ============================ UTIL =======================================

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
//	_string - string to messure.
//	_font - font that will render it.
RDE_FUNC uint rde_util_font_get_string_width(const char* _string, const rde_font* _font);
RDE_FUNC rde_vec_2I rde_util_font_get_string_size(const char* _string, const rde_font* _font);

RDE_FUNC char* rde_util_string_trim(char* _s);
RDE_FUNC bool rde_util_string_starts_with(const char* _string, const char* _prefix);
RDE_FUNC bool rde_util_string_ends_with(const char* _string, const char* _suffix);
RDE_FUNC bool rde_util_string_contains_substring(const char* _string, const char* _substring, bool _case_sensitive);
RDE_FUNC uint rde_util_string_char_appearences(const char* _string, char _char);
RDE_FUNC void rde_util_string_concat(char* _string, uint _size, const char* _fmt, ...);
RDE_FUNC void rde_util_string_to_lower(char* _destination, const char* _string);
RDE_FUNC void rde_util_string_to_lower_itself(char* _string);
RDE_FUNC void rde_util_string_to_upper(char* _destination, const char* _string);
RDE_FUNC void rde_util_string_to_upper_itself(char* _string);
RDE_FUNC void rde_util_string_replace_char(char* _string, char _old, char _new);
RDE_FUNC void rde_util_string_replace_chars_all(char* _string, char _old, char _new);
RDE_FUNC char* rde_util_string_replace_substring(char* _string, char* _old_string, char* _new_string, int* _output_appearences);
RDE_FUNC void rde_util_string_replace_substring_no_alloc(char* _src, char* _dst, uint _dst_size, char* _old_string, char* _new_string, int* _output_appearences);
RDE_FUNC uint rde_util_string_split(char* _string, char*** _split_array, char _split_mark);

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
	RDE_FUNC _type rde_math_clamp_##_type(_type _value, _type _min, _type _max);

RDE_DECLARE_CLAMP_FUNCS(int)
RDE_DECLARE_CLAMP_FUNCS(uint)
RDE_DECLARE_CLAMP_FUNCS(float)
RDE_DECLARE_CLAMP_FUNCS(double)
RDE_DECLARE_CLAMP_FUNCS(long)
RDE_DECLARE_CLAMP_FUNCS(ulong)

RDE_FUNC float rde_math_radians_to_degrees(float _radians);
RDE_FUNC float rde_math_degrees_to_radians(float _degrees);

RDE_FUNC rde_vec_3F rde_math_quaternion_to_euler_degrees(rde_quaternion _quaternion);
RDE_FUNC rde_quaternion rde_math_euler_degrees_to_quaternion(rde_vec_3F _euler_degrees);

#define RDE_DECLARE_EASING_FUNCS(_form, _type) \
	RDE_FUNC float rde_math_easing_##_form##_##_type(float _current_time, float _start_value, float _change_in_value, float _duration);

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


RDE_FUNC rde_window* rde_engine_create_engine(int _argc, char** _argv, const char* _config_path);

RDE_FUNC rde_engine_init_info rde_engine_load_config(const char* _config_path);

RDE_FUNC void rde_setup_initial_info(rde_end_user_mandatory_callbacks _end_user_callbacks); /// Sets up some basic info the engine needs, call this BEFORE rde_engine_create_engine

RDE_FUNC void rde_engine_set_event_user_callback(rde_event_func _user_event_callback);

RDE_FUNC bool rde_engine_logs_supressed();
RDE_FUNC void rde_engine_supress_logs(bool _supress);

RDE_FUNC RDE_PLATFORM_TYPE_ rde_engine_get_platform();

RDE_FUNC float rde_engine_get_fixed_delta();
RDE_FUNC void rde_engine_set_fixed_delta(float _fixed_dt);

RDE_FUNC void rde_engine_on_run();

RDE_FUNC bool rde_engine_is_running();
RDE_FUNC void rde_engine_set_running(bool _running);

RDE_FUNC rde_vec_2I rde_engine_get_display_size();

RDE_FUNC void rde_engine_get_available_display_ids(uint* _out_ids);
RDE_FUNC void rde_engine_switch_window_display(rde_window* _window, uint _new_display);

RDE_FUNC rde_window* rde_engine_get_focused_window();

RDE_FUNC void rde_engine_use_rde_2d_physics_system(bool _use);

RDE_FUNC bool rde_engine_is_vsync_active();
RDE_FUNC void rde_engine_set_vsync_active(bool _vsync);

RDE_FUNC void rde_engine_show_message_box(RDE_LOG_LEVEL_ _level, const char* _title, const char* _content, rde_window* _window);

RDE_FUNC rde_transform* rde_engine_transform_load();
RDE_FUNC void rde_engine_transform_unload(rde_transform* _transform);

RDE_FUNC rde_vec_3F  rde_engine_transform_get_position(rde_transform* _transform);
RDE_FUNC void rde_engine_transform_set_position(rde_transform* _transform, rde_vec_3F _position);

RDE_FUNC rde_vec_3F  rde_engine_transform_get_rotation_degs(rde_transform* _transform);
RDE_FUNC void rde_engine_transform_set_rotation(rde_transform* _transform, rde_vec_3F _rotation_degs);

RDE_FUNC rde_vec_3F  rde_engine_transform_get_scale(rde_transform* _transform);
RDE_FUNC void rde_engine_transform_set_scale(rde_transform* _transform, rde_vec_3F _scale);

RDE_FUNC rde_transform* rde_engine_trasnform_get_parent(rde_transform* _transform);
RDE_FUNC void rde_engine_transform_set_parent(rde_transform* _transform, rde_transform* _parent);

RDE_FUNC uint rde_engine_transform_get_children_count(rde_transform* _transform);

RDE_FUNC void rde_engine_destroy_engine();

/// ============================ WINDOW =====================================

RDE_FUNC rde_window* rde_window_create_window_os();

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

RDE_FUNC void rde_events_window_consume_events(rde_event* _event, rde_window* _window);
RDE_FUNC void rde_events_display_consume_events(rde_event* _event, rde_window* _window);
RDE_FUNC void rde_events_keyboard_consume_events(rde_event* _event, rde_window* _window);
RDE_FUNC void rde_events_mouse_consume_events(rde_event* _event, rde_window* _window);
RDE_FUNC void rde_events_drag_and_drop_consume_events(rde_event* _event, rde_window* _window);
RDE_FUNC void rde_events_mobile_consume_events(rde_event* _event, rde_window* _window);

RDE_FUNC bool rde_events_is_key_just_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key);
RDE_FUNC bool rde_events_is_key_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key);
RDE_FUNC bool rde_events_is_key_just_released(rde_window* _window, RDE_KEYBOARD_KEY_ _key);

RDE_FUNC bool rde_events_is_mouse_button_just_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button);
RDE_FUNC bool rde_events_is_mouse_button_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button);
RDE_FUNC bool rde_events_is_mouse_button_just_released(rde_window* _window, RDE_MOUSE_BUTTON_ _button);
RDE_FUNC rde_vec_2F rde_events_mouse_get_scrolled(rde_window* _window);
RDE_FUNC rde_vec_2I rde_events_mouse_get_position(rde_window* _window);

RDE_FUNC bool rde_events_is_mobile_touch_just_pressed(rde_window* _window, uint _finger_id);
RDE_FUNC bool rde_events_is_mobile_touch_pressed(rde_window* _window, uint _finger_id);
RDE_FUNC bool rde_events_is_mobile_touch_released(rde_window* _window, uint _finger_id);
RDE_FUNC uint rde_events_mobile_get_finger_amount(rde_window* _window);
RDE_FUNC 

/// ============================ RENDERING ==================================

RDE_FUNC rde_shader* rde_rendering_shader_load(const char* _name, const char* _vertex_code, const char* _fragment_code);
RDE_FUNC void rde_rendering_shader_set_uniform_value_float(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_FV_ _type, float* _data, bool _transpose);
RDE_FUNC void rde_rendering_shader_set_uniform_value_int(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_IV_ _type, int* _data);
RDE_FUNC void rde_rendering_shader_set_uniform_value_uint(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_UIV_ _type, uint* _data);
RDE_FUNC rde_shader_data rde_rendering_shader_get_data(rde_shader* _shader);
RDE_FUNC rde_shader* rde_rendering_shader_get_by_name(const char* _name);
RDE_FUNC void rde_rendering_shader_unload(rde_shader* _shader);

RDE_FUNC rde_texture* rde_rendering_texture_load(const char* _file_path, const rde_texture_parameters* _params); // Pass NULL to params for default parameters
RDE_FUNC rde_texture* rde_rendering_texture_text_load(const char* _file_path);
RDE_FUNC rde_texture_data rde_rendering_texture_get_data(rde_texture* _texture);
RDE_FUNC void rde_rendering_texture_unload(rde_texture* _texture);

RDE_FUNC rde_atlas* rde_rendering_atlas_load(const char* _texture_path); // There will be two files with the same name and different extensions to load, just end the path with the atlas name without extension
RDE_FUNC rde_texture* rde_rendering_atlas_get_subtexture(rde_atlas* _atlas, const char* _texture_name);
RDE_FUNC rde_atlas_data rde_rendering_atlas_get_data(rde_atlas* _atlas);
RDE_FUNC void rde_rendering_atlas_unload(rde_atlas* _atlas);

RDE_FUNC rde_texture* rde_rendering_memory_texture_create(uint _width, uint _height, int _channels);
RDE_FUNC void rde_rendering_memory_texture_set_pixel(rde_texture* _memory_texture, rde_vec_2I _position, rde_color _color);
RDE_FUNC rde_color rde_rendering_memory_texture_get_pixel(rde_texture* _memory_texture, rde_vec_2I _position);
RDE_FUNC void rde_rendering_memory_texture_gpu_upload(rde_texture* _memory_texture);
RDE_FUNC unsigned char* rde_rendering_memory_texture_get_pixels(rde_texture* _memory_texture);
RDE_FUNC void rde_rendering_memory_texture_destroy(rde_texture* _memory_texture);

RDE_FUNC rde_render_texture* rde_rendering_render_texture_create(uint _width, uint _height);
RDE_FUNC void rde_rendering_render_texture_enable(rde_render_texture* _render_texture);
RDE_FUNC void rde_rendering_render_texture_disable();
RDE_FUNC void rde_rendering_render_texture_update(rde_render_texture* _render_texture, uint _width, uint _height);
RDE_FUNC void rde_rendering_render_texture_destroy(rde_render_texture* _render_texture);

RDE_FUNC rde_font* rde_rendering_font_load(const char* _font_path); // There will be two files with the same name and different extensions to load, just end the path with the font name without extension
RDE_FUNC rde_font_data rde_rendering_font_get_data(rde_font* _font);
RDE_FUNC void rde_rendering_font_unload(rde_font* _font);

RDE_FUNC void rde_rendering_set_background_color(const rde_color _color);

RDE_FUNC void rde_rendering_2d_begin_drawing(rde_camera* _camera, rde_window* _window, bool _is_hud);
RDE_FUNC void rde_rendering_2d_draw_point(rde_vec_2F _position, rde_color _color, rde_shader* _shader); /// Draws a point in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_2d_draw_line(rde_vec_2F _init, rde_vec_2F _end, rde_color _color, rde_shader* _shader); /// Draws a batched line in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_2d_draw_triangle(rde_vec_2F _verte_a, rde_vec_2F _vertex_b, rde_vec_2F _vertex_c, rde_color _color, rde_shader* _shader); /// Draws a batched triangle in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_2d_draw_rectangle(rde_vec_2F _bottom_left, rde_vec_2F _top_right, rde_color _color, rde_shader* _shader); /// Draws a batched rectangle in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_2d_draw_circle(rde_vec_2F _position, float _radius, rde_color _color, rde_shader* _shader); /// Draws a batched circle in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_2d_draw_polygon(const rde_transform* _transform, const rde_polygon* _polygon, rde_color _color, const rde_shader* _shader);  /// Draws a batched polygon in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_2d_draw_texture(const rde_transform* _transform, const rde_texture* _texture, rde_color _tint_color, rde_shader* _shader); /// Draws a batched quad texture in 2D space, pass RDE_COLOR_WHITE to _tint_color for no tint effects, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_2d_draw_memory_texture(const rde_transform* _transform, rde_texture* _texture, rde_color _tint_color, rde_shader* _shader); /// Draws a batched quad texture in 2D space, pass RDE_COLOR_WHITE to _tint_color for no tint effects, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_2d_draw_text(const rde_transform* _transform, const rde_font* _font, const char* _text, rde_color _tint_color, rde_shader* _shader); /// Draws a batched group of quads representing the _text in 2D space, pass RDE_COLOR_WHITE to _tint_color for no tint effects, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_2d_end_drawing();

RDE_FUNC rde_mesh* rde_struct_memory_mesh_create(rde_mesh_gen_data* _data); // creates a new mesh that when not needed anymore, needs to be destroyed. A quad mesh will have 4 vertices and 6 indices and uploads to GPU
RDE_FUNC rde_mesh* rde_rendering_mesh_create_cube(float _size, rde_material* _material); // creates a new mesh that when not needed anymore, needs to be destroyed. A quad mesh will have 4 vertices and 6 indices and uploads to GPU
RDE_FUNC rde_mesh* rde_rendering_mesh_create_prism(rde_vec_3F _size, rde_material* _material); // creates a new mesh that when not needed anymore, needs to be destroyed. A quad mesh will have 4 vertices and 6 indices and uploads to GPU
RDE_FUNC rde_mesh* rde_rendering_mesh_create_sphere(float _radius, rde_material* _material); // creates a new mesh that when not needed anymore, needs to be destroyed. A quad mesh will have 4 vertices and 6 indices and uploads to GPU
RDE_FUNC rde_mesh* rde_rendering_mesh_create_triangular_pyramid(rde_vec_2F _size, rde_material* _material); // creates a new mesh that when not needed anymore, needs to be destroyed. A quad mesh will have 4 vertices and 6 indices and uploads to GPU
RDE_FUNC rde_mesh* rde_rendering_mesh_create_pyramid(rde_vec_3F _size, rde_material* _material); // creates a new mesh that when not needed anymore, needs to be destroyed. A quad mesh will have 4 vertices and 6 indices and uploads to GPU
RDE_FUNC rde_mesh_data rde_rendering_mesh_get_data(rde_mesh* _mesh);
RDE_FUNC void rde_rendering_mesh_destroy(rde_mesh* _mesh, bool _delete_allocated_buffers);

#if defined(RDE_OBJ_MODULE) || defined(RDE_FBX_MODULE)
RDE_FUNC rde_model* rde_rendering_model_load(const char* _model_path);
RDE_FUNC uint rde_rendering_model_get_vertices_count(rde_model* _model);
RDE_FUNC void rde_rendering_model_set_light_data(rde_model* _model, rde_material_light_data _light_data);
RDE_FUNC rde_material_light_data rde_rendering_model_get_light_data(rde_model* _model);
RDE_FUNC rde_model_data rde_rendering_model_get_data(rde_model* _model); // User manually needs to free 'meshes' field as it allocates dynamic memory
RDE_FUNC void rde_rendering_model_unload(rde_model* _model);
#endif

RDE_FUNC void rde_rendering_3d_begin_drawing(rde_camera* _camera, rde_window* _window, bool _draw_wireframe_over_mesh);
RDE_FUNC void rde_rendering_3d_draw_point(rde_vec_3F _position, rde_color _color, rde_shader* _shader); /// Draws a point in 3D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_3d_draw_line(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, unsigned short _thickness, rde_shader* _shader); /// Draws a batched line in 2D space, pass NULL on the _shader for the default shader
RDE_FUNC void rde_rendering_3d_draw_mesh(const rde_transform* _transform, rde_mesh* _mesh, rde_shader* _shader);
RDE_FUNC void rde_rendering_3d_draw_model(const rde_transform* _transform, rde_model* _model, rde_shader* _shader);
RDE_FUNC void rde_rendering_3d_draw_skybox(rde_camera* _camera);
RDE_FUNC void rde_rendering_3d_end_drawing();

RDE_FUNC void rde_rendering_lighting_set_directional_light_direction(rde_vec_3F _direction);
RDE_FUNC void rde_rendering_lighting_set_directional_light_position(rde_vec_3F _position);
RDE_FUNC void rde_rendering_lighting_set_directional_light_ambient_color(rde_color _ambient_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_ambient_color_f(rde_vec_3F _ambient_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_diffuse_color(rde_color _diffuse_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_diffuse_color_f(rde_vec_3F _diffuse_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_specular_color(rde_color _specular_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_specular_color_f(rde_vec_3F _specular_color);
RDE_FUNC void rde_rendering_light_add_add_point_light(rde_point_light* _point_light);
RDE_FUNC void rde_rendering_light_add_add_spot_light(rde_spot_light* _spot_light);
RDE_FUNC rde_directional_light rde_rendering_lighting_get_directional_light();

RDE_FUNC rde_skybox_id rde_rendering_skybox_load(const char* _texture_paths[6]); // order is right, left, bottom, top, front, back
RDE_FUNC void rde_rendering_skybox_use(rde_skybox_id _skybox_id);
RDE_FUNC void rde_rendering_skybox_unload(rde_skybox_id _skybox_id);

RDE_FUNC void rde_rendering_set_antialiasing(rde_window* _window, RDE_ANTIALIASING_ _antialiasing); // This function CANNOT be called during on_render.
RDE_FUNC RDE_ANTIALIASING_ rde_rendering_get_current_antialiasing();

RDE_FUNC void rde_rendering_shadows_begin(rde_window* _window, rde_camera* _camera);
RDE_FUNC void rde_rendering_shadows_end();

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

/// ============================ FILE SYSTEM ================================

RDE_FUNC rde_file_handle* rde_file_open(const char* _file_path, RDE_FILE_MODE_ _file_mode);
RDE_FUNC char* rde_file_read_full_file(rde_file_handle* _file_handler, uint* _output_file_size);
RDE_FUNC unsigned char* rde_file_read_full_file_bytes(rde_file_handle* _file_handler, uint* _output_file_size);
RDE_FUNC char* rde_file_read_line(rde_file_handle* _file_handler, uint _line);
RDE_FUNC char* rde_file_read_chunk(rde_file_handle* _file_handler, uint _begin_byte, uint _end_byte);
RDE_FUNC void rde_file_write(rde_file_handle* _file_handler, uint _bytes, const char* _data);
RDE_FUNC void rde_file_write_to_line(rde_file_handle* _file_handler, uint _bytes, const char* _data, uint _line);
RDE_FUNC void rde_file_append(rde_file_handle* _file_handler, uint _append_byte, uint _bytes, const char* _data, uint _line); // Appends some data at a specific point in the file. -1 as begin means EOF.
RDE_FUNC void rde_file_clear_content(rde_file_handle* _file_handler);
RDE_FUNC bool rde_file_exists(const char* _file_path);
RDE_FUNC void rde_file_delete(const char* _file_path);
RDE_FUNC void rde_file_move(const char* _file_path, const char* _new_file_path);
RDE_FUNC void rde_file_close(rde_file_handle* _file_handler);
RDE_FUNC void rde_file_free_read_text(rde_file_handle* _file_handle);
RDE_FUNC void rde_file_free_read_bytes(rde_file_handle* _file_handle);



/// ============================ PHYSICS ==============================

#ifdef RDE_PHYSICS_MODULE
RDE_FUNC void rde_jolt_draw_debug_shapes(rde_window* _window, rde_camera* _camera);
#endif



/// ============================ ERROR ================================

RDE_FUNC void rde_critical_error(bool _condition, const char* _fmt, ...);





#if RDE_IS_ANDROID()
RDE_FUNC ANativeWindow* rde_android_get_native_window();
#endif

#ifdef RDE_GOD
#include "private_structs.c"
#endif

#ifdef __cplusplus
}
#endif

#endif
