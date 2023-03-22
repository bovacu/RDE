// Created by borja on 8/2/22.


#ifndef RDE_PLATFORM_H
#define RDE_PLATFORM_H

#include "core/Enums.h"
#include "core/Core.h"

namespace RDE {

	struct RDEConfig;
	class Window;

    class Platform {
		friend class Engine;

        public:
			RDE_FUNC Window* createWindow(Engine* _engine, RDEConfig* _config);

        private:
			RDE_PLATFORM_TYPE_ currentPlatform;
    };

}


#endif //RDE_PLATFORM_H
