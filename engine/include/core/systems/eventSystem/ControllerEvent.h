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
			RDE_FUNC ControllerAxisMovedEvent(const Vec2F& _leftJoy, const Vec2F& _rightJoy, const Vec2F& _backButtons) : leftJoy(_leftJoy), rightJoy(_rightJoy),
                                                                                                                 backButtons(_backButtons) {  }
            /**
             * @brief Returns how much the left joystick of the controller was moved, normalized.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getLeftJoy() const { return leftJoy; }

            /**
             * @brief Returns how much the right joystick of the controller was moved, normalized.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getRightJoy() const { return rightJoy; }

            /**
             * @brief Returns how much the back buttons of the controller were moved, normalized. X = left, Y = right.
             * @return Vec2F
             */
			RDE_FUNC_ND Vec2F getBackButtons() const { return backButtons; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": LJ (x = " << leftJoy.x << ", y = " << leftJoy.y << ") -- " << ": RJ (x = " << rightJoy.x << ", y = " << rightJoy.y << ") -- " << ": BB (x = " << backButtons.x << ", y = " << backButtons.y << ")";
                return _sst.str();
            }

			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_CONTROLLER_AXIS_MOVED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "JoystickAxisMoved"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_CONTROLLER_BUTTON; }
    };



    class ControllerButtonDownEvent : public Event {
        private:
            RDE_CONTROLLER_BUTTON_ button;

        public:
			RDE_FUNC_EXPLICIT ControllerButtonDownEvent(RDE_CONTROLLER_BUTTON_ _button) : button(_button) {  }

            /**
             * @brief Returns the pressed button.
             * @return GamePadButtons
             */
			RDE_FUNC_ND RDE_CONTROLLER_BUTTON_ getButton() const { return button; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Button " + std::string(ControllerKeysStr[(int)button]);
                return _sst.str();
            }

			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_CONTROLLER_BUTTON_DOWN; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "JoystickButtonDown"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_CONTROLLER_BUTTON; }
    };

    class ControllerButtonUpEvent : public Event {
        private:
		RDE_CONTROLLER_BUTTON_ button;

        public:
			RDE_FUNC_EXPLICIT ControllerButtonUpEvent(RDE_CONTROLLER_BUTTON_ _button) : button(_button) {  }

            /**
             * @brief Returns the freed button.
             * @return GamePadButtons
             */
			RDE_FUNC_ND RDE_CONTROLLER_BUTTON_ getButton() const { return button; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Button " + std::string(ControllerKeysStr[(int)button]);
                return _sst.str();
            }

			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_CONTROLLER_BUTTON_UP; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "JoystickButtonUp"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_CONTROLLER_BUTTON; }
    };

}

#endif //RDE_JOYSTICK_EVENT_H
