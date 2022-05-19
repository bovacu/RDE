// Created by borja on 6/2/22.


#ifndef ENGINE2_0_CONTROLLER_INPUT_H
#define ENGINE2_0_CONTROLLER_INPUT_H


#include "core/util/Util.h"
#include "core/render/window/keysAndButtons/GamePadButtons.h"
#include "core/render/window/input/Input.h"

namespace GDE {

    struct VibrationConfig {
        int durationInSeconds;
        int delayToGetToMaxForce;
        int delayToGetToMinForce;
        int force; // 0-32767
        int period;
    };

    struct Effect_Controllers {
        SDL_HapticEffect vibrationEffect;
        int vibrationEffectID = -1;
        std::vector<int> controllerIDs = {  };
    };

    class ControllerInput; class Controller;
    class VibrationManager {
        friend class ControllerInput;
        private:
            std::unordered_map<std::string, Effect_Controllers> effects;
            std::unordered_map<int, Controller*>* controllers = nullptr;
            VibrationManager() = default;
            void init(std::unordered_map<int, Controller*>* _controllers);
            void destroy();

        public:
            static VibrationManager& get();

            void assignVibrationEffectToController(const std::string& _effectName, int _controllerID);
            void addVibrationEffect(const std::string& _effectName, const VibrationConfig& _config);
            void removeVibrationEffect(const std::string& _effectName);
    };

    struct Controller {

        explicit Controller(int _controllerID);

        int ID;
        std::unordered_map<GamePadButtons, int>  pressedGamepadButtons;
        std::unordered_map<GamePadAxis, int>  pressedGamepadAxis;
        Vec2F leftJoystickValue, rightJoystickValue, backButtonsValue;
        SDL_GameController* sdlGameController = nullptr;
        SDL_Haptic* vibration = nullptr;
    };

    class ControllerInput : public Input {
        private:
            std::unordered_map<int, Controller*> controllers;
            int controllerCounter = 0;

        public:
            void init(Engine* _engine, Window* _window);
            int getButtonState(int _keyOrButton, int _controllerID = 0);
            void setButtonState(int _keyOrButton, int _state, int _controllerID = 0);
            int getAxisState(int _keyOrButton, int _controllerID = 0);
            void setAxisState(int _keyOrButton, int _state, int _controllerID = 0);
            void vibrate(const std::string& _vibrateEffect, int _controllerID = 0);
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
