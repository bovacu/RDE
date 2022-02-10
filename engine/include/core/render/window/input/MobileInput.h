// Created by borja on 10/2/22.


#ifndef ENGINE2_0_MOBILE_INPUT_H
#define ENGINE2_0_MOBILE_INPUT_H

#include "core/render/window/Window.h"
#include "core/render/window/input/Input.h"

namespace engine {

    typedef uint32_t TimeStamp;
    class MobileInput : public Input {
        private:
            std::unordered_map<int, int> pressedFingers;
            float pressure;
            Vec2F fingerPosInit, fingerPosEnd, fingerMoveDistance;
            TimeStamp initOfTouch, endOfTouch;

        public:
            void init(Window* _window);
            int getState(int _keyOrButton);
            void setState(int _keyOrButton, int _state);

        private:
            void onTouchDown(SDL_Event& _event);
            void onTouchUp(SDL_Event& _event);
            void onTouchMoved(SDL_Event& _event);
    };

}

#endif //ENGINE2_0_MOBILE_INPUT_H
