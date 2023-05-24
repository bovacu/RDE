// Created by borja on 24/12/21.

#include <iostream>

#ifndef RDE_CORE_H
#define RDE_CORE_H

/// \cond DO_NOT_DOCUMENT
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

#define RESOURCES_PATH "assets/"
/// \endcond

/**
 * @brief Used for physics calculations.
*/
#ifndef EPSILON
#define EPSILON 0.0001f
#endif

/**
 * @brief Used for math calculations.
*/
#ifndef PI
#define PI 3.141592741f
#endif

/**
 * @brief Used for UI size calculations.
*/
#ifndef BASE_DPI
#define BASE_DPI 96.f
inline float currentDPI = 0;
#endif

/**
 * @brief Used for UI size calculations.
*/
#define toDPI(x) x * (currentDPI / BASE_DPI)

/**
 * @brief Used for UI size calculations.
*/
#define toDPI_V(vec) Vec2F {vec.x * (currentDPI / BASE_DPI), vec.y * (currentDPI / BASE_DPI)}

/// \cond DO_NOT_DOCUMENT
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
	#endif
#else
#define ENGINE_DEBUGBREAK()
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
/// \endcond

/**
 * @brief Checks if the OS is Mac.
*/
#define IS_MAC() (defined(__APPLE__) && defined(MAC_PLATFORM))

/**
 * @brief Checks if the OS is Windows.
*/
#define IS_WINDOWS() _WIN32

/**
 * @brief Checks if the OS is Linux.
*/
#define IS_LINUX() (defined(__linux__))

/**
 * @brief Checks if the OS is Mac, Windows or Linux.
*/
#define IS_DESKTOP() (IS_LINUX() || IS_MAC() || IS_WINDOWS())

/**
 * @brief Checks if the OS is iOS.
*/
#define IS_IOS() (defined(__APPLE__) && defined(IOS_PLATFORM))

#define IS_APPLE() (IS_MAC() || IS_IOS())

/**
 * @brief Checks if the OS is Android.
*/
#define IS_ANDROID() (defined(__ANDROID__))

/**
 * @brief Checks if it is Android or iOS
*/
#define IS_MOBILE() (IS_ANDROID() || IS_IOS())


/// \cond DO_NOT_DOCUMENT
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
/// \endcond


#if IS_MOBILE() || IS_MAC() || defined(_WIN32)
typedef unsigned long ulong;
typedef unsigned int uint;
#endif

#ifdef __EMSCRIPTEN__
typedef unsigned long ulong;
typedef unsigned int uint;
#endif

/**
 * @brief Makes a class iterable onto some itarable attribute, like a map or a vector.
*/
#define MAKE_CLASS_ITERABLE(iteratorType, iterable)                               \
public:                                                                           \
    iteratorType::iterator begin()                  { return iterable.begin();  } \
    iteratorType::iterator end()                    { return iterable.end();    } \
    iteratorType::reverse_iterator rbegin()         { return iterable.rbegin(); } \
    iteratorType::reverse_iterator rend()           { return iterable.rend();   } \
    iteratorType::const_iterator cbegin()           { return iterable.cbegin(); } \
    iteratorType::const_iterator cend()             { return iterable.cend();   } \
    iteratorType::const_reverse_iterator crbegin()  { return iterable.crbegin();} \
    iteratorType::const_reverse_iterator crend()    { return iterable.crend();  } \

#define SAFE_POINTER(_var, _function) if(_var) do { _var->_function; } while(0);

#if IS_WINDOWS()
//#ifdef RDE_LIBRARY
#define RDE_FUNC __declspec(dllexport)
#define RDE_FUNC_ND [[nodiscard]] __declspec(dllexport)
#define RDE_FUNC_EXPLICIT __declspec(dllexport) explicit
#define RDE_FUNC_STATIC __declspec(dllexport) static
#define RDE_FUNC_ND_STATIC [[nodiscard]] __declspec(dllexport) static
#define RDE_FUNC_ENGINE extern "C" RDE_FUNC
//#else
//#define RDE_FUNC __declspec(dllimport)
//#define RDE_FUNC_ND [[nodiscard]] __declspec(dllimport)
//#define RDE_FUNC_EXPLICIT __declspec(dllimport) explicit
//#endif
#else
#define RDE_FUNC 
#define RDE_FUNC_ND [[nodiscard]]
#define RDE_FUNC_STATIC static
#define RDE_FUNC_EXPLICIT explicit
#define RDE_FUNC_ENGINE
#define RDE_FUNC_ND_STATIC [[nodiscard]] static
#endif

#endif //RDE_CORE_H
