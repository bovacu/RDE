// Created by borja on 30/1/22.


#ifndef RDE_JOYSTICK_EVENT_H
#define RDE_JOYSTICK_EVENT_H

#include "Event.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"
#include "core/systems/inputSystem/keysAndButtons/ControllerButtons.h"
#include "core/util/Vec.h"

namespace RDE {

    class ControllerAxisMovedEvent : public Event {
        private:
            Vec2F leftJoy, rightJoy, backButtons;

        public:
            ControllerAxisMovedEvent(const Vec2F& _leftJoy, const Vec2F& _rightJoy, const Vec2F& _backButtons) : leftJoy(_leftJoy), rightJoy(_rightJoy),
                                                                                                                 backButtons(_backButtons) {  }
            /**
             * @brief Returns how much the left joystick of the controller was moved, normalized.
             * @return Vec2F
             */
            [[nodiscard]] inline Vec2F getLeftJoy() const { return leftJoy; }

            /**
             * @brief Returns how much the right joystick of the controller was moved, normalized.
             * @return Vec2F
             */
            [[nodiscard]] inline Vec2F getRightJoy() const { return rightJoy; }

            /**
             * @brief Returns how much the back buttons of the controller were moved, normalized. X = left, Y = right.
             * @return Vec2F
             */
            [[nodiscard]] inline Vec2F getBackButtons() const { return backButtons; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": LJ (x = " << leftJoy.x << ", y = " << leftJoy.y << ") -- " << ": RJ (x = " << rightJoy.x << ", y = " << rightJoy.y << ") -- " << ": BB (x = " << backButtons.x << ", y = " << backButtons.y << ")";
                return _sst.str();
            }

			static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_CONTROLLER_AXIS_MOVED; }
			[[nodiscard]] RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "JoystickAxisMoved"; }
			[[nodiscard]] int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_CONTROLLER_BUTTON; }
    };



    class ControllerButtonDownEvent : public Event {
        private:
            RDE_CONTROLLER_BUTTON_ button;

        public:
			explicit ControllerButtonDownEvent(RDE_CONTROLLER_BUTTON_ _button) : button(_button) {  }

            /**
             * @brief Returns the pressed button.
             * @return GamePadButtons
             */
			[[nodiscard]] inline RDE_CONTROLLER_BUTTON_ getButton() const { return button; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Button " + std::string(ControllerKeysStr[(int)button]);
                return _sst.str();
            }

			static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_CONTROLLER_BUTTON_DOWN; }
			[[nodiscard]] RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "JoystickButtonDown"; }
			[[nodiscard]] int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_CONTROLLER_BUTTON; }
    };

    class ControllerButtonUpEvent : public Event {
        private:
		RDE_CONTROLLER_BUTTON_ button;

        public:
			explicit ControllerButtonUpEvent(RDE_CONTROLLER_BUTTON_ _button) : button(_button) {  }

            /**
             * @brief Returns the freed button.
             * @return GamePadButtons
             */
			[[nodiscard]] inline RDE_CONTROLLER_BUTTON_ getButton() const { return button; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Button " + std::string(ControllerKeysStr[(int)button]);
                return _sst.str();
            }

			static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_CONTROLLER_BUTTON_UP; }
			[[nodiscard]] RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "JoystickButtonUp"; }
			[[nodiscard]] int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_CONTROLLER_BUTTON; }
    };

}

#endif //RDE_JOYSTICK_EVENT_H
