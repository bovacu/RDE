#include "platform.hpp"

namespace engine {
    Platform getCurrentPlatform() {
    #if defined(__APPLE__)
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        return engine::Platform::ios;
    #else
        return engine::Platform::mac;
    #endif
    #elif __ANDROID__
        return engine::Platform::android;
    #endif
    }
}
