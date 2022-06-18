// Created by borja on 6/2/22.


#ifndef ENGINE2_0_CONTROLLER_INPUT_H
#define ENGINE2_0_CONTROLLER_INPUT_H


#include "core/util/Util.h"
#include "core/systems/inputSystem/keysAndButtons/GamePadButtons.h"
#include "Input.h"

namespace GDE {

    /**
     * @brief Data struct that configures how the controller should vibrate.
     */
    struct VibrationConfig {
        /**
         * @brief Seconds that the vibration lasts.
         */
        int durationInSeconds;

        /**
         * @brief Time to get to maximum intensity.
         */
        int delayToGetToMaxForce;

        /**
         * @brief Time to get to minimum intensity.
         */
        int delayToGetToMinForce;

        /**
         * @brief Intensity of vibration.
         */
        int force; // 0-32767

        /**
         * @brief Period of the vibration.
         */
        int period;
    };

    /**
     * @brief Data struct that joins an specific vibration effect to many controllers.
     */
    struct Effect_Controllers {
        /**
         * @brief Vibration effect.
         */
        SDL_HapticEffect vibrationEffect;

        /**
         * @brief Vibration effect ID.
         */
        int vibrationEffectID = -1;

        /**
         * @brief Controllers binded to it.
         */
        std::vector<int> controllerIDs = {  };
    };

    class ControllerInput; class Controller;
    /**
     * @brief This class controls which controller is bind to which effects and how they play.
     */
    class ControllerVibrationManager {
        friend class ControllerInput;
        private:
            /**
             * @brief Map EffectName -> SDL_VibrationEffect.
             */
            std::unordered_map<std::string, Effect_Controllers> effects;

            /**
             * @brief Map ControllerID -> Controller*.
             */
            std::unordered_map<int, Controller*>* controllers = nullptr;

            /**
             * @brief Initiates the controller vibration system.
             * @param _controllers Controllers list.
             */
            void init(std::unordered_map<int, Controller*>* _controllers);

            /**
             * @brief Destroys the controller vibration system.
             */
            void destroy();

        public:
            ControllerVibrationManager() = default;

            /**
             * @brief Binds an specific vibration effect to a controller.
             * @param _effectName Effect name
             * @param _controllerID Controller ID
             */
            void assignVibrationEffectToController(const std::string& _effectName, int _controllerID);

            /**
             * @brief Adds a new vibration effect given a config.
             * @param _effectName Vibration effect name
             * @param _config Vibration effect configuration
             */
            void addVibrationEffect(const std::string& _effectName, const VibrationConfig& _config);

            /**
             * @brief Deletes a vibration effect from the system.
             * @param _effectName Vibration effect name to remove
             */
            void removeVibrationEffect(const std::string& _effectName);
    };

    struct Controller {

        public:
            explicit Controller(int _controllerID);

            /**
             * @brief ID of the controller in the system.
             */
            int ID;

            /**
             * @brief Map ControllerButton -> State.
             */
            std::unordered_map<GamePadButtons, int>  pressedGamepadButtons;

            /**
             * @brief Map ControllerAxis -> State.
             */
            std::unordered_map<GamePadAxis, int>  pressedGamepadAxis;

            /**
             * @brief Left joystick value
             */
            Vec2F leftJoystickValue;

            /**
             * @brief Right joystick value
             */
            Vec2F rightJoystickValue;

            /**
             * @brief Back pedals values, X = left, Y = right.
             */
            Vec2F backButtonsValue;

            /**
             * @brief Inner SDL controller.
             */
            SDL_GameController* sdlGameController = nullptr;

            /**
             * @brief SDL Vibration effect.
             */
            SDL_Haptic* vibration = nullptr;
    };

    class ControllerInput : public Input {
        private:
            /**
             * @brief Map ControllerID -> Controller*
             */
            std::unordered_map<int, Controller*> controllers;

            /**
             * @brief Controller counter, for debugging purposes.
             */
            int controllerCounter = 0;

        public:
            /**
             * @brief Initiates a new Controller
             * @param _engine Engine
             * @param _window Window
             */
            void init(Engine* _engine, Window* _window);

            /**
             * @brief Returns the state of a button.
             * @param _keyOrButton Controller button
             * @param _controllerID Controller ID
             * @return int
             */
            int getButtonState(int _keyOrButton, int _controllerID = 0);

            /**
             * @brief Sets the state of a button.
             * @param _keyOrButton Controller button.
             * @param _state State
             * @param _controllerID Controller ID
             */
            void setButtonState(int _keyOrButton, int _state, int _controllerID = 0);

            /**
             * @brief Returns the state of an axis.
             * @param _keyOrButton Controller axis
             * @param _controllerID Controller ID
             * @return int
             */
            int getAxisState(int _keyOrButton, int _controllerID = 0);

            /**
             * @brief Sets the state of an axis.
             * @param _keyOrButton Controller axis.
             * @param _state State
             * @param _controllerID Controller ID
             */
            void setAxisState(int _keyOrButton, int _state, int _controllerID = 0);

            /**
             * @brief Makes the controller vibrate.
             * @param _vibrateEffect Vibration effect name
             * @param _controllerID Controller ID
             */
            void vibrate(const std::string& _vibrateEffect, int _controllerID = 0);

            /**
             * @brief Initiates the connected controllers.
             */
            void initControllers();

            /**
             * @brief Returns the value of an axis.
             * @param _axis Controller axis
             * @param _controllerID Controller ID
             * @return VecF
             */
            Vec2F getAxisValue(const GamePadAxis& _axis, int _controllerID = 0);

            /**
             * @brief Reassigns a controller to another one. (Change order)
             * @param _controllerID Controller ID
             * @param _as New Controller value
             * @return bool
             */
            bool reassignController(int _controllerID, int _as);

            /**
             * @brief Returns if a controller is connected.
             * @param _id Controller ID
             * @return bool
             */
            bool hasController(int _id);

            /**
             * @brief Internal conversion method
             * @param _playerIndex Player index
             * @return int
             */
            int playerIndexToInnerControllerID(int _playerIndex);

            /**
             * @brief Destroys and frees all allocated controllers.
             */
            void destroy();

        private:
            /**
             * @brief Handles controller axis moved.
             * @param _event Event
             */
            void onControllerMoved(SDL_Event& _event);

            /**
             * @brief Handles controller button down.
             * @param _event Event
             */
            void onControllerButtonDown(SDL_Event& _event);

            /**
             * @brief Handles controller button up.
             * @param _event Event
             */
            void onControllerButtonUp(SDL_Event& _event);

            /**
             * @brief Handles if a new controller is connected.
             * @param _event Event
             */
            void onControllerConnected(SDL_Event& _event);

            /**
             * @brief Handles if a controller is disconnected.
             * @param _event Event
             */
            void onControllerDisconnected(SDL_Event& _event);
    };

}


#endif //ENGINE2_0_CONTROLLER_INPUT_H
