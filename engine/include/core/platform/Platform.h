// Created by borja on 8/2/22.


#ifndef RDE_PLATFORM_H
#define RDE_PLATFORM_H

#include "core/systems/inputSystem/input/Input.h"
namespace RDE {

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
            Window* createWindow(RDEConfig* _config);

        private:
            PlatformType getPlatform();
    };

}


#endif //RDE_PLATFORM_H
