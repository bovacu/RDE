// Created by borja on 8/2/22.


#include "core/platform/Platform.h"
#include "core/Core.h"

namespace engine {

    PlatformType Platform::getPlatform() {

        #if IS_ANDROID()
            return PlatformType::ANDROID_;
        #endif

        #if IS_IOS()
            return Platform::IOS:
        #endif

        #if IS_LINUX()
            return PlatformType::LINUX;
        #elif IS_WINDOWS()
            return PlatformType::WINDOWS;
        #elif IIS_MAC
            return Platform::MAC;
        #endif
    }
}