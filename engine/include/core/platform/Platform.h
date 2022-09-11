// Created by borja on 8/2/22.


#ifndef GDE_PLATFORM_H
#define GDE_PLATFORM_H

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


#endif //GDE_PLATFORM_H
