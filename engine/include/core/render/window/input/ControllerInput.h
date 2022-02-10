// Created by borja on 6/2/22.


#ifndef ENGINE2_0_CONTROLLER_INPUT_H
#define ENGINE2_0_CONTROLLER_INPUT_H


#include "core/util/Util.h"
#include "core/render/window/keysAndButtons/GamePadKeys.h"
#include "core/render/window/Window.h"
#include "core/render/window/input/Input.h"

namespace engine {

    class ControllerInput : public Input {
        private:
            std::unordered_map<GamePadKeys, int>  pressedGamepadKeys;
            std::vector<SDL_Joystick*> joysticks;
            Vec2F leftJoystickValue, rightJoystickValue, backButtonsValue;

        public:
            void init(Window* _window);
            int getState(int _keyOrButton);
            void setState(int _keyOrButton, int _state);
            void initGamepads();
            Vec2F getAxisValue(const GamePadAxis& _axis);

        private:
            void onGamepadsMoved(SDL_Event& _event);
            void onGamepadsButtonDown(SDL_Event& _event);
            void onGamepadsButtonUp(SDL_Event& _event);
            void onGamepadConnected(SDL_Event& _event);
            void onGamepadDisconnected(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_CONTROLLER_INPUT_H
