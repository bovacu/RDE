// Created by borja on 24/12/21.


#ifndef ENGINE2_0_CORE_H
#define ENGINE2_0_CORE_H

#define TEXTURE_VERTEX_SHADER_ES "assets/shaders/es/texture/vertex.glsl"
#define DEBUG_VERTEX_SHADER_ES "assets/shaders/es/debug/debugVertex.glsl"

#define TEXTURE_VERTEX_SHADER_CORE "assets/shaders/core/texture/vertex.glsl"
#define DEBUG_VERTEX_SHADER_CORE "assets/shaders/core/debug/debugVertex.glsl"

#define TEXTURE_FRAGMENT_SHADER_CORE "assets/shaders/core/texture/fragment.glsl"
#define TEXT_FRAGMENT_SHADER_CORE "assets/shaders/core/texture/textFragment.glsl"
#define OUTLINE_FRAGMENT_SHADER_CORE "assets/shaders/core/texture/outlineFragment.glsl"
#define NEON_FRAGMENT_SHADER_CORE "assets/shaders/core/texture/glowFragment.glsl"
#define BLOOM_FRAGMENT_SHADER_CORE "assets/shaders/core/texture/bloomFragment.glsl"
#define BLUR_FRAGMENT_SHADER_CORE "assets/shaders/core/texture/blurFragment.glsl"
#define DEBUG_FRAGMENT_SHADER_CORE "assets/shaders/core/debug/debugFragment.glsl"

#define TEXTURE_FRAGMENT_SHADER_ES "assets/shaders/es/texture/fragment.glsl"
#define TEXT_FRAGMENT_SHADER_ES "assets/shaders/es/texture/textFragment.glsl"
#define OUTLINE_FRAGMENT_SHADER_ES "assets/shaders/es/texture/outlineFragment.glsl"
#define NEON_FRAGMENT_SHADER_ES "assets/shaders/es/texture/glowFragment.glsl"
#define BLOOM_FRAGMENT_SHADER_ES "assets/shaders/es/texture/bloomFragment.glsl"
#define BLUR_FRAGMENT_SHADER_ES "assets/shaders/es/texture/blurFragment.glsl"
#define DEBUG_FRAGMENT_SHADER_ES "assets/shaders/es/debug/debugFragment.glsl"

#define ENGINE_DEBUG
#define ENGINE_ENABLE_ASSERTS

#define IS_MAC() (defined(__APPLE__) && defined(TARGET_OS_MAC))
#define IS_WINDOWS() (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#define IS_LINUX() (defined(__linux__))
#define IS_DESKTOP() (IS_LINUX() || IS_MAC() || IS_WINDOWS())

#define IS_IOS() (defined(__APPLE__) && (defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)))
#define IS_ANDROID() (defined(__ANDROID__))
#define IS_MOBILE() (IS_ANDROID() || IS_IOS())

#ifdef ENGINE_DEBUG
    #if defined(_WIN32)
		#define ENGINE_DEBUGBREAK() __debugbreak()
	#elif defined(__linux__)
		#include <csignal>
		#define ENGINE_DEBUG_BREAK() raise(SIGTRAP)
	#endif
	#define ENGINE_ENABLE_ASSERTS
#else
#define ENGINE_DEBUGBREAK()
#endif

#ifdef ENGINE_ENABLE_ASSERTS
    #define ENGINE_ASSERT(x, ...) { if(!(x)) { LOG_E("Assertion Failed: ", __VA_ARGS__); ENGINE_DEBUG_BREAK(); } }
#else
    #define ENGINE_ASSERT(x, ...)
#endif

#define ENGINE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return fn(std::forward<decltype(args)>(args)...); }

// This is for the EventBus
#define PLACEHOLDERS_1 std::placeholders::_1
#define PLACEHOLDERS_2 std::placeholders::_1, std::placeholders::_2
#define PLACEHOLDERS_3 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
#define PLACEHOLDERS_4 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4
#define PLACEHOLDERS_5 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5

#define BIND_FUNC_0(_method) std::bind(&_method, this)
#define BIND_FUNC_1(_method) std::bind(&_method, this, PLACEHOLDERS_1)
#define BIND_FUNC_2(_method) std::bind(&_method, this, PLACEHOLDERS_2)
#define BIND_FUNC_3(_method) std::bind(&_method, this, PLACEHOLDERS_3)
#define BIND_FUNC_4(_method) std::bind(&_method, this, PLACEHOLDERS_4)
#define BIND_FUNC_5(_method) std::bind(&_method, this, PLACEHOLDERS_5)

#endif //ENGINE2_0_CORE_H
