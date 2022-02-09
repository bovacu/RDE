// Created by borja on 8/2/22.


#include "core/platform/Platform.h"

namespace engine {

    PlatformType Platform::getPlatform() {

        #ifdef __ANDROID__
            return PlatformType::ANDROID_;
        #endif

        #ifdef IOS_PLATFORM
            return Platform::IOS:
        #endif

        #ifdef __linux__
            return PlatformType::LINUX;
        #elif _WIN32
            return PlatformType::WINDOWS;
        #else defined(__APPLE__)
            return Platform::MAC;
        #endif
    }
}