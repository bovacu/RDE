// Created by borja on 8/2/22.


#ifndef ENGINE2_0_PLATFORM_H
#define ENGINE2_0_PLATFORM_H

namespace GDE {

    enum PlatformType {
        LINUX,
        WINDOWS,
        MAC,
        ANDROID_,
        IOS
    };

    class Platform {

        public:
            static PlatformType getPlatform();

    };

}


#endif //ENGINE2_0_PLATFORM_H
