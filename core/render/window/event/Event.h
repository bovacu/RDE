#pragma once

#ifndef ENGINE2_0_EVENT_H
#define ENGINE2_0_EVENT_H

#include <sstream>
#include <functional>

namespace engine {

    /// All the different types of events that the engine can manage.
    enum class EventType : unsigned {
        None, WindowClosed, WindowResized, WindowMoved, WindowFocused, WindowLostFocus, GameFrame, GameUpdate,
        GameRender, KeyPressed, KeyDown, KeyReleased, KeyTyped, MouseButtonPressed, MouseButtonDown, MouseButtonReleased,
        MouseScrolled, MouseMoved, WindowMinimized
    };

    /// Masking the different categories of the events. This makes really easy to get which event type is each event.
    /// Getting it is as easy as making (EventCategoryXXX & customEventCategoryPassed) == EventCategoryXXX.
    ///
    /// This is, for example, customEventCategoryPassed = 00000110, that means, an event which is both EventCategoryInput
    /// and EventCategoryKeyboard. doing 00000110 & 00000100 and later 00000110 & 00000010 gives as that the current
    /// is both types.
    enum EventCategory : unsigned {
        None,
        EventCategoryGame           = 1u << 0u, /// 00000001
        EventCategoryInput          = 1u << 1u, /// 00000010
        EventCategoryKeyboard       = 1u << 2u, /// 00000100
        EventCategoryMouse          = 1u << 3u, /// 00001000
        EventCategoryMouseButton    = 1u << 4u  /// 00010000
    };

    /// This class is the base for any event and contains the information and methods necessary to capture and control
    /// the events.
    class Event {
        public:
            bool handled = false;

        public:
            /// Returns the type of an event in a static way.
            /// @return The EventType of the event.
            static EventType getStaticType() { return EventType::None; }

            /// Returns the event type from the current object.
            /// @return The EventType of the event.
            [[nodiscard]] virtual EventType getEventType() const = 0;

            /// Returns the event name.
            /// @return Event name.
            [[nodiscard]] virtual const char* getName() const = 0;

            /// Returns the categories of an event condensed on a single int.
            /// @return The flags of the categories it belongs to.
            [[nodiscard]] virtual int getCategoryFlags() const = 0;

            /// Returns all the event in form of string.
            /// @return The event in string.
            [[nodiscard]] virtual std::string toString() const { return "Not defined in specific class"; };

            /// Checks if an event fits in a specific category.
            /// @param _category Category to check.
            /// @return true if belongs, false otherwise.
            [[nodiscard]] inline bool isInCategory(EventCategory _category) const { return (unsigned)this->getCategoryFlags() & _category; }
    };

    class EventDispatcher {

        /// This method is key. It defines a type of method that takes an Event& (although it is not checked to improve
        /// performance) as a parameter and returns a bool. This methods will be the ones that will execute as a
        /// callback when an event happens.
        template<typename T>
        using EventFn = std::function<bool(T&)>;

    private:
        Event& event;

    public:
        explicit EventDispatcher(Event& _event) : event(_event) {}

        /// This method is used to handle internally the event (apart from the handling in the EventFunction callback).
        /// @param _func The callback associated to the specific event.
        /// @return true if the event could me managed (not if it was handled), false otherwise.
        template<typename T>
        bool dispatchEvent(EventFn<T> _func) {
            /// This is why we needed the static version of getType.
            if (this->event.getEventType() == T::getStaticType()) {
                /// Using *(T*)& was the only way I could find to make this work, fucking pointers.
                /// Update: it can be done via static_cast<T&>.
                this->event.handled = _func(static_cast<T&>(this->event));
                return true;
            }

            return false;
        }
    };


}

#endif //ENGINE2_0_EVENT_H
