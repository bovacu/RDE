// Created by borja on 6/2/22.


#ifndef ENGINE2_0_CONTROLLER_INPUT_H
#define ENGINE2_0_CONTROLLER_INPUT_H


#include "core/util/Util.h"
#include "core/render/window/keysAndButtons/GamePadButtons.h"
#include "core/render/window/Window.h"
#include "core/render/window/input/Input.h"

namespace engine {

    struct Controller {

        explicit Controller(int _controllerID);

        int ID;
        std::unordered_map<GamePadButtons, int>  pressedGamepadButtons;
        std::unordered_map<GamePadAxis, int>  pressedGamepadAxis;
        Vec2F leftJoystickValue, rightJoystickValue, backButtonsValue;
        SDL_GameController* sdlGameController = nullptr;
    };

    class ControllerInput : public Input {
        private:
            std::unordered_map<int, Controller*> controllers;
            int controllerCounter = 0;

        public:
            void init(Window* _window);
            int getButtonState(int _keyOrButton, int _controllerID = 0);
            void setButtonState(int _keyOrButton, int _state, int _controllerID = 0);
            int getAxisState(int _keyOrButton, int _controllerID = 0);
            void setAxisState(int _keyOrButton, int _state, int _controllerID = 0);
            void initGamepads();
            Vec2F getAxisValue(const GamePadAxis& _axis, int _controllerID = 0);
            bool reassignController(int _controllerID, int _as);
            bool hasController(int _id);
            int playerIndexToInnerControllerID(int _playerIndex);
            void destroy();

        private:
            void onGamepadsMoved(SDL_Event& _event);
            void onGamepadsButtonDown(SDL_Event& _event);
            void onGamepadsButtonUp(SDL_Event& _event);
            void onGamepadConnected(SDL_Event& _event);
            void onGamepadDisconnected(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_CONTROLLER_INPUT_H
