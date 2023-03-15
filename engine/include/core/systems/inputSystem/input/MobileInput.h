// Created by borja on 10/2/22.


#ifndef RDE_MOBILE_INPUT_H
#define RDE_MOBILE_INPUT_H

#include "core/systems/inputSystem/input/Input.h"

namespace RDE {

    typedef uint32_t TimeStamp;

    /**
     * @brief This class handles all of the mobile events.
     */
    class MobileInput : public Input {
        private:
            /**
             * @brief Map FingerID -> State.
             */
			std::unordered_map<int, RDE_INPUT_STATUS_> pressedFingers;

            /**
             * @brief Pressure used on the clicks.
             */
            float pressure;

            /**
             * @brief Initial position of a click.
             */
            Vec2F fingerPosInit;

            /**
             * @brief Final position of a click.
             */
            Vec2F fingerPosEnd;

            /**
             * @brief Distance moved by the finger.
             */
            Vec2F fingerMoveDistance;

            /**
             * @brief Moment in time of the initial click.
             */
            TimeStamp initOfTouch;

            /**
             * @brief Moment in time of the end of click.
             */
            TimeStamp endOfTouch;

        public:
            /**
             * @brief Initiates the system and sets the initial states.
             * @param _engine Engine
             * @param _window Window
             */
            void init(Engine* _engine, Window* _window);

            /**
             * @brief Gets the state of a finger.
             * @param _keyOrButton Finger
			 * @return RDE_INPUT_STATUS_
             */
			RDE_INPUT_STATUS_ getState(int _finger);

            /**
             * @brief Sets the state of a finger.
             * @param _keyOrButton Finger
			 * @param _state RDE_INPUT_STATUS_
             */
			void setState(int _finger, RDE_INPUT_STATUS_ _state);

        private:
            /**
             * @brief Handles the touch down event.
             * @param _event Event
             */
            void onTouchDown(SDL_Event& _event);

            /**
             * @brief Handles the touch up event.
             * @param _event Event
             */
            void onTouchUp(SDL_Event& _event);

            /**
             * @brief Handles the touch moved event.
             * @param _event Event
             */
            void onTouchMoved(SDL_Event& _event);
    };

}

#endif //RDE_MOBILE_INPUT_H
