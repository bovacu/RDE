// Created by borja on 8/2/22.


#ifndef ENGINE2_0_PLATFORM_H
#define ENGINE2_0_PLATFORM_H

#include "core/systems/inputSystem/input/Input.h"
namespace GDE {

    enum PlatformType {
        LINUX,
        WINDOWS,
        MAC,
        ANDROID_,
        IOS,
        UNSUPPORTED
    };

    class Platform {
        public:
            Window* createWindow(GDEConfig* _config);

        private:
            PlatformType getPlatform();
    };

}


#endif //ENGINE2_0_PLATFORM_H
