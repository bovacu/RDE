// Created by borja on 8/2/22.


#ifndef RDE_PLATFORM_H
#define RDE_PLATFORM_H

#include "core/systems/inputSystem/input/Input.h"
namespace RDE {

    enum RDE_PLATFORM_TYPE_ {
		RDE_PLATFORM_TYPE_LINUX,
		RDE_PLATFORM_TYPE_WINDOWS,
		RDE_PLATFORM_TYPE_MAC,
		RDE_PLATFORM_TYPE_ANDROID,
		RDE_PLATFORM_TYPE_IOS,
		RDE_PLATFORM_TYPE_UNSUPPORTED
    };

    class Platform {
		friend class Engine;

        public:
            Window* createWindow(RDEConfig* _config);

        private:
			RDE_PLATFORM_TYPE_ currentPlatform;
    };

}


#endif //RDE_PLATFORM_H
