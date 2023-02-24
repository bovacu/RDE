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

    Window* Platform::createWindow(RDEConfig* _config) {
        #if IS_ANDROID()
			currentPlatform = RDE_PLATFORM_TYPE_ANDROID;
            return new AndroidWindow(_config);
        #endif

        #if IS_IOS()
			currentPlatform = RDE_PLATFORM_TYPE_IOS;
            return new IOSWindow(_config);
        #endif

        #if IS_LINUX() && !IS_ANDROID()
			currentPlatform = RDE_PLATFORM_TYPE_LINUX;
            return new LinuxWindow(_config);
        #endif

        #if IS_WINDOWS() && !IS_ANDROID()
			currentPlatform = RDE_PLATFORM_TYPE_WINDOWS;
            return new WindowsWindow(_config);
        #endif

        #if IS_MAC() && !IS_ANDROID()
			currentPlatform = RDE_PLATFORM_TYPE_MAC;
            return new MacWindow(_config);
        #endif

        return nullptr;
    }
}
