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

#ifdef FORCE_DEBUG
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

#define FE_1_INVOKE(WHAT, X) WHAT(X)
#define FE_2_INVOKE(WHAT, X, ...) WHAT(X)FE_1_INVOKE(WHAT, __VA_ARGS__)
#define FE_3_INVOKE(WHAT, X, ...) WHAT(X)FE_2_INVOKE(WHAT, __VA_ARGS__)
#define FE_4_INVOKE(WHAT, X, ...) WHAT(X)FE_3_INVOKE(WHAT, __VA_ARGS__)
#define FE_5_INVOKE(WHAT, X, ...) WHAT(X)FE_4_INVOKE(WHAT, __VA_ARGS__)
//... repeat as needed

#define FE_1_DECLARATION(WHAT, X, Y) WHAT Y X
#define FE_2_DECLARATION(WHAT, X, Y, ...) WHAT Y X FE_1_DECLARATION(WHAT, X, __VA_ARGS__)
#define FE_3_DECLARATION(WHAT, X, Y, ...) WHAT Y X FE_2_DECLARATION(WHAT, X, __VA_ARGS__)
#define FE_4_DECLARATION(WHAT, X, Y, ...) WHAT Y X FE_3_DECLARATION(WHAT, X, __VA_ARGS__)
#define FE_5_DECLARATION(WHAT, X, Y, ...) WHAT Y X FE_4_DECLARATION(WHAT, X, __VA_ARGS__)
#define FE_6_DECLARATION(WHAT, X, Y, ...) WHAT Y X FE_5_DECLARATION(WHAT, X, __VA_ARGS__)
#define FE_7_DECLARATION(WHAT, X, Y, ...) WHAT Y X FE_6_DECLARATION(WHAT, X, __VA_ARGS__)
#define FE_8_DECLARATION(WHAT, X, Y, ...) WHAT Y X FE_7_DECLARATION(WHAT, X, __VA_ARGS__)
#define FE_9_DECLARATION(WHAT, X, Y, ...) WHAT Y X FE_8_DECLARATION(WHAT, X, __VA_ARGS__)
#define FE_10_DECLARATION(WHAT, X, Y, ...) WHAT Y X FE_9_DECLARATION(WHAT, X, __VA_ARGS__)

#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,NAME, ...) NAME
#define FOR_EACH_DECLARATION(init, end, ...) \
  GET_MACRO(__VA_ARGS__,FE_10_DECLARATION,FE_9_DECLARATION,FE_8_DECLARATION,FE_7_DECLARATION,FE_6_DECLARATION,FE_5_DECLARATION,FE_4_DECLARATION,FE_3_DECLARATION,FE_2_DECLARATION,FE_1_DECLARATION)(init, end,__VA_ARGS__)

#define FOR_EACH_INVOKE(action,...) \
  GET_MACRO(__VA_ARGS__,FE_5_INVOKE,FE_4_INVOKE,FE_3_INVOKE,FE_2_INVOKE,FE_1_INVOKE)(action,__VA_ARGS__)


#define FRIEND_CLASS(...) FOR_EACH_DECLARATION(friend class, ;, __VA_ARGS__)
#define FORWARD_DECLARE_CLASS(...) FOR_EACH_DECLARATION(class, ;, __VA_ARGS__)
#define FRIEND_STRUCT(...) FOR_EACH_DECLARATION(friend struct, ;, __VA_ARGS__)
#define FORWARD_DECLARE_STRUCT(...) FOR_EACH_DECLARATION(struct, ;, __VA_ARGS__)

#endif //GDE_CORE_H
