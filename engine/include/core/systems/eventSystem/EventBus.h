// Created by borja on 17/12/21.

#ifndef RESOURCES_GAME_EVENT_BUS
#define RESOURCES_GAME_EVENT_BUS

#include <functional>
#include <map>
#include "core/util/Logger.h"
#include <type_traits>

template <typename Type, typename... Args>
class EventBus {
//    static_assert(std::is_enum<Type>::value, "Must be an enum type");
    public:
        typedef std::function<bool(Args...)> HandlerFunc;

    private:
        std::map<Type, std::vector<HandlerFunc>> handlers;

    public:
        class HandlerId {
            public:
                HandlerId() : valid(false) {  }
                friend class EventBus<Args...>;
                typename std::vector<HandlerFunc>::iterator iter;
                explicit HandlerId(typename std::vector<HandlerFunc>::iterator i) : iter(i), valid(true) {  }
                bool valid;
        };

        // register to be notified
        HandlerId subscribe(const Type& _key, HandlerFunc _func) {
            if (_func) {
                handlers[_key].push_back(_func);
                return HandlerId(--handlers[_key].end());
            }
            return HandlerId();
        }

        void unsubscribe(const Type& _type, HandlerId& _handlerId) {
            if(!hasType(_type)) {
                LOG_W_TIME("Tried to unsubscribe Type ", typeid(_type).name(), " but it wasn't subscribed!")
                return;
            }

            if (_handlerId.valid) {
                handlers[_type].erase(_handlerId.iter);
            } else
                LOG_W_TIME("Tried to unsubscribe an event", typeid(_type).name()," that wasn't subscribed yet!")
        }

        bool dispatch(const Type& _type, Args... _args) {
            if(!hasType(_type)) {
//                LOG_W_TIME("Tried to dispatch Type ", typeid(_type).name(), "[", _type, "] but it wasn't subscribed!")
                return false;
            }

            bool _success = false;
            for (auto& _cb : handlers[_type]) {
                _success |= _cb(_args...);
            }

            return _success;
        }

        bool isSubscribed(const Type& _type) {
            return hasType(_type);
        }

    private:
        bool hasType(const Type& _type) {
            return handlers.find(_type) != handlers.end();
        }
};


#endif // RESOURCES_GAME_EVENT_BUS
