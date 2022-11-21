// Created by borja on 8/2/22.


#include "core/platform/Platform.h"
#include "core/Core.h"

#if IS_LINUX() && !IS_ANDROID()
#include "core/render/window/LinuxWindow.h"
#endif

#if IS_WINDOWS() && !IS_ANDROID()
#include "core/render/window/WindowsWindow.h"
#endif

#if IS_MAC() && !IS_ANDROID()
#include "core/render/window/MacWindow.h"
#endif

#if IS_ANDROID()
#include "core/render/window/AndroidWindow.h"
#endif

#if IS_IOS()
#include "core/render/window/IOSWindow.h"
#endif

namespace RDE {

    PlatformType Platform::getPlatform() {

        #if IS_ANDROID()
            return PlatformType::ANDROID_;
        #endif

        #if IS_IOS()
            return PlatformType::IOS;
        #endif

        #if IS_LINUX()
            return PlatformType::LINUX;
        #endif

        #if IS_WINDOWS()
            return PlatformType::WINDOWS;
        #endif

        #if IS_MAC()
            return PlatformType::MAC;
        #endif

        return PlatformType::UNSUPPORTED;
    }

    Window* Platform::createWindow(RDEConfig* _config) {
        #if IS_ANDROID()
            return new AndroidWindow(_config);
        #endif

        #if IS_IOS()
            return new IOSWindow(_config);
        #endif

        #if IS_LINUX() && !IS_ANDROID()
            return new LinuxWindow(_config);
        #endif

        #if IS_WINDOWS() && !IS_ANDROID()
            return new WindowsWindow(_config);
        #endif

        #if IS_MAC() && !IS_ANDROID()
            return new MacWindow(_config);
        #endif
    }
}
