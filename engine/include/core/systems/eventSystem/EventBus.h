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
        std::map<Type, engine::MDelegate<bool(Args...)>> handlers;

    public:
        class HandlerId {
            public:
                HandlerId() : valid(false) {  }
                friend class EventBus<Args...>;
                size_t id{};
                explicit HandlerId(size_t _id) : id(_id), valid(true) {  }
                bool valid;
        };

        // register to be notified
        template<auto Func, typename Class>
        HandlerId subscribe(const Type& _key, Class* _class) {
            if (Func) {
                handlers[_key].template bind<Func>(_class);
                return HandlerId(handlers[_key].getEnd());
            }
            return HandlerId();
        }

        void unsubscribe(const Type& _type, HandlerId& _handlerId) {
            if(!hasType(_type)) {
                LOG_W_TIME("Tried to unsubscribe Type ", typeid(_type).name(), " but it wasn't subscribed!")
                return;
            }

            if (_handlerId.valid) {
                handlers[_type].remove(_handlerId.id);
            } else
                LOG_W_TIME("Tried to unsubscribe an event", typeid(_type).name()," that wasn't subscribed yet!")
        }

        bool dispatch(const Type& _type, Args... _args) {
            if(!hasType(_type)) {
                return false;
            }

            std::vector<bool> _results;
            handlers[_type].exec(_results, _args...);
            return std::all_of(_results.begin(), _results.end(), [](bool v) { return v; });
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
