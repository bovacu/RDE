// Created by borja on 6/2/22.


#ifndef ENGINE2_0_CONTROLLER_INPUT_H
#define ENGINE2_0_CONTROLLER_INPUT_H


#include "core/util/Util.h"
#include "core/render/window/keysAndButtons/GamePadKeys.h"
#include "core/render/window/Window.h"

namespace engine {

    class ControllerInput {
        private:
            std::unordered_map<GamePadKeys, int>  pressedGamepadKeys;
            std::vector<SDL_Joystick*> joysticks;
            Vec2F leftJoystickValue, rightJoystickValue, backButtonsValue;
            Window* window = nullptr;
            std::unordered_map<int, std::function<void(SDL_Event&)>> events;
            std::vector<SDL_EventType> ignoreEvents;

        public:
            void init(Window* _window);
            bool pollEvent(SDL_Event& _event);
            void initGamepads();
            void onGamepadsMoved(SDL_Event& _event);
            void onGamepadsButtonDown(SDL_Event& _event);
            void onGamepadsButtonUp(SDL_Event& _event);
            void onGamepadConnected(SDL_Event& _event);
            void onGamepadDisconnected(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_CONTROLLER_INPUT_H
