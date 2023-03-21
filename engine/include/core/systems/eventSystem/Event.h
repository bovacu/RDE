#pragma once

#ifndef RDE_EVENT_H
#define RDE_EVENT_H

#include "core/util/Delegate.h"
#include "core/Enums.h"
#include <string>
#include <sstream>

namespace RDE {

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
			RDE_FUNC_ND static RDE_EVENT_TYPE_ getStaticType() { return RDE_EVENT_TYPE_NONE; }

            /**
             * @brief Returns the event type from the current object.
             * @return EventType
             */
			RDE_FUNC_ND virtual RDE_EVENT_TYPE_ getEventType() const = 0;

            /**
             * @brief Returns the event name.
             * @return const char*
             */
			RDE_FUNC_ND virtual const char* getName() const = 0;

            /**
             * @brief Returns the categories of an event condensed on a single int.
             * @return int
             */
			RDE_FUNC_ND virtual int getCategoryFlags() const = 0;

            /**
             * @brief Returns all the event in form of string.
             * @return std::string
             */
			RDE_FUNC_ND virtual std::string toString() const { return "Not defined in specific class"; };

            /**
             * @brief Checks if an event fits in a specific category.
             * @param _category Category to check
             * @return bool
             */
			RDE_FUNC_ND bool isInCategory(RDE_EVENT_CATEGORY_ _category) const { return (unsigned)getCategoryFlags() & _category; }
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
		RDE_FUNC_EXPLICIT EventDispatcher(Event& _event) : event(_event) {}

        /**
         * @brief This method is used to handle internally the event (apart from the handling in the EventFunction callback).
         * @param _func The callback associated to the specific event.
         * @return bool
         */
        template<typename T>
		RDE_FUNC bool dispatchEvent(const UniqueDelegate<bool(T&)>& _delegate) {
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
		RDE_FUNC bool dispatchEvent() {
            if(event.handled) return false;
            return event.getEventType() == T::getStaticType();
        }
    };


}

#endif //RDE_EVENT_H
