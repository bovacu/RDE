//
// Created by borja on 8/20/22.
//

#ifndef GDE_ANDROID_WINDOW_H
#define GDE_ANDROID_WINDOW_H

#include "Window.h"

namespace GDE {

    class AndroidWindow : public Window {
        public:
            explicit AndroidWindow(GDEConfig* _config);
    };


}
#endif //GDE_ANDROID_WINDOW_H
