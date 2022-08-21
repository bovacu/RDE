// Created by borja on 8/2/22.


#include "core/platform/Platform.h"
#include "core/Core.h"
#include "core/render/window/LinuxWindow.h"
#include "core/render/window/WindowsWindow.h"
#include "core/render/window/MacWindow.h"
#include "core/render/window/AndroidWindow.h"
#include "core/render/window/IOSWindow.h"

namespace GDE {

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

    Window* Platform::createWindow(GDEConfig* _config) {
        switch(getPlatform()) {
            case WINDOWS: return new WindowsWindow(_config);
            case LINUX: return new LinuxWindow(_config);
            case MAC: return new MacWindow(_config);
            case ANDROID_: return new AndroidWindow(_config);
            case IOS: return new IOSWindow(_config);
            case UNSUPPORTED: {
                LOG_E("Trying to create a window for an unsupported platform!!")
                return nullptr;
            }
        }
        return nullptr;
    }
}
