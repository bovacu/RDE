#pragma once

#ifndef RDE_KEY_EVENT_H
#define RDE_KEY_EVENT_H

#include "Event.h"
#include "core/systems/inputSystem/keysAndButtons/KeyboardKeys.h"


namespace RDE {

    /// Base class for all the key events
    class KeyEvent          : public Event {
        protected:
            RDE_KEYBOARD_KEY_ keyCode;

        protected:
			RDE_FUNC_EXPLICIT KeyEvent(RDE_KEYBOARD_KEY_ _keyCode) : keyCode(_keyCode) {}

        public:
            /// Returns the code of the key that has been pressed, typed or released.
            /// @return The code of the key that made the event happened.
			RDE_FUNC_ND RDE_KEYBOARD_KEY_ getKeyCode() const { return keyCode; }

            /// Returns the flag with all the categories that the event fills in.
            /// @return The flag with the categories.
			RDE_FUNC_ND int getCategoryFlags() const override { return RDE_EVENT_CATEGORY_INPUT | RDE_EVENT_CATEGORY_KEYBOARD; }
    };




    /// This class represents all of the events that involves pressing a key.
    class KeyPressedEvent   : public KeyEvent {
        private:
            int repeatedTimes;

        public:
			RDE_FUNC_EXPLICIT KeyPressedEvent(RDE_KEYBOARD_KEY_ _keyCode, int _repeatedTimes) : KeyEvent(_keyCode), repeatedTimes(_repeatedTimes) {}

            /// Returns the number of times in a sequence that the key has been pressed.
            /// @return The number of pressing times.
			RDE_FUNC_ND int getRepeatedTimes() const { return repeatedTimes; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Key = " << (char)keyCode << " (" << keyCode << ") (down " << repeatedTimes << " times)";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_KEY_PRESSED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "KeyPressed"; }
    };




    /// This class represents all of the events that involves typing a key.
    class KeyTypedEvent     : public KeyEvent {
        private:
            char typedChar;

        public:
			RDE_FUNC_EXPLICIT KeyTypedEvent(RDE_KEYBOARD_KEY_ _keyCode) : KeyEvent(_keyCode), typedChar((char)keyCode) {  }

            /// Returns the number of times in a sequence that the key has been pressed.
            /// @return The char typed.
			RDE_FUNC_ND char getCharTyped() const { return (char)typedChar; }

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Key = " << (char)keyCode;
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_KEY_TYPED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "KeyTyped"; }
    };




    /// This class represents all of the events that involves releasing a key.
    class KeyReleasedEvent  : public KeyEvent {
        public:
			RDE_FUNC_EXPLICIT KeyReleasedEvent(RDE_KEYBOARD_KEY_ _keyCode) : KeyEvent(_keyCode) {}

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Key = " << (char)keyCode << " (" << keyCode << ")";
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType()  { return RDE_EVENT_TYPE_KEY_RELEASED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "KeyReleased"; }
    };


    /// This class represents all of the events that involves releasing a key.
    class TextTypedEvent  : public KeyEvent {
        public:
			RDE_FUNC_EXPLICIT TextTypedEvent(char* _text) : KeyEvent(RDE_KEYBOARD_KEY_D0) { text = std::string(_text); }

            std::string text;

			RDE_FUNC_ND std::string toString() const override {
                std::stringstream _sst;
                _sst << getName() << ": Text = " << text;
                return _sst.str();
            }

            /// Implementation of the rest of the static and virtual methods.
			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType()  { return RDE_EVENT_TYPE_TEXT_TYPED; }
			RDE_FUNC_ND RDE_EVENT_TYPE_ getEventType() const override { return getStaticType(); }
			RDE_FUNC_ND const char* getName() const override { return "TextTyped"; }
    };

}

#endif //RDE_KEY_EVENT_H
