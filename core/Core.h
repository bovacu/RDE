// Created by borja on 24/12/21.


#ifndef ENGINE2_0_CORE_H
#define ENGINE2_0_CORE_H

#define TEXTURE_VERTEX_SHADER "assets/shaders/texture/vertex.glsl"
#define DEBUG_VERTEX_SHADER "assets/shaders/debug/debugVertex.glsl"

#define TEXTURE_FRAGMENT_SHADER "assets/shaders/texture/fragment.glsl"
#define TEXT_FRAGMENT_SHADER "assets/shaders/texture/textFragment.glsl"
#define OUTLINE_FRAGMENT_SHADER "assets/shaders/texture/outlineFragment.glsl"
#define NEON_FRAGMENT_SHADER "assets/shaders/texture/glowFragment.glsl"
#define BLOOM_FRAGMENT_SHADER "assets/shaders/texture/bloomFragment.glsl"
#define BLUR_FRAGMENT_SHADER "assets/shaders/texture/blurFragment.glsl"
#define DEBUG_FRAGMENT_SHADER "assets/shaders/debug/debugFragment.glsl"

#define ENGINE_DEBUG
#define ENGINE_ENABLE_ASSERTS

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

#define ENGINE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

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
