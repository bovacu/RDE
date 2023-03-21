#pragma once

#ifndef RDE_MOUSE_EVENT_H
#define RDE_MOUSE_EVENT_H

#include "Event.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"
#include "core/systems/inputSystem/keysAndButtons/MouseKeys.h"

namespace RDE {

    /// This class represents all of the events that involves moving the mouse.
    class MouseMovedEvent           : public Event {
        private:
            float xPos, yPos;

        public:
			RDE_FUNC_EXPLICIT MouseMovedEvent(float _xPos, float _yPos) : xPos(_xPos), yPos(_yPos) {  }

            /// Returns the x position of the mouse.
            /// @return x position of the mouse.
			RDE_FUNC_ND float getX() const { return xPos; }

            /// Returns the y position of the mouse.
            /// @return y position of the mouse.
			RDE_FUNC_ND float getY() const { return yPos; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": moved (x = " << xPos << ", y = " << yPos << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_MOUSE_MOVED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "MouseMoved"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_MOUSE; }
    };




    /// This class represents all of the events that involves scrolling with the mouse.
    class MouseScrolledEvent        : public Event {
        private:
            float xScrolled, yScrolled;

        public:
			RDE_FUNC_EXPLICIT MouseScrolledEvent(float _xScrolled, float _yScrolled) : xScrolled(_xScrolled), yScrolled(_yScrolled) {  }

            /// Returns the scrolled value on x.
            /// @return x scrolling value.
			RDE_FUNC_ND float getScrollX() const { return xScrolled; }

            /// Returns the scrolled value on y.
            /// @return y scrolling value.
			RDE_FUNC_ND float getScrollY() const { return yScrolled; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": scrolled (x = " << xScrolled << ", y = " << yScrolled << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_MOUSE_SCROLLED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "MouseScrolled"; }
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_MOUSE; }
    };




    /// This class represents all of the events that involves doing stuff with the mouse buttons.
    class MouseButtonEvent          : public Event {
        protected:
            RDE_MOUSE_BUTTON_ mouseButton;

        protected:
			RDE_FUNC_EXPLICIT MouseButtonEvent(RDE_MOUSE_BUTTON_ _mouseButton) : mouseButton(_mouseButton) {  }

        public:
            /// Returns the button that made the event happen.
            /// @return The code of the button that triggered the event.
			RDE_FUNC_ND inline RDE_MOUSE_BUTTON_ getMouseButton() const { return mouseButton; }
			RDE_FUNC_ND int getCategoryFlags() const override  { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_MOUSE_BUTTON; }
    };




    /// This class represents all of the events that involves pressing the mouse buttons.
    class MouseButtonPressedEvent   : public MouseButtonEvent {
        private:
            int repeatedTimes = 1;

        public:
			RDE_FUNC_EXPLICIT MouseButtonPressedEvent(RDE_MOUSE_BUTTON_ _mouseButton) : MouseButtonEvent(_mouseButton) {  }
			RDE_FUNC_ND int getRepeatedTimes() const { return repeatedTimes; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": button = " << mouseButton << " (repeated " << repeatedTimes << " times)";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "MouseButtonPressed"; }
    };




    /// This class represents all of the events that involves releasing the mouse buttons.
    class MouseButtonReleasedEvent  : public MouseButtonEvent {
        public:
			RDE_FUNC_EXPLICIT MouseButtonReleasedEvent(RDE_MOUSE_BUTTON_ _mouseButton) : MouseButtonEvent(_mouseButton) {  }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": button = " << mouseButton;
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "MouseButtonReleased"; }
    };

}

#endif //RDE_MOUSE_EVENT_H
