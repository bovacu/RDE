// Created by borja on 8/3/22.

#ifndef RDE_DELEGATE_H
#define RDE_DELEGATE_H

#include <exception>
#include <functional>
#include <unordered_map>
#include "core/Core.h"

namespace RDE {

    template <typename R = void, typename...Args>   
    struct Delegate;

    // Primary template intentionally left empty
    template <typename Signature>
    struct UniqueDelegate;

    template <typename Signature>
    struct ReturnDelegate;

    template <typename Signature>
    struct Stub;

    class BadDelegateCall : public std::exception {
        public:
			RDE_FUNC_ND auto what() const noexcept -> const char* override {
                // auto _error = APPEND_S("Delegate had no attached function, in line ", __LINE__, " in file ", __FILE_NAME__);
                return "Delegate had no attached function";
            }
    };

    template <typename R, typename...Args>
    struct Stub<R(Args...)> {
        using stub_function = R(*)(const void*, Args...);
        const void* instance = nullptr; ///< A pointer to the instance (if it exists)
        stub_function stub = &stub_null; ///< A pointer to the function to invoke

		RDE_FUNC_ND_STATIC auto stub_null(const void* _p, Args...) -> R {
            throw BadDelegateCall{};
        }
    };

    typedef uint32_t DelegateID;


    /**
     * @brief This struct can hold one reference to another function, and execute it whenever is needed. It can return void or any other desired type. Use it if just one function reference is
     * going to be stored. 
     */ 
    template <typename R, typename...Args>
    struct UniqueDelegate<R(Args...)> {

        friend class Delegate<R(Args...)>;
        friend class ReturnDelegate<R(Args...)>;

        protected:
			RDE_FUNC_ND_STATIC auto stub_null(const void* _p, Args...) -> R {
                throw BadDelegateCall{};
            }

            using stub_function = R(*)(const void*, Args...);
            const void* instance = nullptr; ///< A pointer to the instance (if it exists)
            stub_function stub = &stub_null; ///< A pointer to the function to invoke

        public:
			RDE_FUNC UniqueDelegate() {}
			RDE_FUNC UniqueDelegate(const UniqueDelegate& other) = default;
			RDE_FUNC auto operator=(const UniqueDelegate& other) -> UniqueDelegate& = default;
			RDE_FUNC auto operator==(UniqueDelegate* _other) {
                if (_other == nullptr) {
                    return instance == nullptr && stub == stub_null;
                }
                return _other->instance == instance && _other->stub == stub;
            }
			RDE_FUNC auto operator!=(UniqueDelegate* _other) {
                if (_other == nullptr) {
                    return instance != nullptr || stub != stub_null;
                }
                return _other->instance != instance && _other->stub != stub;
            }
			RDE_FUNC auto operator==(const UniqueDelegate& _other) { return _other->instance == instance; }
			RDE_FUNC auto operator!=(const UniqueDelegate& _other) { return _other->instance != instance; }
			RDE_FUNC auto operator()(Args... _args) const -> R {
                return std::invoke(stub, instance, _args...);
            }


            // FREE FUNCTION
            template <auto Function, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(Function), Args...>>>
			RDE_FUNC auto bind() -> void {
                instance = nullptr;
                stub = static_cast<stub_function>([](const void*, Args... _args) -> R{
                    return std::invoke(Function, std::forward<Args>(_args)...);
                });
            }

            // CONST MEMBER FUNCTION
            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),const Class*, Args...>>>
			RDE_FUNC auto bind(const Class* _cls) -> void {
                instance = _cls;
                stub = static_cast<stub_function>([](const void* _p, Args... _args) -> R{
                    const auto* _c = static_cast<const Class*>(_p);
                    return std::invoke(MemberFunction, _c, _args...);
                });
            }

            // NON-CONST MEMBER FUNCTION
            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),Class*, Args...>>>
			RDE_FUNC auto bind(Class* _cls) -> void {
                instance = _cls;
                stub = static_cast<stub_function>([](const void* _p, Args... _args) -> R{
                    auto* _c = const_cast<Class*>(static_cast<const Class*>(_p));
                    return std::invoke(MemberFunction, _c, _args...);
                });
            }

			RDE_FUNC void unbind() {
                instance = nullptr;
                stub = stub_null;
            }
    };

    /**
     * @brief This struct can store as many void functions as needed and execute them all at once when needed. Only void function types allowed. To return values use UniqueDelegate or ReturnDelegate.
     */
    template <typename R, typename...Args>
    struct Delegate<R(Args...)> {
        private:
            using stub_function = R(*)(const void*, Args...);
            std::unordered_map<DelegateID, UniqueDelegate<R(Args...)>> callables;
            uint32_t id;

        public:
			RDE_FUNC auto operator()(Args... args) {
                for(auto _it = callables.begin(); _it != callables.end(); _it++) _it->second(args...);
            }

            // FREE FUNCTION
            template <auto Function, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(Function), Args...>>>
			RDE_FUNC DelegateID bind() {
                callables[id] = UniqueDelegate<R(Args...)>();
                callables[id].instance = nullptr;
                callables[id].stub = static_cast<stub_function>([](const void*, Args... _args) -> R{
                    return std::invoke(Function, std::forward<Args>(_args)...);
                });

                return id++;
            }

            // CONST MEMBER FUNCTION
            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),const Class*, Args...>>>
			RDE_FUNC DelegateID bind(const Class* _cls) {
                callables[id] = UniqueDelegate<R(Args...)>();
                callables[id].instance = _cls;
                callables[id].stub = static_cast<stub_function>([](const void* _p, Args... _args) -> R{
                    const auto* _c = static_cast<const Class*>(_p);
                    return std::invoke(MemberFunction, _c, _args...);
                });

                return id++;
            }

            // NON-CONST MEMBER FUNCTION
            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),Class*, Args...>>>
			RDE_FUNC DelegateID bind(Class* _cls) {
                callables[id] = UniqueDelegate<R(Args...)>();
                callables[id].instance = _cls;
                callables[id].stub = static_cast<stub_function>([](const void* _p, Args... _args) -> R{
                    auto* _c = const_cast<Class*>(static_cast<const Class*>(_p));
                    return std::invoke(MemberFunction, _c, _args...);
                });

                return id++;
            }


			RDE_FUNC void clear() {
                callables.clear();
            }

			RDE_FUNC void unbind(DelegateID _id) {
                auto _it = callables.find(_id);
                if(_it != callables.end()) {
                    callables.erase(_id);
                }
            }

			RDE_FUNC_ND bool isEmpty() {
                return callables.empty();
            }
    };

    /**
     * @brief This struct can store as many return type functions as needed and execute them all at once when needed. Any return type is valid except void. If no value is returned, use UniqueDelegate or Delegate.
     */
    typedef uint32_t DelegateRef;
    template <typename R, typename...Args>
    struct ReturnDelegate<R(Args...)> {
        using stub_function = R(*)(const void*, Args...);
        private:
            std::unordered_map<DelegateID, UniqueDelegate<R(Args...)>> callables;
            uint32_t id;

        public:
            // Creates an unbound delegate
			RDE_FUNC ReturnDelegate() {}

            // We want the Delegate to be copyable, since its lightweight
			RDE_FUNC ReturnDelegate(const ReturnDelegate& other) = default;
			RDE_FUNC auto operator=(const ReturnDelegate& other) -> ReturnDelegate& = default;
			RDE_FUNC std::vector<R> operator()(Args... args) {
                std::vector<R> _results;
                for(auto _it = callables.begin(); _it != callables.end(); _it++) _it->second(args...);
                return _results;
            }

			// FREE FUNCTION
            template <auto Function, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(Function), Args...>>>
			RDE_FUNC DelegateID bind() {
                callables[id] = UniqueDelegate<R(Args...)>();
                callables[id].instance = nullptr;
                callables[id].stub = static_cast<stub_function>([](const void*, Args... _args) -> R{
                    return std::invoke(Function, std::forward<Args>(_args)...);
                });

                return id++;
            }

            // CONST MEMBER FUNCTION
            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),const Class*, Args...>>>
			RDE_FUNC DelegateID bind(const Class* _cls) {
                callables[id] = UniqueDelegate<R(Args...)>();
                callables[id].instance = _cls;
                callables[id].stub = static_cast<stub_function>([](const void* _p, Args... _args) -> R{
                    const auto* _c = static_cast<const Class*>(_p);
                    return std::invoke(MemberFunction, _c, _args...);
                });

                return id++;
            }

            // NON-CONST MEMBER FUNCTION
            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),Class*, Args...>>>
			RDE_FUNC DelegateID bind(Class* _cls) {
                callables[id] = UniqueDelegate<R(Args...)>();
                callables[id].instance = _cls;
                callables[id].stub = static_cast<stub_function>([](const void* _p, Args... _args) -> R{
                    auto* _c = const_cast<Class*>(static_cast<const Class*>(_p));
                    return std::invoke(MemberFunction, _c, _args...);
                });

                return id++;
            }


			RDE_FUNC void clear() {
                callables.clear();
            }

			RDE_FUNC void unbind(DelegateID _id) {
                auto _it = callables.find(_id);
                if(_it != callables.end()) {
                    callables.erase(_id);
                }
            }

			RDE_FUNC_ND bool isEmpty() {
                return callables.empty();
            }
    };
}

#endif //RDE_DELEGATE_H
