#ifndef RDE_H
#define RDE_H

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
#include "SDL2/SDL.h"

/// *************************************************************************************************
/// *                                		DEFINES                         						*
/// *************************************************************************************************

/// ========================= NUMERIC CONSTANTS ============================

#define RDE_EPSILON 0.0001f
#define RDE_PI 3.141592741f

#define RDE_MAX_NUMBER_OF_WINDOWS 10

#define RDE_MAX_VERTICES_PER_BATCH 50000

/// ============================== SHADERS =================================

#define FRAMEBUFFER_VERTEX_SHADER_ES "defaultAssets/shaders/es/framebuffer/FrameBufferVertex.glsl"
#define FRAMEBUFFER_FRAGMENT_SHADER_ES "defaultAssets/shaders/es/framebuffer/FrameBufferFragment.glsl"
#define FRAMEBUFFER_VERTEX_SHADER_CORE "defaultAssets/shaders/core/framebuffer/FrameBufferVertex.glsl"
#define FRAMEBUFFER_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/framebuffer/FrameBufferFragment.glsl"

#define TEXTURE_VERTEX_SHADER_ES "defaultAssets/shaders/es/texture/vertex.glsl"
#define TEXT_VERTEX_SHADER_ES "defaultAssets/shaders/es/texture/textVertex.glsl"
#define DEBUG_VERTEX_SHADER_ES "defaultAssets/shaders/es/debug/debugVertex.glsl"

#define TEXTURE_VERTEX_SHADER_CORE "defaultAssets/shaders/core/texture/vertex.glsl"
#define TEXT_VERTEX_SHADER_CORE "defaultAssets/shaders/core/texture/textVertex.glsl"
#define DEBUG_VERTEX_SHADER_CORE "defaultAssets/shaders/core/debug/debugVertex.glsl"
#define DEBUG_GRID_VERTEX_SHADER_CORE "defaultAssets/shaders/core/debug/gridVertex.glsl"
#define DEBUG_GRID_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/debug/gridFragment.glsl"

#define TEXTURE_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/texture/fragment.glsl"
#define MIRROR_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/texture/mirrorFragment.glsl"
#define MIRROR_VERTEX_SHADER_CORE "defaultAssets/shaders/core/texture/mirrorVertex.glsl"
#define TEXT_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/texture/textFragment.glsl"
#define OUTLINE_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/texture/outlineFragment.glsl"
#define NEON_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/texture/glowFragment.glsl"
#define BLOOM_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/texture/bloomFragment.glsl"
#define BLUR_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/texture/blurFragment.glsl"
#define DEBUG_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/debug/debugFragment.glsl"

#define TEXTURE_FRAGMENT_SHADER_ES "defaultAssets/shaders/es/texture/fragment.glsl"
#define TEXT_FRAGMENT_SHADER_ES "defaultAssets/shaders/es/texture/textFragment.glsl"
#define OUTLINE_FRAGMENT_SHADER_ES "defaultAssets/shaders/es/texture/outlineFragment.glsl"
#define NEON_FRAGMENT_SHADER_ES "defaultAssets/shaders/es/texture/glowFragment.glsl"
#define BLOOM_FRAGMENT_SHADER_ES "defaultAssets/shaders/es/texture/bloomFragment.glsl"
#define BLUR_FRAGMENT_SHADER_ES "defaultAssets/shaders/es/texture/blurFragment.glsl"
#define DEBUG_FRAGMENT_SHADER_ES "defaultAssets/shaders/es/debug/debugFragment.glsl"

#define SPRITE_RENDERER_SHADER "basic"
#define TEXT_RENDERER_SHADER "basicText"
#define DEBUG_SHADER "debug"
#define FRAMEBUFFER_SHADER "framebuffer"



/// ====================== COMPILATION AND EXPORT ==========================

#ifndef NDEBUG
#define ENGINE_DEBUG
#endif

#ifdef FORCE_DEBUG
	#define ENGINE_DEBUG
#endif

#ifdef ENGINE_DEBUG
	#if defined(_WIN32)
		#define ENGINE_DEBUG_BREAK() __debugbreak()
		#define ENGINE_ENABLE_ASSERTS
	#elif defined(__linux__)
		#include <csignal>
		#define ENGINE_DEBUG_BREAK() raise(SIGTRAP)
		#define ENGINE_ENABLE_ASSERTS
	#else
		#define ENGINE_DEBUGBREAK()
	#endif
#endif

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
#define IS_LINUX() (defined(__linux__))
#define IS_DESKTOP() (IS_LINUX() || IS_MAC() || IS_WINDOWS())
#define IS_IOS() (defined(__APPLE__) && defined(IOS_PLATFORM))
#define IS_APPLE() (IS_MAC() || IS_IOS())
#define IS_ANDROID() (defined(__ANDROID__))
#define IS_MOBILE() (IS_ANDROID() || IS_IOS())

#ifdef ENGINE_ENABLE_ASSERTS
	#define ENGINE_ASSERT(x, message) { if(!(x)) { std::cout << (message) << std::endl; ENGINE_DEBUG_BREAK(); } }
#else
	#define ENGINE_ASSERT(x, message)
#endif

#ifdef RDE_EXPORT
	#define RDE_API [[gnu::visibility("default")]]
	#define RDE_HIDDEN [[gnu::visibility("hidden")]]
	#define RDE_INTERNAL [[gnu::visibility("internal")]]
	#define RDE_DEPRECATED(_explanation) [[gnu::deprecated(_explanation)]]
#endif

#if IS_WINDOWS() && !defined(ONLY_EXECUTABLE)
	#define RDE_FUNC __declspec(dllexport)
	#define RDE_FUNC_STATIC __declspec(dllexport) static
	#define RDE_FUNC_EXTERNAL extern "C" RDE_FUNC
#else
	#define RDE_FUNC 
	#define RDE_FUNC_STATIC static
	#define RDE_FUNC_EXTERNAL
#endif



/// ============================== UTIL ====================================

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define LINE_STRING STRINGIZE(__LINE__)
#define FUNC_STRING STRINGIZE(__func__)
#define FILE_STRING STRINGIZE(__FILE__)



/// ======================= WARNING SILENCER ===============================

#define UNUSED(_x) (void)_x;
//#define UNIMPLEMENTED(_message) {						\
//	std::string _full_message = "[UNIMPLEMENTED - "; 	\
//	_full_message += "[UNIMPLEMENTED - File ";			\
//	_full_message += FILE_STRING;						\
//	_full_message += " - Func ";						\
//	_full_message += FUNC_STRING;						\
//	_full_message += " - Line "; 						\
//	_full_message += LINE_STRING;						\
//	_full_message += "] -> ";							\
//	_full_message += _message;							\
//	assert(false && _full_message.c_str());				\
//	}

#define UNIMPLEMENTED(_message) {				\
		assert(false);							\
	}

#define UNIMPLEMENTED_STRUCT() short foo;



/// ==================== GENERIC FUNCS AND STRUCTS ==========================

#define SPECIALIZED_VEC2(_type, _name) 	\
	struct _name {						\
		_type x;						\
		_type y;						\
	};

#define SPECIALIZED_VEC3(_type, _name) 	\
	struct _name {						\
		_type x;						\
		_type y;						\
		_type z;						\
	};

#define SPECIALIZED_VEC4(_type, _name) 	\
	struct _name {						\
		_type x;						\
		_type y;						\
		_type z;						\
		_type w;						\
	};

#define SPECIALIZED_MAT2(_type, _name) 	\
	struct _name {						\
		union {							\
			struct {					\
				float m00, m01;			\
				float m10, m11;			\
			};							\
										\
			float m[2][2];				\
			float v[4];					\
		};								\
	};

#define SPECIALIZED_MAT3(_type, _name) 	\
	struct _name {						\
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
	};

#define SPECIALIZED_MAT4(_type, _name) 		\
	struct _name {							\
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
	};

#define COMMON_CALLBACK_IMPLEMENTATION_FOR_EVENT(_func_name, _callback_type, _callback_name, _extra_code) 	\
	void _func_name(rde_engine* _engine, rde_window* _window, rde_event* _event) {					\
		UNUSED(_window);																					\
		if(!_engine->_callback_type._callback_name.block_engine_default_implementation) {					\
			_extra_code																						\
		}																									\
																											\
		if(_engine->_callback_type._callback_name._callback != nullptr) {									\
			_engine->_callback_type._callback_name._callback(_event);										\
		}																									\
	}



/// ====================== PLATFORM SPECIFIC TYPES ==========================

#if IS_MOBILE() || IS_MAC() || defined(_WIN32)
	typedef unsigned long ulong;
	typedef unsigned int uint;
#endif

#ifdef __EMSCRIPTEN__
	typedef unsigned long ulong;
	typedef unsigned int uint;
#endif






/// *************************************************************************************************
/// *                                		  ENUMS                         						*
/// *************************************************************************************************

/// ================================= STATE =================================

enum RDE_INPUT_STATUS_ {
	RDE_INPUT_STATUS_ERROR 			=-1,
	RDE_INPUT_STATUS_UNINITIALIZED 	= 0,
	RDE_INPUT_STATUS_JUST_PRESSED 	= 1,
	RDE_INPUT_STATUS_KEEP_PRESSED 	= 2,
	RDE_INPUT_STATUS_JUST_RELEASED 	= 3,
	RDE_INPUT_STATUS_KEEP_RELEASED 	= 4,
};

enum RDE_MOUSE_STATUS_ {
	RDE_MOUSE_STATUS_ENTERED,
	RDE_MOUSE_STATUS_EXITED
};



/// ================================ EVENTS ================================

enum RDE_EVENT_TYPE_ {
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
	RDE_EVENT_TYPE_KEY_PRESSED, RDE_EVENT_TYPE_KEY_DOWN, RDE_EVENT_TYPE_KEY_RELEASED, RDE_EVENT_TYPE_KEY_TYPED, RDE_EVENT_TYPE_TEXT_TYPED, 
	RDE_EVENT_TYPE_KEY_END,	

	RDE_EVENT_TYPE_MOUSE_BEGIN,
	RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED, RDE_EVENT_TYPE_MOUSE_BUTTON_DOWN, RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED, RDE_EVENT_TYPE_MOUSE_SCROLLED, RDE_EVENT_TYPE_MOUSE_MOVED,
	RDE_EVENT_TYPE_MOUSE_END,

	RDE_EVENT_TYPE_CONTROLLER_AXIS_MOVED, RDE_EVENT_TYPE_CONTROLLER_BUTTON_DOWN, RDE_EVENT_TYPE_CONTROLLER_BUTTON_UP,

	RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN, RDE_EVENT_TYPE_MOBILE_TOUCH_UP, RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED
};



/// =============================== KEYBOARD ================================

enum RDE_KEYBOARD_KEY_ {
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
};



/// ================================ MOUSE ==================================

enum RDE_MOUSE_BUTTON_ {
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
};



/// ============================== CONTROLLER ===============================

enum RDE_CONTROLLER_BUTTON_ {
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
};

enum RDE_CONTROLLER_AXIS_ {
	RDE_CONTROLLER_AXIS_LEFT,
	RDE_CONTROLLER_AXIS_RIGHT,
	RDE_CONTROLLER_AXIS_LT,
	RDE_CONTROLLER_AXIS_RT,
	RDE_CONTROLLER_AXIS_NONE
};



/// =============================== PLATFORM ================================

enum RDE_PLATFORM_TYPE_ {
	RDE_PLATFORM_TYPE_LINUX,
	RDE_PLATFORM_TYPE_WINDOWS,
	RDE_PLATFORM_TYPE_MAC,
	RDE_PLATFORM_TYPE_ANDROID,
	RDE_PLATFORM_TYPE_IOS,
	RDE_PLATFORM_TYPE_EMSCRIPTEN,
	RDE_PLATFORM_TYPE_UNSUPPORTED
};

enum RDE_INPUT_TYPE_ {
	RDE_INPUT_TYPE_WINDOW,
	RDE_INPUT_TYPE_MOUSE,
	RDE_INPUT_TYPE_KEYBOARD,
	RDE_INPUT_TYPE_CONTROLLER,
	RDE_INPUT_TYPE_MOBILE
};

enum RDE_BATTERY_ {
	RDE_BATTERY_UKNOWN,
	RDE_BATTERY_ON_BATTERY,
	RDE_BATTERY_NO_BATTERY,
	RDE_BATTERY_CHARGING,
	RDE_BATTERY_CHARGED
};



/// ============================== RENDERING ================================

enum RDE_UI_ANCHOR_ {
	RDE_UI_ANCHOR_MIDDLE              = 1 << 1,
	RDE_UI_ANCHOR_LEFT                = 1 << 2,
	RDE_UI_ANCHOR_RIGHT               = 1 << 3,
	RDE_UI_ANCHOR_TOP                 = 1 << 4,
	RDE_UI_ANCHOR_BOTTOM              = 1 << 5,
	RDE_UI_ANCHOR_LEFT_BOTTOM         = 1 << 6,
	RDE_UI_ANCHOR_LEFT_TOP            = 1 << 7,
	RDE_UI_ANCHOR_RIGHT_BOTTOM        = 1 << 8,
	RDE_UI_ANCHOR_RIGHT_TOP           = 1 << 9
};

enum RDE_UI_STRETCH_ {
	RDE_UI_STRETCH_NO_STRETCH          = 1 << 10,
	RDE_UI_STRETCH_VERTICAL_STRETCH    = 1 << 11,
	RDE_UI_STRETCH_HORIZONTAL_STRETCH  = 1 << 12,
	RDE_UI_STRETCH_FULL_STRETCH        = 1 << 13
};

enum RDE_BATCH_PRIORITY_ {
	RDE_BATCH_PRIORITY_SPRITE = 0,
	RDE_BATCH_PRIORITY_TEXT = 1,
	RDE_BATCH_PRIORITY_NONE = 2,
};

enum RDE_RENDERIZABLE_TYPE_ {
	RDE_RENDERIZABLE_TYPE_NONE        = 0,
	RDE_RENDERIZABLE_TYPE_SPRITE      = 1,
	RDE_RENDERIZABLE_TYPE_TEXT        = 2,
	RDE_RENDERIZABLE_TYPE_UI_IMAGE    = 3,
	RDE_RENDERIZABLE_TYPE_UI_TEXT     = 4
};

enum RDE_UNIFORM_FV_ {
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
};

enum RDE_UNIFORM_IV_ {
	RDE_UNIFORM_IV_1,
	RDE_UNIFORM_IV_2,
	RDE_UNIFORM_IV_3,
	RDE_UNIFORM_IV_4
};

enum RDE_UNIFORM_UIV_ {
	RDE_UNIFORM_UIV_1,
	RDE_UNIFORM_UIV_2,
	RDE_UNIFORM_UIV_3,
	RDE_UNIFORM_UIV_4
};



/// ============================= FILE SYSTEM ===============================

enum RDE_IMAGE_EXTENSION_ {
	RDE_IMAGE_EXTENSION_PNG,
	RDE_IMAGE_EXTENSION_JPG,
	RDE_IMAGE_EXTENSION_BMP,
	RDE_IMAGE_EXTENSION_SVG
};

enum RDE_FILE_RESULT_ {
	RDE_FILE_RESULT_OK,
	RDE_FILE_RESULT_NOT_FOUND,
	RDE_FILE_RESULT_LINE_OUT_OF_SCOPE,
	RDE_FILE_RESULT_NOT_ALL_LINES_IN_SCOPE
};

enum RDE_FILE_MODE_ {
	RDE_FILE_MODE_READ,
	RDE_FILE_MODE_WRITE,
	RDE_FILE_MODE_APPEND,
	RDE_FILE_MODE_READ_AND_WRITE,
	RDE_FILE_MODE_READ_AND_APPEND,
};



/// =============================== PHYSICS =================================

enum RDE_PHYSICS_BODY_TYPE_ {
	RDE_PHYSICS_BODY_TYPE_STATIC,
	RDE_PHYSICS_BODY_TYPE_KINEMATIC,
	RDE_PHYSICS_BODY_TYPE_DYNAMIC
};

enum RDE_PHYSICS_SHAPE_TYPE_ {
	RDE_PHYSICS_SHAPE_TYPE_BOX,
	RDE_PHYSICS_SHAPE_TYPE_CIRCLE,
	RDE_PHYSICS_SHAPE_TYPE_POLYGON,
	RDE_PHYSICS_SHAPE_TYPE_SEGMENT
};



/// =============================== LOCALIZATION ============================

enum RDE_LANGUAGE_ { 
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
};






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

SPECIALIZED_VEC3(int, 			rde_vec_3I);
SPECIALIZED_VEC3(unsigned int, 	rde_vec_3UI);
SPECIALIZED_VEC3(float, 		rde_vec_3F);
SPECIALIZED_VEC3(double,		rde_vec_3D);
SPECIALIZED_VEC3(long,			rde_vec_3L);
SPECIALIZED_VEC3(unsigned long, rde_vec_3UL);

SPECIALIZED_VEC4(int, 			rde_vec_4I);
SPECIALIZED_VEC4(unsigned int, 	rde_vec_4UI);
SPECIALIZED_VEC4(float, 		rde_vec_4F);
SPECIALIZED_VEC4(double,		rde_vec_4D);
SPECIALIZED_VEC4(long,			rde_vec_4L);
SPECIALIZED_VEC4(unsigned long, rde_vec_4UL);

SPECIALIZED_MAT2(int, 			rde_mat_2I);
SPECIALIZED_MAT2(unsigned int, 	rde_mat_2UI);
SPECIALIZED_MAT2(float, 		rde_mat_2F);
SPECIALIZED_MAT2(double,		rde_mat_2D);
SPECIALIZED_MAT2(long,			rde_mat_2L);
SPECIALIZED_MAT2(unsigned long, rde_mat_2UL);

SPECIALIZED_MAT3(int, 			rde_mat_3I);
SPECIALIZED_MAT3(unsigned int, 	rde_mat_3UI);
SPECIALIZED_MAT3(float, 		rde_mat_3F);
SPECIALIZED_MAT3(double,		rde_mat_3D);
SPECIALIZED_MAT3(long,			rde_mat_3L);
SPECIALIZED_MAT3(unsigned long, rde_mat_3UL);

SPECIALIZED_MAT4(int, 			rde_mat_4I);
SPECIALIZED_MAT4(unsigned int, 	rde_mat_4UI);
SPECIALIZED_MAT4(float, 		rde_mat_4F);
SPECIALIZED_MAT4(double,		rde_mat_4D);
SPECIALIZED_MAT4(long,			rde_mat_4L);
SPECIALIZED_MAT4(unsigned long, rde_mat_4UL);

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



/// ================== CALLBACKS AND FUNCTION POINTERS ======================

struct rde_engine;
struct rde_window;
struct rde_event;

typedef void (*rde_event_func_inner)(rde_event*);
typedef void (*rde_event_func_outer)(rde_engine*, rde_window*, rde_event*);

struct rde_event_callback {
	bool block_engine_default_implementation = false;
	rde_event_func_inner _callback = nullptr;
};

struct rde_window_callbacks {
	rde_event_callback on_window_resize = {0};
	rde_event_callback on_window_focused_by_mouse = {0};
	rde_event_callback on_window_unfocused_by_mouse = {0};
	rde_event_callback on_window_focused_by_keyboard = {0};
	rde_event_callback on_window_unfocused_by_keyboard = {0};
	rde_event_callback on_window_moved = {0};
	rde_event_callback on_window_minimized = {0};
	rde_event_callback on_window_maximized = {0};
	rde_event_callback on_window_closed = {0};
	rde_event_callback on_window_display_changed = {0};
};

struct rde_display_callbacks {
	rde_event_callback on_display_connected = {0};
	rde_event_callback on_display_disconnected = {0};
	rde_event_callback on_display_changed_orientation = {0};
};

struct rde_mobile_callbacks {
	rde_event_callback on_app_terminating = {0}; 			/// Android -> onDestroy(), iOS -> applicationWillTerminate()
	rde_event_callback on_app_low_memory = {0}; 			/// Android -> onLowMemory(), iOS -> applicationDidReceiveMemoryWarning()
	rde_event_callback on_app_will_enter_background = {0}; 	/// Android -> onPause(), iOS -> applicationWillResignActive()
	rde_event_callback on_app_did_enter_background = {0}; 	/// Android -> onPause(), iOS -> applicationDidEnterBackground()
	rde_event_callback on_app_will_enter_foreground = {0}; 	/// Android -> onResume(), iOS -> applicationWillEnterForeground()
	rde_event_callback on_app_did_enter_foreground = {0}; 	/// Android -> onResume(), iOS -> applicationDidBecomeActive()
};

struct rde_controller_callbacks {
	rde_event_callback on_controller_added = {0};
	rde_event_callback on_controller_removed = {0};
	rde_event_callback on_controller_reassigned = {0};
};

struct rde_drag_and_drop_callbacks {
	rde_event_callback on_drag_and_drop_begin = {0};
	rde_event_callback on_drag_and_drop_end = {0};
};



/// ============================== ENGINE ===================================

struct rde_display_info {
	int index = -1;
};

// TODO: this include is absolutely temporary
#include "SDL2/SDL_video.h"
struct rde_window {
	SDL_Window* sdl_window = nullptr;
	SDL_GLContext sdl_gl_context;
	rde_display_info display_info;
};

struct rde_engine {
	float delta_time = 0.0f;
	float fixed_delta_time = 0.016f;

	float fixed_time_step_accumulator = 0.0f;
	RDE_PLATFORM_TYPE_ platform_type = RDE_PLATFORM_TYPE_UNSUPPORTED;

	bool running = true;
	bool use_rde_batching_system = true;

	rde_window* engine_windows[RDE_MAX_NUMBER_OF_WINDOWS];

	rde_display_callbacks display_callbacks;
	rde_window_callbacks window_callbacks;
};



/// ============================== EVENTS ===================================

struct rde_event_window {
	rde_vec_2I position { -1, -1 };
	rde_vec_2I size { -1, -1 };

	int display_index = -1;

	bool minimized = false;
	bool maximized = false;
};

struct rde_event_display {
	int orientation = -1;
	int display_index = -1;
};

struct rde_event_key {
	RDE_KEYBOARD_KEY_ key = RDE_KEYBOARD_KEY_NONE;

	char typed_char = '\0';

	const char* typed_text = nullptr;

	int amount_of_times_pressed = -1;
};

struct rde_event_mouse {
	rde_vec_2I position { -1, -1 };
	rde_vec_2F scrolled { -1.f, -1.f };

	RDE_MOUSE_BUTTON_ button = RDE_MOUSE_BUTTON_NONE;

	int amount_of_times_pressed = -1;
};

struct rde_event_controller {
	size_t controller_id;

	rde_vec_2F left_joystick { -1.f, -1.f };
	rde_vec_2F right_joystick { -1.f, -1.f };
	rde_vec_2F back_buttons { -1.f, -1.f };

	RDE_CONTROLLER_BUTTON_ button = RDE_CONTROLLER_BUTTON_NONE;
	RDE_CONTROLLER_AXIS_ axis = RDE_CONTROLLER_AXIS_NONE;
};

struct rde_event_mobile {
	rde_vec_2I init_touch_position { -1, -1 };
	rde_vec_2I end_touch_position { -1, -1 };

	float pressure = -1.f;

	int finger_id = -1;
};

struct rde_event_drag_and_drop {
	size_t window_id = 0;
	char* file_path = nullptr;
};

struct rde_event_data {
	rde_event_window window_event_data {  };
	rde_event_key key_event_data {  };
	rde_event_mouse mouse_event_data {  };
	rde_event_controller controller_event_data {  };
	rde_event_mobile mobile_event_data {  };
	rde_event_display display_event_data {  };
};

struct rde_event {
	RDE_EVENT_TYPE_ type = RDE_EVENT_TYPE_NONE;
	uint32_t time_stamp = 0;
	bool handled = false;
	rde_event_data data {  };

};



/// ============================ RENDERING ==================================

struct rde_color {
	unsigned char r, g, b, a;
};

struct rde_texture {
	UNIMPLEMENTED_STRUCT()
};

struct rde_cpu_texture {
	UNIMPLEMENTED_STRUCT()
};

struct rde_render_texture {
	UNIMPLEMENTED_STRUCT()
};

struct rde_polygon {
	UNIMPLEMENTED_STRUCT()
};

struct rde_mesh {
	UNIMPLEMENTED_STRUCT()
};

struct rde_shader {
	UNIMPLEMENTED_STRUCT()
};

struct rde_material_map {
	UNIMPLEMENTED_STRUCT()
};

struct rde_material {
	UNIMPLEMENTED_STRUCT()
};

struct rde_model_bone {
	UNIMPLEMENTED_STRUCT()
};

struct rde_model {
	UNIMPLEMENTED_STRUCT()
};

struct rde_model_animation {
	UNIMPLEMENTED_STRUCT()
};

struct rde_vertex_2d {
	rde_vec_3F position { 0.f, 0.f, 0.f };
	size_t color = 0xFFFFFF;
	rde_vec_2F texture_coordinates = { 0.f, 0.f };
};

struct rde_batch_2d {
	RDE_BATCH_PRIORITY_ priority = RDE_BATCH_PRIORITY_NONE;
	size_t id = 0;
	size_t layer = 0;
	rde_shader* shader = nullptr;
	rde_vertex_2d vertices[RDE_MAX_VERTICES_PER_BATCH];
	size_t texture_id = 0;
};

struct rde_batch_3d {
	UNIMPLEMENTED_STRUCT()
};




/// *************************************************************************************************
/// *                                		FUNCTIONS                         						*
/// *************************************************************************************************

/// ============================ MATH =======================================




/// ============================ LOGGING ====================================




/// ============================ UTIL =======================================




/// ============================ ENGINE =====================================

RDE_FUNC_EXTERNAL void				rde_engine_entry_point(int _argc, char** _argv, rde_engine* _engine, rde_window* _window);
RDE_FUNC_EXTERNAL void				rde_engine_user_on_update(float _dt);
RDE_FUNC_EXTERNAL void				rde_engine_user_on_fixed_update(float _dt);
RDE_FUNC_EXTERNAL void				rde_engine_user_on_late_upadte(float _dt);
RDE_FUNC_EXTERNAL void				rde_engine_user_on_render(float _dt);


RDE_FUNC 		rde_engine*			rde_engine_create_engine(int _argc, char** _argv);

RDE_FUNC 		RDE_PLATFORM_TYPE_ 	rde_engine_get_platform(rde_engine* _engine);

RDE_FUNC 		float 				rde_engine_get_fixed_delta(rde_engine* _engine);
RDE_FUNC 		void 				rde_engine_set_fixed_delta(rde_engine* _engine, float _fixed_dt);

RDE_FUNC		void				rde_engine_on_run(rde_engine* _engine);

RDE_FUNC		void				rde_engine_init_imgui_layer(rde_engine* _engine);
RDE_FUNC		void				rde_engine_end_imgui_layer(rde_engine* _engine);

RDE_FUNC		rde_window*			rde_engine_get_window(rde_engine* _engine, int _index);

RDE_FUNC		bool				rde_engine_is_running(rde_engine* _engine);
RDE_FUNC		void				rde_engine_set_running(rde_engine* _engine, bool _running);

RDE_FUNC		rde_vec_2I			rde_engine_get_display_size(rde_engine* _engine);

RDE_FUNC 		rde_display_info*	rde_engine_get_available_displays();
RDE_FUNC 		void				rde_engine_switch_window_display(rde_window* _window, size_t _new_display);

RDE_FUNC		void				rde_engine_destroy_engine(rde_engine* _engine);


/// ============================ WINDOW =====================================

RDE_FUNC rde_window*		rde_window_create_window(rde_engine* _engine);
RDE_FUNC	void			rde_window_set_callbacks(rde_window* _window, rde_window_callbacks _callbacks);

//RDE_FUNC_ND Vec2I			rde_window_get_window_size(rde_window* _window);
//RDE_FUNC	void			rde_window_set_window_size(rde_window* _window, const Vec2I& _size);
//
//RDE_FUNC_ND Vec2I			rde_window_get_position(rde_window* _window);
//RDE_FUNC_ND void			rde_window_set_position(rde_window* _window, const Vec2I& _position);
//
//RDE_FUNC_ND std::string 	rde_window_get_title(rde_window* _window);
//RDE_FUNC	void			rde_window_set_title(rde_window* _window, const std::string& _name);
//
//RDE_FUNC_ND bool			rde_window_is_fullscreen(rde_window* _window);
//RDE_FUNC 	void			rde_window_set_fullscreen(rde_window* _window, bool _fullscreen);
//
//RDE_FUNC_ND bool			rde_window_is_vsync_active(rde_window* _window);
//RDE_FUNC 	void			rde_window_set_vsync_active(rde_window* _window, bool _vsync);
//
//RDE_FUNC_ND SDL_Window* 	rde_window_get_native_sdl_window(rde_window* _window);
//RDE_FUNC_ND SDL_GLContext* 	rde_window_get_native_sdl_gl_window(rde_window* _window);
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
//RDE_FUNC	void 			rde_window_destroy_window(Engine* _engine, rde_window _window);

/// ============================ EVENTS =====================================

RDE_FUNC void rde_events_window_consume_events(rde_engine* _engine, rde_window* _window, rde_event* _event);
RDE_FUNC void rde_events_display_consume_events(rde_engine* _engine, rde_window* _window, rde_event* _event);

#if IS_MOBILE()
RDE_FUNC int rde_events_mobile_consume_events(void* _user_data, SDL_Event* _event);
#endif

/// ============================ RENDERING ==================================

RDE_FUNC void rde_rendering_draw_point_2d(const rde_vec_2F _position, const rde_color _color);


/// ============================ AUDIO ======================================




/// ============================ PHYSICS ====================================




/// ============================ FILE SYSTEM ================================





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


#endif