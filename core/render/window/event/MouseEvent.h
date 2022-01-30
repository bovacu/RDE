#pragma once

#ifndef ENGINE2_0_MOUSE_EVENT_H
#define ENGINE2_0_MOUSE_EVENT_H

#include "core/render/window/event/Event.h"
#include "core/render/window/input/KeyboardKeys.h"
#include "core/render/window/input/MouseKeys.h"

namespace engine {

    /// This class represents all of the events that involves moving the mouse.
    class MouseMovedEvent           : public Event {
        private:
            float xPos, yPos;

        public:
            MouseMovedEvent(float _xPos, float _yPos) : xPos(_xPos), yPos(_yPos) {  }

            /// Returns the x position of the mouse.
            /// @return x position of the mouse.
            [[nodiscard]] inline float getX() const { return xPos; }

            /// Returns the y position of the mouse.
            /// @return y position of the mouse.
            [[nodiscard]] inline float getY() const { return yPos; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": moved (x = " << xPos << ", y = " << yPos << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::MouseMoved; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "MouseMoved"; }
            [[nodiscard]] int getCategoryFlags()    const                           override { return EventCategoryInput | EventCategoryMouse; }
    };




    /// This class represents all of the events that involves scrolling with the mouse.
    class MouseScrolledEvent        : public Event {
        private:
            float xScrolled, yScrolled;

        public:
            MouseScrolledEvent(float _xScrolled, float _yScrolled) : xScrolled(_xScrolled), yScrolled(_yScrolled) {  }

            /// Returns the scrolled value on x.
            /// @return x scrolling value.
            [[nodiscard]] inline float getScrollX() const { return xScrolled; }

            /// Returns the scrolled value on y.
            /// @return y scrolling value.
            [[nodiscard]] inline float getScrollY() const { return yScrolled; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": scrolled (x = " << xScrolled << ", y = " << yScrolled << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::MouseScrolled; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "MouseScrolled"; }
            [[nodiscard]] int getCategoryFlags()    const                           override { return EventCategoryInput | EventCategoryMouse; }
    };




    /// This class represents all of the events that involves doing stuff with the mouse buttons.
    class MouseButtonEvent          : public Event {
        protected:
            MouseCode mouseButton;

        protected:
            explicit MouseButtonEvent(MouseCode _mouseButton) : mouseButton(_mouseButton) {  }

        public:
            /// Returns the button that made the event happen.
            /// @return The code of the button that triggered the event.
            [[nodiscard]] inline MouseCode getMouseButton() const           { return mouseButton; }
            [[nodiscard]] int getCategoryFlags()            const override  { return EventCategoryMouseButton | EventCategoryInput; }
    };




    /// This class represents all of the events that involves pressing the mouse buttons.
    class MouseButtonPressedEvent   : public MouseButtonEvent {
        private:
            int repeatedTimes = 1;

        public:
            explicit MouseButtonPressedEvent(MouseCode _mouseButton) : MouseButtonEvent(_mouseButton) {  }
            [[nodiscard]] inline int getRepeatedTimes() const { return repeatedTimes; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": button = " << mouseButton << " (repeated " << repeatedTimes << " times)";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::MouseButtonPressed; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "MouseButtonPressed"; }
    };




    /// This class represents all of the events that involves releasing the mouse buttons.
    class MouseButtonReleasedEvent  : public MouseButtonEvent {
        public:
            explicit MouseButtonReleasedEvent(MouseCode _mouseButton) : MouseButtonEvent(_mouseButton) {  }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": button = " << mouseButton;
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()                                                 { return EventType::MouseButtonReleased; }
            [[nodiscard]] EventType getEventType()  const                           override { return getStaticType(); }
            [[nodiscard]] const char* getName()     const                           override { return "MouseButtonReleased"; }
    };

}

#endif //ENGINE2_0_MOUSE_EVENT_H
