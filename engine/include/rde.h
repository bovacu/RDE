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

/// *************************************************************************************************
/// *                                		DEFINES                         						*
/// *************************************************************************************************

/// ========================= NUMERIC CONSTANTS ============================

#define RDE_EPSILON 0.0001f
#define RDE_PI 3.141592741f

#define RDE_MAX_NUMBER_OF_WINDOWS 10
#define RDE_MAX_VERTICES_PER_BATCH 50000
#define RDE_MAX_LOADABLE_SHADERS 256

#ifdef RDE_RENDERING_MODULE
#define RDE_MAX_LOADABLE_TEXTURES 512
#define RDE_MAX_LOADABLE_ATLASES 512
#define RDE_MAX_LOADABLE_FONTS 512
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

#define RDE_SHADER_LINE "line_shader"
#define RDE_SHADER_COLOR "color_shader"
#define RDE_SHADER_TEXTURE "texture_shader"
#define RDE_SHADER_TEXT "text_shader"
#define RDE_SHADER_FRAMEBUFFER "framebuffer_shader"
#define RDE_SHADER_MESH "mesh_shader"
#define RDE_SHADER_SKYBOX "skybox_shader"

#define RDE_MAX_POINT_LIGHTS 10
#define RDE_MAX_SPOT_LIGHTS 10

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

#ifdef RDE_NO_NAMESPACES
#define RDE_N(_namespace, _function) _function
#else
#define RDE_N(_namespace, _function) _namespace##_##_function
// Example of usage RDE_FUNC void RDE_N(rde_my_namespace, my_function)(int _a, int _b);
#endif

#if IS_WINDOWS()
	#include <io.h>
	#include <fcntl.h>
	#include <windows.h>
	#include <shlwapi.h>
#endif			 

#define RDE_SHOW_WINDOWS_CONSOLE 							\
	if(AllocConsole()) {									\
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);	\
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);		\
	}

/// ====================== PLATFORM SPECIFIC TYPES ==========================

typedef unsigned long ulong;
typedef unsigned int uint;

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
#elif IS_ANDROID()
#include "SDL2/SDL_main.h"
#include <jni.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/log.h>
#define RDE_MAIN(_window, _heap_allocs_config, _mandatory_callbacks, _init_func, _end_func)	\
	int main(int _argc, char* _argv[]) {													\
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

#define COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(_func_name, _extra_code) 	\
	void _func_name(rde_event* _event, rde_window* _window) {				\
		UNUSED(_window);													\
		UNUSED(_event);														\
		_extra_code															\
	}

#if !IS_ANDROID()
	#define rde_log_level(_level, _fmt, ...) do { 				\
		if(!rde_engine_logs_supressed()) {						\
			rde_log_level_inner(_level, _fmt __VA_OPT__(,) __VA_ARGS__);	\
			printf("\n");										\
		}														\
	} while(0);

	#define rde_log_color(_color, _fmt, ...) do { 				\
		if(!rde_engine_logs_supressed()) {						\
			rde_log_color_inner(_color, _fmt  __VA_OPT__(,) __VA_ARGS__);	\
			printf("\n");										\
		}														\
	} while(0);
	
	#define rde_log_level_sl(_level, _fmt, ...) do { 			\
		if(!rde_engine_logs_supressed()) {						\
			rde_log_level_inner(_level, _fmt  __VA_OPT__(,) __VA_ARGS__);	\
		}														\
	} while(0);
	
	#define rde_log_color_sl(_color, _fmt, ...) do { 			\
		if(!rde_engine_logs_supressed()) {						\
			rde_log_color_inner(_color, _fmt  __VA_OPT__(,) __VA_ARGS__);	\
		}														\
	} while(0);
#else
	#define rde_log_level(_level, _fmt, ...) do { 					\
		if(!rde_engine_logs_supressed()) {						\
			__android_log_print(ANDROID_LOG_DEBUG, "SDL_RDE", _fmt __VA_OPT__(,) __VA_ARGS__);								\
			printf("\n");										\
		}														\
	} while(0);
	
	#define rde_log_color(_color, _fmt, ...) do { 					\
		if(!rde_engine_logs_supressed()) {						\
			__android_log_print(ANDROID_LOG_DEBUG, "SDL_RDE", _fmt __VA_OPT__(,) __VA_ARGS__);								\
			printf("\n");										\
		}														\
	} while(0);
	
	#define rde_log_level_sl(_level, _fmt, ...) do { 					\
		if(!rde_engine_logs_supressed()) {						\
			__android_log_print(ANDROID_LOG_DEBUG, "SDL_RDE", _fmt __VA_OPT__(,) __VA_ARGS__);								\
		}														\
	} while(0);
	
	#define rde_log_color_sl(_color, _fmt, ...) do { 					\
		if(!rde_engine_logs_supressed()) {						\
			__android_log_print(ANDROID_LOG_DEBUG, "SDL_RDE", _fmt __VA_OPT__(,) __VA_ARGS__);								\
		}														\
	} while(0);
#endif

#define RDE_SAFE_ARR_ACCESS(_type) RDE_FUNC _type rde_arr_s_get_##_type(uint _index, _type* _arr, size_t _arr_size, char* _fmt, ...);
#define RDE_SAFE_ARR_SET(_type) RDE_FUNC void rde_arr_s_set_##_type(uint _index, _type _value, _type* _arr, size_t _arr_size, char* _fmt, ...);


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

	RDE_EVENT_TYPE_DRAG_AND_DROP_BEGIN,
	RDE_EVENT_TYPE_DRAG_AND_DROP_FILE,
	RDE_EVENT_TYPE_DRAG_AND_DROP_END,

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

/// ================================= UTIL ==================================

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

/// ================================= MATH ==================================

SPECIALIZED_VEC2(int, 			rde_vec_2I);
SPECIALIZED_VEC2(uint, 	rde_vec_2UI);
SPECIALIZED_VEC2(float, 		rde_vec_2F);
SPECIALIZED_VEC2(double,		rde_vec_2D);
SPECIALIZED_VEC2(long,			rde_vec_2L);
SPECIALIZED_VEC2(unsigned long, rde_vec_2UL);
SPECIALIZED_VEC2(size_t, 		rde_vec_2ST);

SPECIALIZED_VEC3(int, 			rde_vec_3I);
SPECIALIZED_VEC3(uint, 	rde_vec_3UI);
SPECIALIZED_VEC3(float, 		rde_vec_3F);
SPECIALIZED_VEC3(double,		rde_vec_3D);
SPECIALIZED_VEC3(long,			rde_vec_3L);
SPECIALIZED_VEC3(unsigned long, rde_vec_3UL);
SPECIALIZED_VEC3(size_t, 		rde_vec_3ST);

SPECIALIZED_VEC4(int, 			rde_vec_4I);
SPECIALIZED_VEC4(uint, 	rde_vec_4UI);
SPECIALIZED_VEC4(float, 		rde_vec_4F);
SPECIALIZED_VEC4(double,		rde_vec_4D);
SPECIALIZED_VEC4(long,			rde_vec_4L);
SPECIALIZED_VEC4(unsigned long, rde_vec_4UL);
SPECIALIZED_VEC4(size_t, 		rde_vec_4ST);

SPECIALIZED_MAT2(int, 			rde_mat_2I);
SPECIALIZED_MAT2(uint, 	rde_mat_2UI);
SPECIALIZED_MAT2(float, 		rde_mat_2F);
SPECIALIZED_MAT2(double,		rde_mat_2D);
SPECIALIZED_MAT2(long,			rde_mat_2L);
SPECIALIZED_MAT2(unsigned long, rde_mat_2UL);
SPECIALIZED_MAT2(size_t, 		rde_mat_2ST);

SPECIALIZED_MAT3(int, 			rde_mat_3I);
SPECIALIZED_MAT3(uint, 	rde_mat_3UI);
SPECIALIZED_MAT3(float, 		rde_mat_3F);
SPECIALIZED_MAT3(double,		rde_mat_3D);
SPECIALIZED_MAT3(long,			rde_mat_3L);
SPECIALIZED_MAT3(unsigned long, rde_mat_3UL);
SPECIALIZED_MAT3(size_t, 		rde_mat_3ST);

SPECIALIZED_MAT4(int, 			rde_mat_4I);
SPECIALIZED_MAT4(uint, 	rde_mat_4UI);
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
rde_probability rde_struct_create_probability();

/// ================== CALLBACKS AND FUNCTION POINTERS ======================

typedef struct rde_engine rde_engine;
typedef struct rde_window rde_window;
typedef struct rde_event rde_event;

typedef void (*rde_event_func)(rde_event*, rde_window*);
typedef void (*rde_engine_user_side_loop_func)(float);
typedef void (*rde_engine_user_side_loop_func_2)(float, rde_window*);

typedef struct rde_inner_window_data rde_inner_window_info;

typedef struct {
	rde_engine_user_side_loop_func on_update;
	rde_engine_user_side_loop_func on_fixed_update;
	rde_engine_user_side_loop_func on_late_update;
	rde_engine_user_side_loop_func_2 on_render;
} rde_end_user_mandatory_callbacks;
rde_end_user_mandatory_callbacks rde_struct_create_end_user_mandatory_callbacks();

/// ============================== ENGINE ===================================

typedef struct rde_file_handle rde_file_handle;

typedef struct {
	int index;
} rde_display_info;
rde_display_info rde_struct_create_display_info();

typedef struct {
	size_t max_number_of_windows;
	size_t max_number_of_vertices_per_batch;
	size_t max_number_of_shaders;
	
#ifdef RDE_RENDERING_MODULE
	size_t max_number_of_textures;
	size_t max_number_of_atlases;
	size_t max_number_of_fonts;
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
rde_event_window rde_struct_create_event_window();

typedef struct {
	int orientation;
	int display_index;
} rde_event_display;
rde_event_display rde_struct_create_event_display();

typedef struct {
	RDE_KEYBOARD_KEY_ key;
	char typed_char;
	const char* typed_text;
	int amount_of_times_pressed;
} rde_event_key;
rde_event_key rde_struct_create_event_key();

typedef struct {
	rde_vec_2I position;
	rde_vec_2F scrolled;
	RDE_MOUSE_BUTTON_ button;
	int amount_of_times_pressed;
} rde_event_mouse;
rde_event_mouse rde_struct_create_event_mouse();

typedef struct {
	size_t controller_id;
	rde_vec_2F left_joystick;
	rde_vec_2F right_joystick;
	rde_vec_2F back_buttons;
	RDE_CONTROLLER_BUTTON_ button;
	RDE_CONTROLLER_AXIS_ axis;
} rde_event_controller;
rde_event_controller rde_struct_create_event_controller();

typedef struct {
	rde_vec_2I init_touch_position;
	rde_vec_2I end_touch_position;
	float pressure;
	int finger_id;
} rde_event_mobile;
rde_event_mobile rde_struct_create_event_mobile();

typedef struct {
	size_t window_id;
	char* file_path;
} rde_event_drag_and_drop;
rde_event_drag_and_drop rde_struct_create_event_drag_and_drop();

typedef struct {
	rde_event_window window_event_data;
	rde_event_key key_event_data;
	rde_event_mouse mouse_event_data;
	rde_event_controller controller_event_data;
	rde_event_mobile mobile_event_data;
	rde_event_display display_event_data;
	rde_event_drag_and_drop drag_and_drop_data;
} rde_event_data;
rde_event_data rde_struct_create_event_data();

struct rde_event {
	RDE_EVENT_TYPE_ type;
	ulong time_stamp;
	uint window_id;
	bool handled;
	rde_event_data data;
	void* native_event;
};
rde_event rde_struct_create_event();

typedef struct rde_transform rde_transform;
struct rde_transform {
	rde_vec_3F position;
	rde_vec_3F rotation;
	rde_vec_3F scale;
	rde_transform* parent;
};
rde_transform rde_struct_create_transform();

/// ============================ RENDERING ==================================

#ifdef RDE_RENDERING_MODULE
typedef struct rde_shader rde_shader;
typedef struct {
	uint vertex_program_id;
	uint fragment_program_id;
	int compiled_program_id;
} rde_shader_data;

typedef struct rde_render_texture rde_render_texture;

typedef struct rde_texture rde_texture;
typedef struct {
	int opengl_texture_id;
	rde_vec_2UI size;
	rde_vec_2UI position;
	int channels;
	const char* file_path;
	int atlas_texture_id;
} rde_texture_data;

typedef struct rde_atlas rde_atlas;
typedef struct {
	int opengl_texture_id;
	int amount_of_subtextures;
} rde_atlas_data;

typedef struct rde_font rde_font;
typedef struct {
	int opengl_texture_id;
	int amount_of_chars;
} rde_font_data;

typedef struct {
	float shininess;
	rde_vec_3F ka;
	rde_vec_3F kd;
	rde_vec_3F ks;
} rde_material_light_data;
rde_material_light_data rde_struct_create_material_light_data();

typedef struct {
	rde_texture* map_ka;
	rde_texture* map_kd;
	rde_texture* map_ks;
	rde_texture* map_bump;
	rde_render_texture* render_texture;
	rde_material_light_data material_light_data;
} rde_material;
rde_material rde_struct_create_material();

typedef struct {
	char* name;
	size_t vertex_count;

	float* positions;
	size_t positions_size;
	
	float* texcoords;
	size_t texcoords_size;

	float* normals;
	size_t normals_size;

	rde_material material;
} rde_mesh_gen_data;

typedef struct rde_mesh rde_mesh;
typedef struct {
	int amount_of_vertices;
	int vertex_buffer_objects_ids[4];	// 0 -> positions (static), 
										// 1 -> colors (static), 
										// 2 -> normals (static), 
										// 3 -> texture coords (static)
										// 4 -> transforms to render (dynamic)
} rde_mesh_data;

typedef struct rde_model rde_model;
typedef struct {
	size_t amount_of_meshes;
	rde_mesh** meshes;
} rde_model_data;

typedef struct {
	rde_vec_3F direction;
	rde_vec_3F ambient_color;
	rde_vec_3F diffuse_color;
	rde_vec_3F specular_color;
} rde_directional_light;
rde_directional_light rde_struct_create_directional_light();

typedef struct {
	rde_vec_3F position;
	rde_vec_3F ambient_color;
	rde_vec_3F diffuse_color;
	rde_vec_3F specular_color;
	float constant;
	float linear;
	float quadratic;
} rde_point_light;
rde_point_light rde_struct_create_point_light();

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
rde_spot_light rde_struct_create_spot_light();

typedef uint rde_skybox_id;

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
rde_color rde_struct_create_color();

typedef struct {
	rde_vec_2I* vertices;
	size_t vertices_count;
} rde_polygon;
rde_polygon rde_struct_create_polygon();

struct rde_viewport {
	UNIMPLEMENTED_STRUCT()
};

typedef struct {
	size_t id;
	float zoom;
	float fov;
	rde_transform transform;
	rde_vec_3F direction;
	rde_vec_3F up;
	rde_vec_2F near_far;
	RDE_CAMERA_TYPE_ camera_type;
	bool enabled;
} rde_camera;
rde_camera rde_struct_create_camera(RDE_CAMERA_TYPE_ _camera_type);

/// ============================ AUDIO ==================================

#ifdef RDE_AUDIO_MODULE
typedef struct rde_sound rde_sound;
typedef struct {
	void* user_data;
	unsigned short channels;
	uint rate;
} rde_sound_config;
rde_sound_config rde_struct_create_audio_config();
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
const rde_color RDE_COLOR_NO_TEXTURE		= { 193,  84, 193, 255 };

const rde_engine_heap_allocs_config RDE_DEFAULT_HEAP_ALLOCS_CONFIG = {
	RDE_MAX_NUMBER_OF_WINDOWS,
	RDE_MAX_VERTICES_PER_BATCH,
	RDE_MAX_LOADABLE_SHADERS,

#ifdef RDE_RENDERING_MODULE
	RDE_MAX_LOADABLE_TEXTURES,
	RDE_MAX_LOADABLE_ATLASES,
	RDE_MAX_LOADABLE_FONTS,
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

RDE_FUNC void rde_log_color_inner(RDE_LOG_COLOR_ _color, const char* _fmt, ...);
RDE_FUNC void rde_log_level_inner(RDE_LOG_LEVEL_ _level, const char* _fmt, ...);

/// ============================ UTIL =======================================

RDE_FUNC const char* rde_util_file_get_name_extension(const char* _file_name);
RDE_FUNC void rde_util_file_sanitaize_path(const char* _path, char* _sanitized, size_t _sanitized_size);
RDE_FUNC rde_file_handle* rde_file_open(const char* _file_path, RDE_FILE_MODE_ _file_mode);
RDE_FUNC char* rde_file_read_full_file(rde_file_handle* _file_handler);
RDE_FUNC char* rde_file_read_line(rde_file_handle* _file_handler, size_t _line);
RDE_FUNC char* rde_file_read_chunk(rde_file_handle* _file_handler, size_t _begin_byte, size_t _end_byte);
RDE_FUNC void rde_file_write(rde_file_handle* _file_handler, size_t _bytes, const char* _data);
RDE_FUNC void rde_file_write_to_line(rde_file_handle* _file_handler, size_t _bytes, const char* _data, size_t _line);
RDE_FUNC void rde_file_append(rde_file_handle* _file_handler, size_t _append_byte, size_t _bytes, const char* _data, size_t _line); // Appends some data at a specific point in the file. -1 as begin means EOF.
RDE_FUNC void rde_file_clear_content(rde_file_handle* _file_handler);
RDE_FUNC bool rde_file_exists(const char* _file_path);
RDE_FUNC void rde_file_delete(const char* _file_path);
RDE_FUNC void rde_file_move(const char* _file_path, const char* _new_file_path);
RDE_FUNC void rde_file_close(rde_file_handle* _file_handler);

#ifdef RDE_RENDERING_MODULE
RDE_FUNC size_t rde_util_font_get_string_width(const char* _string, const rde_font* _font);
RDE_FUNC rde_vec_2I rde_util_font_get_string_size(const char* _string, const rde_font* _font);
#endif

RDE_FUNC char* rde_util_string_trim(char* _s);
RDE_FUNC bool rde_util_string_starts_with(const char* _string, const char* _prefix);
RDE_FUNC bool rde_util_string_ends_with(const char* _string, const char* _suffix);
RDE_FUNC bool rde_util_string_contains_substring(const char* _string, const char* _substring, bool _case_sensitive);
RDE_FUNC size_t rde_util_string_char_appearences(const char* _string, char _char);
RDE_FUNC void rde_util_string_concat(char* _string, size_t _size, const char* _fmt, ...);
RDE_FUNC void rde_util_string_to_lower(char* _destination, const char* _string);
RDE_FUNC void rde_util_string_to_lower_itself(char* _string);
RDE_FUNC void rde_util_string_to_upper(char* _destination, const char* _string);
RDE_FUNC void rde_util_string_to_upper_itself(char* _string);
RDE_FUNC void rde_util_string_replace_char(char* _string, char _old, char _new);
RDE_FUNC void rde_util_string_replace_chars_all(char* _string, char _old, char _new);
RDE_FUNC size_t rde_util_string_split(char* _string, char** _split_array, const char* _split_mark);

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


RDE_FUNC rde_window* rde_engine_create_engine(int _argc, char** _argv, rde_engine_heap_allocs_config _heap_allocs_config);
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

RDE_FUNC rde_display_info* rde_engine_get_available_displays();
RDE_FUNC void rde_engine_switch_window_display(rde_window* _window, size_t _new_display);

RDE_FUNC rde_window* rde_engine_get_focused_window();

RDE_FUNC void rde_engine_use_rde_2d_physics_system(bool _use);

RDE_FUNC bool rde_engine_is_vsync_active();
RDE_FUNC void rde_engine_set_vsync_active(bool _vsync);

RDE_FUNC void rde_engine_show_message_box(RDE_LOG_LEVEL_ _level, const char* _title, const char* _content, rde_window* _window);

RDE_FUNC void rde_engine_destroy_engine();


/// ============================ WINDOW =====================================

RDE_FUNC rde_window* rde_window_create_window();

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

RDE_FUNC bool rde_events_is_key_just_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key);
RDE_FUNC bool rde_events_is_key_pressed(rde_window* _window, RDE_KEYBOARD_KEY_ _key);
RDE_FUNC bool rde_events_is_key_just_released(rde_window* _window, RDE_KEYBOARD_KEY_ _key);

RDE_FUNC bool rde_events_is_mouse_button_just_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button);
RDE_FUNC bool rde_events_is_mouse_button_pressed(rde_window* _window, RDE_MOUSE_BUTTON_ _button);
RDE_FUNC bool rde_events_is_mouse_button_just_released(rde_window* _window, RDE_MOUSE_BUTTON_ _button);
RDE_FUNC rde_vec_2F rde_events_mouse_get_scrolled(rde_window* _window);
RDE_FUNC rde_vec_2I rde_events_mouse_get_position(rde_window* _window);

//#if IS_MOBILE()
//RDE_FUNC int rde_events_mobile_consume_events(rde_event* _event, rde_window* _window, void* _user_data);
//#endif

/// ============================ RENDERING ==================================

#ifdef RDE_RENDERING_MODULE
RDE_FUNC rde_shader* rde_rendering_shader_load(const char* _name, const char* _vertex_code, const char* _fragment_code);
RDE_FUNC void rde_rendering_shader_set_uniform_value_float(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_FV_ _type, float* _data, bool _transpose);
RDE_FUNC void rde_rendering_shader_set_uniform_value_int(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_IV_ _type, int* _data);
RDE_FUNC void rde_rendering_shader_set_uniform_value_uint(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_UIV_ _type, uint* _data);
RDE_FUNC rde_shader_data rde_rendering_shader_get_data(rde_shader* _shader);
RDE_FUNC rde_shader* rde_rendering_shader_get_by_name(const char* _name);
RDE_FUNC void rde_rendering_shader_unload(rde_shader* _shader);

RDE_FUNC rde_texture* rde_rendering_texture_load(const char* _file_path);
RDE_FUNC rde_texture* rde_rendering_texture_text_load(const char* _file_path);
RDE_FUNC rde_texture_data rde_rendering_texture_get_data(rde_texture* _texture);
RDE_FUNC void rde_rendering_texture_unload(rde_texture* _texture);

RDE_FUNC rde_atlas* rde_rendering_atlas_load(const char* _texture_path, const char* _config_path);
RDE_FUNC rde_texture* rde_rendering_atlas_get_subtexture(rde_atlas* _atlas, const char* _texture_name);
RDE_FUNC rde_atlas_data rde_rendering_atlas_get_data(rde_atlas* _atlas);
RDE_FUNC void rde_rendering_atlas_unload(rde_atlas* _atlas);

RDE_FUNC rde_texture* rde_rendering_memory_texture_create(size_t _width, size_t _height, int _channels);
RDE_FUNC void rde_rendering_memory_texture_set_pixel(rde_texture* _memory_texture, rde_vec_2I _position, rde_color _color);
RDE_FUNC rde_color rde_rendering_memory_texture_get_pixel(rde_texture* _memory_texture, rde_vec_2I _position);
RDE_FUNC void rde_rendering_memory_texture_gpu_upload(rde_texture* _memory_texture);
RDE_FUNC unsigned char* rde_rendering_memory_texture_get_pixels(rde_texture* _memory_texture);
RDE_FUNC void rde_rendering_memory_texture_destroy(rde_texture* _memory_texture);

RDE_FUNC rde_render_texture* rde_rendering_render_texture_create(size_t _width, size_t _height);
RDE_FUNC void rde_rendering_render_texture_enable(rde_render_texture* _render_texture);
RDE_FUNC void rde_rendering_render_texture_disable();
RDE_FUNC void rde_rendering_render_texture_update(rde_render_texture* _render_texture, size_t _width, size_t _height);
RDE_FUNC void rde_rendering_render_texture_destroy(rde_render_texture* _render_texture);

RDE_FUNC rde_font* rde_rendering_font_load(const char* _font_path, const char* _font_config_path);
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
RDE_FUNC rde_mesh* rde_rendering_mesh_create_sphere(float _radius, rde_material* _material); // creates a new mesh that when not needed anymore, needs to be destroyed. A quad mesh will have 4 vertices and 6 indices and uploads to GPU
RDE_FUNC rde_mesh_data rde_rendering_mesh_get_data(rde_mesh* _mesh);
RDE_FUNC void rde_rendering_mesh_destroy(rde_mesh* _mesh, bool _delete_allocated_buffers);

#if defined(RDE_OBJ_MODULE) || defined(RDE_FBX_MODULE)
RDE_FUNC rde_model* rde_rendering_model_load(const char* _model_path);
RDE_FUNC size_t rde_rendering_model_get_vertices_count(rde_model* _model);
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
RDE_FUNC void rde_rendering_lighting_set_directional_light_ambient_color(rde_color _ambient_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_ambient_color_f(rde_vec_3F _ambient_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_diffuse_color(rde_color _diffuse_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_diffuse_color_f(rde_vec_3F _diffuse_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_specular_color(rde_color _specular_color);
RDE_FUNC void rde_rendering_lighting_set_directional_light_specular_color_f(rde_vec_3F _specular_color);
RDE_FUNC void rde_rendering_light_add_add_point_light(rde_point_light* _point_light);
RDE_FUNC void rde_rendering_light_add_add_spot_light(rde_spot_light* _spot_light);
RDE_FUNC rde_directional_light rde_rendering_lighting_get_directional_light();

RDE_FUNC rde_skybox_id rde_rendering_skybox_load(const char* _texture_paths[6]); // order is right, left, top, bottom, front, back
RDE_FUNC void rde_rendering_skybox_use(rde_skybox_id _skybox_id);
RDE_FUNC void rde_rendering_skybox_unload(rde_skybox_id _skybox_id);

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

#ifdef RDE_FILE_SYSTEM_MODULE
#endif

#if IS_ANDROID()
RDE_FUNC ANativeWindow* rde_android_get_native_window();
#endif

#ifdef RDE_GOD
#include "private_structs.c"
#endif

#ifdef __cplusplus
}
#endif

#endif