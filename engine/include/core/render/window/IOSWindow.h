//
// Created by borja on 8/20/22.
//

#ifndef GDE_IOS_WINDOW_H
#define GDE_IOS_WINDOW_H

#include "Window.h"

namespace GDE {

    class IOSWindow : public Window {
        public:
            explicit IOSWindow(GDEConfig* _config);
    };


}
#endif //GDE_IOS_WINDOW_H