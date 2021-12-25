// Created by borja on 24/12/21.


#ifndef ENGINE2_0_CORE_H
#define ENGINE2_0_CORE_H

#define DEFAULT_SHADER_PATH "assets/shaders/Texture.glsl"
#define PIXEL_SHADER "assets/shaders/FlatColor.glsl"

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
    #define ENGINE_ASSERT(x, ...) { if(!(x)) { LOG_E("Assertion Failed: {0}", __VA_ARGS__); ENGINE_DEBUG_BREAK(); } }
#else
    #define ENGINE_ASSERT(x, ...)
#endif

#define ENGINE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#endif //ENGINE2_0_CORE_H
