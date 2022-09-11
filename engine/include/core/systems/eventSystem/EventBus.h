// Created by borja on 17/12/21.

#ifndef GDE_EVENT_BUS
#define GDE_EVENT_BUS

#include "core/util/Logger.h"

/**
 * @brief The Event bus works in the following way:
 *
 *  1 - We need to specify the type of Event that our EventBus is going to use to store each of the observable events.
 *      This Event can be anything, like a string, an int, bool, classes, structs, enums... Then we specify the
 *      AssociatedFunctionArgs which are a set of types.
 *
 *  2 - Once we have decided what type of Event we need for this EventBus, we start defining them and attaching the functions
 *      that will be called when the particular Event happens.
 *
 *  3 - Once an Event is subscribed, each time you repeat a specific Event, the function will be linked to that Event.
 *
 *  4 - The associated functions must always return bool.
 *
 *  Example:
 *      Let's imagine we have the EventBus<std::string, char> that will be observing the events that happen when a key is pressed
 *      or released.
 *
 *          EventBus<std::string, char> keyEventBus;
 *
 *      Then we define (somewhere) two functions:
 *
 *          bool MyClass::keyUp(char _char) {
 *              // Do whatever...
 *              return true;
 *          }
 *
 *          bool MyClass::keyDown(char _char) {
 *              // Do whatever...
 *              return true;
 *          }
 *
 *
 *      Now let's link events with functions:
 *
 *          keyEventBus.subscribe<&MyClass::keyDown>("keyDown", this);
 *          keyEventBus.subscribe<&MyClass::keyUp>("keyUp", this);
 *
 *          *Notes: if the associated function is a free function -> keyEventBus.subscribe<&keyDown>("keyDown", nullptr);
 *
 *
 *      Then, in any part of the code that the event needs to be dispatched, we do:
 *
 *          keyEventBus.dispatch("keyUp", _theCharPressed);
 *                          or/and
 *          keyEventBus.dispatch("keyDown", _theCharReleased);
 *
 *      And with this, all of the linked functions will know about this change and will react as the function specifies
 */

namespace GDE {

    template <typename Event, typename... AssociatedFunctionArgs>
    /**
     * @warning Deprecated
     * @tparam Event
     * @tparam AssociatedFunctionArgs
     */
    class EventBus {
//    static_assert(std::is_enum<Type>::value, "Must be an enum type");
        public:
            typedef std::function<bool(AssociatedFunctionArgs...)> HandlerFunc;

        private:
            std::map<Event, GDE::MDelegate<bool(AssociatedFunctionArgs...)>> handlers;

        public:
            class HandlerId {
                public:
                    HandlerId() : valid(false) {  }
                    friend class EventBus<AssociatedFunctionArgs...>;
                    size_t id{};
                    explicit HandlerId(size_t _id) : id(_id), valid(true) {  }
                    bool valid;
            };

            // register to be notified
            template<auto Func, typename Class>
            HandlerId subscribe(const Event& _key, Class* _class) {
                if (Func) {
                    handlers[_key].template bind<Func>(_class);
                    return HandlerId(handlers[_key].getEnd());
                }
                return HandlerId();
            }

            void unsubscribe(const Event& _type, HandlerId& _handlerId) {
                if(!hasType(_type)) {
                    LOG_W_TIME("Tried to unsubscribe Type ", typeid(_type).name(), " but it wasn't subscribed!")
                    return;
                }

                if (_handlerId.valid) {
                    handlers[_type].remove(_handlerId.id);
                } else
                LOG_W_TIME("Tried to unsubscribe an event", typeid(_type).name()," that wasn't subscribed yet!")
            }

            bool dispatch(const Event& _type, AssociatedFunctionArgs... _args) {
                if(!hasType(_type)) {
                    return false;
                }

                std::vector<bool> _results;
                handlers[_type].exec(_results, _args...);
                return std::all_of(_results.begin(), _results.end(), [](bool v) { return v; });
            }

            bool isSubscribed(const Event& _type) {
                return hasType(_type);
            }

        private:
            bool hasType(const Event& _type) {
                return handlers.find(_type) != handlers.end();
            }
    };

}


#endif // GDE_EVENT_BUS
