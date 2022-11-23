#pragma once

#ifndef RDE_KEY_EVENT_H
#define RDE_KEY_EVENT_H

#include "Event.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"


namespace RDE {

    /// Base class for all the key events
    class KeyEvent          : public Event {
        protected:
            KeyCode keyCode;
            char normalChar;
            char modifiedChar;

        protected:
            explicit KeyEvent(KeyCode _keyCode, char _normalChar, char _alternativeChar) : keyCode(_keyCode), normalChar(_normalChar), modifiedChar(_alternativeChar) {}

        public:
            /// Returns the code of the key that has been pressed, typed or released.
            /// @return The code of the key that made the event happened.
            [[nodiscard]] inline KeyCode getKeyCode() const { return keyCode; }

            /// Returns the char of the key that has been pressed, typed or released.
            /// @return The char of the key that made the event happened.
            [[nodiscard]] inline char getChar() const { return normalChar; }

            /// Returns the alternative char of the key that has been pressed, typed or released.
            /// @return The alternative char of the key that made the event happened.
            [[nodiscard]] inline char getAlternateChar() const { return modifiedChar; }

            /// Returns the flag with all the categories that the event fills in.
            /// @return The flag with the categories.
            [[nodiscard]] int getCategoryFlags() const override { return EventCategory::EventCategoryKeyboard | EventCategory::EventCategoryInput; }
    };




    /// This class represents all of the events that involves pressing a key.
    class KeyPressedEvent   : public KeyEvent {
        private:
            int repeatedTimes;

        public:
            KeyPressedEvent(KeyCode _keyCode, char _normalChar, char _alternativeChar, int _repeatedTimes) : KeyEvent(_keyCode, _normalChar, _alternativeChar), repeatedTimes(_repeatedTimes) {}

            /// Returns the number of times in a sequence that the key has been pressed.
            /// @return The number of pressing times.
            [[nodiscard]] inline int getRepeatedTimes() const { return repeatedTimes; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Key = " << (char)keyCode << " (" << keyCode << ") (down " << repeatedTimes << " times)";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType() { return EventType::KeyPressed; }
            [[nodiscard]] EventType getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "KeyPressed"; }
    };




    /// This class represents all of the events that involves typing a key.
    class KeyTypedEvent     : public KeyEvent {
        private:
            char typedChar;

        public:
            explicit KeyTypedEvent(KeyCode _keyCode, char _normalChar, char _modifiedChar) : KeyEvent(_keyCode, _normalChar, _modifiedChar), typedChar((char)keyCode) {  }

            /// Returns the number of times in a sequence that the key has been pressed.
            /// @return The char typed.
            [[nodiscard]] inline char getCharTyped() const { return (char)typedChar; }

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Key = " << (char)keyCode;
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType() { return EventType::KeyTyped; }
            [[nodiscard]] EventType getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "KeyTyped"; }
    };




    /// This class represents all of the events that involves releasing a key.
    class KeyReleasedEvent  : public KeyEvent {
        public:
            explicit KeyReleasedEvent(KeyCode _keyCode, char _normalChar, char _modifiedChar) : KeyEvent(_keyCode, _normalChar, _modifiedChar) {}

            [[nodiscard]] std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Key = " << (char)keyCode << " (" << keyCode << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
            static EventType getStaticType()  { return EventType::KeyReleased; }
            [[nodiscard]] EventType getEventType() const override { return getStaticType(); }
            [[nodiscard]] const char* getName() const override { return "KeyReleased"; }
    };

}

#endif //RDE_KEY_EVENT_H
