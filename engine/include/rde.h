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
#include <float.h>

/// *************************************************************************************************
/// *                                		DEFINES                         						*
/// *************************************************************************************************

/// ========================= NUMERIC CONSTANTS ============================

#define RDE_EPSILON 0.0001f
#define RDE_PI 3.141592741f

#define RDE_MAX_NUMBER_OF_WINDOWS 10
#define RDE_MAX_VERTICES_PER_BATCH 50000
#define RDE_MAX_LOADABLE_SHADERS 256
#define RDE_MAX_LOADABLE_TEXTURES 512
#define RDE_MAX_LOADABLE_ATLASES 512

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
#define IS_WASM() __EMSCRIPTEN__
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
		std::ios::sync_with_stdio(1);						\
	}

/// ============================== UTIL ====================================

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define LINE_STRING STRINGIZE(__LINE__)
#define FUNC_STRING STRINGIZE(__func__)
#define FILE_STRING STRINGIZE(__FILE__)



/// ======================= WARNING SILENCER ===============================

#define UNUSED(_x) (void)_x;

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
	void _func_name(rde_window* _window, rde_event* _event) {					\
		UNUSED(_window);																					\
		if(!ENGINE._callback_type._callback_name.block_engine_default_implementation) {					\
			_extra_code																						\
		}																									\
																											\
		if(ENGINE._callback_type._callback_name._callback != nullptr) {									\
			ENGINE._callback_type._callback_name._callback(_event);										\
		}																									\
	}

#define RDE_LOG_B_BLACK() printf("\033[0;30m");
#define RDE_LOG_B_RED() printf("\033[0;31m");
#define RDE_LOG_B_GREEN() printf("\033[0;32m");
#define RDE_LOG_B_YELLOW() printf("\033[0;33m");
#define RDE_LOG_B_BLUE() printf("\033[0;34m");
#define RDE_LOG_B_PURPLE() printf("\033[0;35m");
#define RDE_LOG_B_CYAN() printf("\033[0;36m");
#define RDE_LOG_B_WHITE() printf("\033[0;37m");
#define RDE_LOG_E() printf("\033[0m");
#define RDE_LOG_E_NL() printf("\033[0m \n");

/// ====================== PLATFORM SPECIFIC TYPES ==========================

typedef unsigned long ulong;
typedef unsigned int uint;






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
	RDE_EVENT_TYPE_KEY_PRESSED, RDE_EVENT_TYPE_KEY_RELEASED, RDE_EVENT_TYPE_KEY_TYPED, RDE_EVENT_TYPE_TEXT_TYPED, 
	RDE_EVENT_TYPE_KEY_END,	

	RDE_EVENT_TYPE_MOUSE_BEGIN,
	RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED, RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED, RDE_EVENT_TYPE_MOUSE_SCROLLED, RDE_EVENT_TYPE_MOUSE_MOVED,
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

enum RDE_CAMERA_TYPE_ {
	RDE_CAMERA_TYPE_PERSPECTIVE,
	RDE_CAMERA_TYPE_ORTHOGRAPHIC
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

typedef struct rde_random rde_random;

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

struct rde_probability {
	float probability_rolled = 0.f;
	bool happened = false;
};

/// ================== CALLBACKS AND FUNCTION POINTERS ======================

struct rde_engine;
struct rde_window;
struct rde_event;

typedef void (*rde_event_func_inner)(rde_event*);
typedef void (*rde_event_func_outer)(rde_window*, rde_event*);
typedef void (*rde_engine_user_side_loop_func)(float);
typedef void (*rde_engine_user_side_loop_func_2)(float, rde_window*);

typedef struct rde_inner_window_data rde_inner_window_info;

struct rde_event_callback {
	bool block_engine_default_implementation = false;
	rde_event_func_inner _callback = nullptr;
};

struct rde_window_callbacks {
	rde_event_callback on_window_resize = {  };
	rde_event_callback on_window_focused_by_mouse = {  };
	rde_event_callback on_window_unfocused_by_mouse = {  };
	rde_event_callback on_window_focused_by_keyboard = {  };
	rde_event_callback on_window_unfocused_by_keyboard = {  };
	rde_event_callback on_window_moved = {  };
	rde_event_callback on_window_minimized = {  };
	rde_event_callback on_window_maximized = {  };
	rde_event_callback on_window_closed = {  };
	rde_event_callback on_window_display_changed = {  };
};

struct rde_display_callbacks {
	rde_event_callback on_display_connected = {  };
	rde_event_callback on_display_disconnected = {  };
	rde_event_callback on_display_changed_orientation = {  };
};

struct rde_mobile_callbacks {
	rde_event_callback on_app_terminating = {  }; 			/// Android -> onDestroy(), iOS -> applicationWillTerminate()
	rde_event_callback on_app_low_memory = {  }; 			/// Android -> onLowMemory(), iOS -> applicationDidReceiveMemoryWarning()
	rde_event_callback on_app_will_enter_background = {  }; 	/// Android -> onPause(), iOS -> applicationWillResignActive()
	rde_event_callback on_app_did_enter_background = {  }; 	/// Android -> onPause(), iOS -> applicationDidEnterBackground()
	rde_event_callback on_app_will_enter_foreground = {  }; 	/// Android -> onResume(), iOS -> applicationWillEnterForeground()
	rde_event_callback on_app_did_enter_foreground = {  }; 	/// Android -> onResume(), iOS -> applicationDidBecomeActive()
};

struct rde_controller_callbacks {
	rde_event_callback on_controller_added = {  };
	rde_event_callback on_controller_removed = {  };
	rde_event_callback on_controller_reassigned = {  };
};

struct rde_drag_and_drop_callbacks {
	rde_event_callback on_drag_and_drop_begin = {  };
	rde_event_callback on_drag_and_drop_end = {  };
};

struct rde_end_user_mandatory_callbacks {
	rde_engine_user_side_loop_func on_update = nullptr;
	rde_engine_user_side_loop_func on_fixed_update = nullptr;
	rde_engine_user_side_loop_func on_late_update = nullptr;
	rde_engine_user_side_loop_func_2 on_render = nullptr;
};

/// ============================== ENGINE ===================================

struct rde_display_info {
	int index = -1;
};

typedef struct rde_window rde_window;


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
	ulong time_stamp = 0;
	uint window_id = 0;
	bool handled = false;
	rde_event_data data {  };

};

struct rde_transform {
	rde_vec_3F position = { 0.f, 0.f, 0.f };
	rde_vec_3F rotation = { 0.f, 0.f, 0.f };
	rde_vec_3F scale = { 1.f, 1.f, 1.f };

	rde_transform* parent;
};

/// ============================ RENDERING ==================================

typedef struct rde_shader rde_shader;
typedef struct rde_texture rde_texture;
typedef struct rde_cpu_texture rde_cpu_texture;
typedef struct rde_render_texture rde_render_texture;
typedef struct rde_atlas rde_atlas;

struct rde_color {
	unsigned char r, g, b, a;
};

struct rde_polygon {
	rde_vec_2I* vertices = nullptr;
	size_t vertices_count = 0;
};

struct rde_mesh {
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
	int color = 0xFFFFFF;
	rde_vec_2F texture_coordinates = { 0.f, 0.f };
};

struct rde_viewport {
	UNIMPLEMENTED_STRUCT()
};

struct rde_camera {
	size_t id = 0;
	float zoom = 1.f;
	rde_transform transform = {  };
	RDE_CAMERA_TYPE_ camera_type = RDE_CAMERA_TYPE_ORTHOGRAPHIC;
	bool enabled = true;
};

struct rde_font {
	rde_vec_2I position;
	rde_vec_2I size;
};

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




/// *************************************************************************************************
/// *                                		FUNCTIONS                         						*
/// *************************************************************************************************

/// ============================ UTIL =======================================


/// ============================ MATH =======================================

RDE_FUNC rde_random* rde_math_create_random(long _seed = -1);
RDE_FUNC void rde_math_set_random_seed(long _seed);
RDE_FUNC int rde_math_get_random_int(int _min_included = INT_MIN, int _max_not_included = INT_MAX);
RDE_FUNC float rde_math_get_random_float(float _min_included = FLT_MIN, float _max_not_included = FLT_MAX);
RDE_FUNC rde_probability rde_math_get_probability(float _normalized_chance_to_happen);

RDE_FUNC void rde_math_convert_world_position_to_screen_coordinates(rde_window* _window, rde_vec_3F* _vec);
RDE_FUNC void rde_math_convert_world_position_to_screen_coordinates(rde_window* _window, rde_vec_2F* _vec);
RDE_FUNC void rde_math_convert_world_size_to_screen_size(rde_window* _window, rde_vec_2F* _vec);

#define RDE_DECLARE_CLAMP_FUNCS(_type) \
RDE_FUNC inline _type rde_math_clamp_##type(_type _value, _type _min, _type _max);

RDE_DECLARE_CLAMP_FUNCS(int)
RDE_DECLARE_CLAMP_FUNCS(uint)
RDE_DECLARE_CLAMP_FUNCS(float)
RDE_DECLARE_CLAMP_FUNCS(double)
RDE_DECLARE_CLAMP_FUNCS(long)
RDE_DECLARE_CLAMP_FUNCS(ulong)
RDE_DECLARE_CLAMP_FUNCS(size_t)

RDE_FUNC inline float rde_radians_to_degrees(float _radians);
RDE_FUNC inline float rde_degrees_to_radians(float _degrees);

#define RDE_DECLARE_EASING_FUNCS(_form, _type) \
	RDE_FUNC inline float rde_easing_##_form##_##_type(float _current_time, float _start_value, float _change_in_value, float _duration);

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


RDE_FUNC rde_window* rde_engine_create_engine(int _argc, char** _argv);
RDE_FUNC void rde_setup_initial_info(const rde_end_user_mandatory_callbacks _end_user_callbacks);

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
RDE_FUNC unsigned char* getAreaOfScreenPixels(rde_window* _window, rde_vec_2I _position, rde_vec_2I _size);

RDE_FUNC float rde_window_get_aspect_ratio(rde_window* _window);

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

#if IS_MOBILE()
RDE_FUNC int rde_events_mobile_consume_events(void* _user_data, SDL_Event* _event);
#endif

/// ============================ RENDERING ==================================

RDE_FUNC rde_shader* rde_rendering_load_shader(const char* _vertex_code, const char* _fragment_code);
RDE_FUNC void rde_rendering_set_shader_uniform_value_float(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_FV_ _type, float* _data, bool _transpose = false);
RDE_FUNC void rde_rendering_set_shader_uniform_value_int(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_UIV_ _type, int* _data);
RDE_FUNC void rde_rendering_set_shader_uniform_value_uint(rde_shader* _shader, const char* _uniform_name, RDE_UNIFORM_UIV_ _type, uint* _data);
RDE_FUNC void rde_rendering_unload_shader(rde_shader* _shader);

RDE_FUNC rde_texture* rde_rendering_load_texture(const char* _file_path);
RDE_FUNC void rde_rendering_unload_texture(rde_texture* _texture);

RDE_FUNC rde_atlas* rde_rendering_load_atlas(const char* _texture_path, const char* _config_path);
RDE_FUNC rde_texture* rde_rendering_get_atlas_sub_texture(rde_atlas* _atlas, const char* _texture_name);
RDE_FUNC void rde_rendering_unload_atlas(rde_atlas* _atlas);

RDE_FUNC rde_texture* rde_rendering_create_cpu_texture(rde_vec_2UI _texture_size);
RDE_FUNC void rde_rendering_destroy_cpu_texture(rde_cpu_texture* _cpu_texture);
RDE_FUNC void rde_rendering_upload_cpu_texture_to_gpu(rde_cpu_texture* _cpu_texture);

RDE_FUNC rde_font* rde_rendering_load_font(const char* _file_path);
RDE_FUNC void rde_rendering_unload_font(rde_font* _font);

RDE_FUNC void rde_rendering_set_background_color(const rde_color _color);

RDE_FUNC void rde_rendering_begin_drawing_2d(rde_camera* _camera, rde_window* _window);
RDE_FUNC void rde_rendering_begin_drawing_3d(rde_camera* _camera, rde_window* _window);

RDE_FUNC void rde_rendering_draw_point_2d(rde_vec_2F _position, rde_color _color, rde_shader* _shader = nullptr);
RDE_FUNC void rde_rendering_draw_point_3d(rde_vec_3F _position, rde_color _color, rde_shader* _shader = nullptr);

RDE_FUNC void rde_rendering_draw_line_2d(rde_vec_2F _init, rde_vec_2F _end, rde_color _color, rde_shader* _shader = nullptr);
RDE_FUNC void rde_rendering_draw_line_3d(rde_vec_3F _init, rde_vec_3F _end, rde_color _color, rde_shader* _shader = nullptr);

RDE_FUNC void rde_rendering_draw_triangle_2d(rde_vec_2F _verte_a, rde_vec_2F _vertex_b, rde_vec_2F _vertex_c, rde_color _color, rde_shader* _shader = nullptr);
RDE_FUNC void rde_rendering_draw_rectangle_2d(rde_vec_2F _bottom_left, rde_vec_2F _top_right, rde_color _color, rde_shader* _shader = nullptr);
RDE_FUNC void rde_rendering_draw_circle_2d(rde_vec_2F _position, float _radius, rde_color _color, rde_shader* _shader = nullptr);
RDE_FUNC void rde_rendering_draw_polygon_2d(const rde_transform* _transform, const rde_polygon* _polygon, rde_color _color, const rde_shader* _shader = nullptr);

RDE_FUNC void rde_rendering_draw_texture(const rde_transform* _transform, rde_texture* _texture, rde_color _tintColor = RDE_COLOR_WHITE, rde_shader* _shader = nullptr);

RDE_FUNC void rde_rendering_end_drawing_2d();
RDE_FUNC void rde_rendering_end_drawing_3d();

/// ============================ AUDIO ======================================

#ifdef RDE_INCLUDE_AUDIO_MODULE

#endif

/// ============================ PHYSICS ====================================

#ifdef RDE_INCLUDE_PHYSICS_MODULE

#endif

/// ============================ FILE SYSTEM ================================

#endif