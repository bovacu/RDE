#pragma once

#ifndef RDE_EVENT_H
#define RDE_EVENT_H

#include "core/util/Delegate.h"
#include <string>
#include <sstream>

namespace RDE {

    /**
     * @brief All the different types of events that the engine can manage.
     */
    enum RDE_EVENT_TYPE_ : unsigned {
		RDE_EVENT_TYPE_NONE, RDE_EVENT_TYPE_WINDOW_CLOSED, RDE_EVENT_TYPE_WINDOW_RESIZED, RDE_EVENT_TYPE_WINDOW_MOVED, RDE_EVENT_TYPE_WINDOW_FOCUSED, RDE_EVENT_TYPE_WINDOW_LOST_FOCUS, 
		
		RDE_EVENT_TYPE_GAME_FRAME, RDE_EVENT_TYPE_GAME_UPDATE, RDE_EVENT_TYPE_GAME_RENDER,
		
		RDE_EVENT_TYPE_KEY_PRESSED, RDE_EVENT_TYPE_KEY_DOWN, RDE_EVENT_TYPE_KEY_RELEASED, RDE_EVENT_TYPE_KEY_TYPED, RDE_EVENT_TYPE_TEXT_TYPED, 
		
		RDE_EVENT_TYPE_MOUSE_BUTTON_PRESSED, RDE_EVENT_TYPE_MOUSE_BUTTON_DOWN, RDE_EVENT_TYPE_MOUSE_BUTTON_RELEASED, RDE_EVENT_TYPE_MOUSE_SCROLLED, RDE_EVENT_TYPE_MOUSE_MOVED,
		
		RDE_EVENT_TYPE_WINDOW_MINIMIZED, RDE_EVENT_TYPE_WINDOW_DISPLAY_CHANGED,

		RDE_EVENT_TYPE_CONTROLLER_AXIS_MOVED, RDE_EVENT_TYPE_CONTROLLER_BUTTON_DOWN, RDE_EVENT_TYPE_CONTROLLER_BUTTON_UP,

		RDE_EVENT_TYPE_MOBILE_TOUCH_DOWN, RDE_EVENT_TYPE_MOBILE_TOUCH_UP, RDE_EVENT_TYPE_MOBILE_TOUCH_MOVED
    };

    /**
     * @brief Masking the different categories of the events. This makes really easy to get which event type is each event.
     * Getting it is as easy as making (EventCategoryXXX & customEventCategoryPassed) == EventCategoryXXX.
     *
     * This is, for example, customEventCategoryPassed = 00000110, that means, an event which is both EventCategoryInput
     * and EventCategoryKeyboard. doing 00000110 & 00000100 and later 00000110 & 00000010 gives as that the current
     * is both types.
     */
    enum RDE_EVENT_CATEGORY_ : unsigned {
		RDE_EVENT_CATEGORY_NONE,
		RDE_EVENT_CATEGORY_GAME           		= 1u << 0u, /// 00000001
		RDE_EVENT_CATEGORY_INPUT          		= 1u << 1u, /// 00000010
		RDE_EVENT_CATEGORY_KEYBOARD       		= 1u << 2u, /// 00000100
		RDE_EVENT_CATEGORY_MOUSE          		= 1u << 3u, /// 00001000
		RDE_EVENT_CATEGORY_MOUSE_BUTTON    		= 1u << 4u, /// 00010000
		RDE_EVENT_CATEGORY_CONTROLLER_BUTTON    = 1u << 5u,  /// 00100000
		RDE_EVENT_CATEGORY_MOBILE_INPUT    		= 1u << 6u  /// 00100000
    };

    /**
     * @brief This class is the base for any event and contains the information and methods necessary to capture and control
     * the events.
     */
    class Event {
        public:
            /**
             * @brief If the event has been handled or not. If not, it will be propagated to deeper Nodes that want to
             * listen and handle this event.
             */
            bool handled = false;

        public:
            /**
             * @brief Returns the type of an event in a static way.
             * @return EventType
             */
			static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_NONE; }

            /**
             * @brief Returns the event type from the current object.
             * @return EventType
             */
			[[nodiscard]] virtual RDE_EVENT_TYPE_ getEventType() const = 0;

            /**
             * @brief Returns the event name.
             * @return const char*
             */
            [[nodiscard]] virtual const char* getName() const = 0;

            /**
             * @brief Returns the categories of an event condensed on a single int.
             * @return int
             */
            [[nodiscard]] virtual int getCategoryFlags() const = 0;

            /**
             * @brief Returns all the event in form of string.
             * @return std::string
             */
            [[nodiscard]] virtual std::string toString() const { return "Not defined in specific class"; };

            /**
             * @brief Checks if an event fits in a specific category.
             * @param _category Category to check
             * @return bool
             */
			[[nodiscard]] inline bool isInCategory(RDE_EVENT_CATEGORY_ _category) const { return (unsigned)getCategoryFlags() & _category; }
    };

    class EventDispatcher {

        /** This method is key. It defines a type of method that takes an Event& (although it is not checked to improve
         * performance) as a parameter and returns a bool. This methods will be the ones that will execute as a
         * callback when an event happens.
         */
        template<typename T>
        using EventFn = std::function<bool(T&)>;

    private:
        /**
         * @brief Event to be handled.
         */
        Event& event;

    public:
        explicit EventDispatcher(Event& _event) : event(_event) {}

        /**
         * @brief This method is used to handle internally the event (apart from the handling in the EventFunction callback).
         * @param _func The callback associated to the specific event.
         * @return bool
         */
        template<typename T>
        bool dispatchEvent(const UniqueDelegate<bool(T&)>& _delegate) {
            if(event.handled) return false;
            /// This is why we needed the static version of getType.
            if (event.getEventType() == T::getStaticType()) {
                /// Using *(T*)& was the only way I could find to make this work, fucking pointers.
                /// Update: it can be done via static_cast<T&>.
                event.handled = _delegate(reinterpret_cast<T&>(event));
                return true;
            }

            return false;
        }

        /**
         * @brief This method is used to handle internally the event.
         * @return bool
         */
        template<typename T>
        bool dispatchEvent() {
            if(event.handled) return false;
            return event.getEventType() == T::getStaticType();
        }
    };


}

#endif //RDE_EVENT_H
