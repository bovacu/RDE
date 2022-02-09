// Created by borja on 30/1/22.


#ifndef ENGINE2_0_JOYSTICK_EVENT_H
#define ENGINE2_0_JOYSTICK_EVENT_H

#include "Event.h"
#include "engine/include/core/render/window/keysAndButtons/KeyboardKeys.h"
#include "engine/include/core/render/window/keysAndButtons/MouseKeys.h"
#include "engine/include/core/render/window/keysAndButtons/GamePadKeys.h"
#include "engine/include/core/util/Vec.h"

namespace engine {

    class JoystickAxisMovedEvent : public Event {
        private:
            Vec2F leftJoy, rightJoy, backButtons;

        public:
            JoystickAxisMovedEvent(const Vec2F& _leftJoy, const Vec2F& _rightJoy, const Vec2F& _backButtons) : leftJoy(_leftJoy), rightJoy(_rightJoy),
                                                                                                          backButtons(_backButtons) {  }

            [[nodiscard]] inline Vec2F getLeftJoy() const { return leftJoy; }
            [[nodiscard]] inline Vec2F getRightJoy() const { return rightJoy; }
            [[nodiscard]] inline Vec2F getBackButtons() const { return backButtons; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": LJ (x = " << leftJoy.x << ", y = " << leftJoy.y << ") -- " << ": RJ (x = " << rightJoy.x << ", y = " << rightJoy.y << ") -- " << ": BB (x = " << backButtons.x << ", y = " << backButtons.y << ")";
                return _sst.str();
            }

            static EventType getStaticType() { return EventType::ControllerAxisMoved; }
            [[nodiscard]] EventType getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "JoystickAxisMoved"; }
            [[nodiscard]] int getCategoryFlags() const override { return EventCategoryInput | EventCategoryControllerButton; }
    };



    class JoystickButtonDownEvent : public Event {
        private:
            GamePadKeys button;

        public:
            explicit JoystickButtonDownEvent(GamePadKeys _button) : button(_button) {  }

            [[nodiscard]] inline GamePadKeys getButton() const { return button; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Button " + std::string(GamePadKeysStr[(int)button]);
                return _sst.str();
            }

            static EventType getStaticType() { return EventType::ControllerButtonDown; }
            [[nodiscard]] EventType getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "JoystickButtonDown"; }
            [[nodiscard]] int getCategoryFlags() const override { return EventCategoryInput | EventCategoryControllerButton; }
    };

    class JoystickButtonUpEvent : public Event {
        private:
        GamePadKeys button;

        public:
            explicit JoystickButtonUpEvent(GamePadKeys _button) : button(_button) {  }

            [[nodiscard]] inline GamePadKeys getButton() const { return button; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Button " + std::string(GamePadKeysStr[(int)button]);
                return _sst.str();
            }

            static EventType getStaticType() { return EventType::ControllerButtonUp; }
            [[nodiscard]] EventType getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "JoystickButtonUp"; }
            [[nodiscard]] int getCategoryFlags() const override { return EventCategoryInput | EventCategoryControllerButton; }
    };

}

#endif //ENGINE2_0_JOYSTICK_EVENT_H
