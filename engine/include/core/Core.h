// Created by borja on 24/12/21.


#ifndef GDE_CORE_H
#define GDE_CORE_H

#define FRAMEBUFFER_VERTEX_SHADER_ES "defaultAssets/shaders/es/framebuffer/FrameBufferVertex.glsl"
#define FRAMEBUFFER_FRAGMENT_SHADER_ES "defaultAssets/shaders/es/framebuffer/FrameBufferFragment.glsl"
#define FRAMEBUFFER_VERTEX_SHADER_CORE "defaultAssets/shaders/core/framebuffer/FrameBufferVertex.glsl"
#define FRAMEBUFFER_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/framebuffer/FrameBufferFragment.glsl"
#define RML_FRAGMENT_SHADER_CORE "defaultAssets/shaders/core/texture/rmlFragment.glsl"
#define RML_FRAGMENT_COLOR_ONLY_SHADER_CORE "defaultAssets/shaders/core/texture/rmlFragmentColorOnly.glsl"
#define RML_VERTEX_SHADER_CORE "defaultAssets/shaders/core/texture/rmlVertex.glsl"

#define TEXTURE_VERTEX_SHADER_ES "defaultAssets/shaders/es/texture/vertex.glsl"
#define DEBUG_VERTEX_SHADER_ES "defaultAssets/shaders/es/debug/debugVertex.glsl"

#define TEXTURE_VERTEX_SHADER_CORE "defaultAssets/shaders/core/texture/vertex.glsl"
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


#ifndef EPSILON
#define EPSILON 0.0001f
#endif

#ifndef PI
#define PI 3.141592741f
#endif

#define radiansToDegrees(x) x * (180.f / PI)
#define degreesToRadians(x) x * (PI / 180.0f)

#ifndef NDEBUG
#define ENGINE_DEBUG
#endif

#ifdef ENGINE_DEBUG
    #if defined(_WIN32)
		#define ENGINE_DEBUG_BREAK() __debugbreak()
        #define ENGINE_ENABLE_ASSERTS
    #elif defined(__APPLE__)
        #include "TargetConditionals.h"
        #if TARGET_OS_IPHONE
            #ifndef IOS_PLATFORM
                #define IOS_PLATFORM
            #endif
        #elif TARGET_OS_MAC
            #define MAC_PLATFORM
        #endif
	#elif defined(__linux__)
		#include <csignal>
		#define ENGINE_DEBUG_BREAK() raise(SIGTRAP)
        #define ENGINE_ENABLE_ASSERTS
	#endif
#else
#define ENGINE_DEBUGBREAK()
#endif

#define IS_MAC() (defined(__APPLE__) && defined(MAC_PLATFORM))
#define IS_WINDOWS() (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#define IS_LINUX() (defined(__linux__))
#define IS_DESKTOP() (IS_LINUX() || IS_MAC() || IS_WINDOWS())

#define IS_IOS() (defined(__APPLE__) && defined(IOS_PLATFORM))
#define IS_ANDROID() (defined(__ANDROID__))
#define IS_MOBILE() (IS_ANDROID() || IS_IOS())

#ifdef ENGINE_ENABLE_ASSERTS
    #define ENGINE_ASSERT(x, ...) { if(!(x)) { LOG_E("Assertion Failed: ", __VA_ARGS__); ENGINE_DEBUG_BREAK(); } }
#else
    #define ENGINE_ASSERT(x, ...)
#endif

#ifdef GDE_EXPORT
    #define GDE_API [[gnu::visibility("default")]]
    #define GDE_HIDDEN [[gnu::visibility("hidden")]]
    #define GDE_INTERNAL [[gnu::visibility("internal")]]
    #define GDE_DEPRECATED(_explanation) [[gnu::deprecated(_explanation)]]
#endif

#if IS_MOBILE() || IS_MAC() || defined(_WIN32)
typedef unsigned long ulong;
typedef unsigned int uint;
#endif

#endif //GDE_CORE_H
