// Created by borja on 8/3/22.


#ifndef ENGINE_DELEGATE_H
#define ENGINE_DELEGATE_H

#include <exception>
#include <functional>

namespace engine {

    // Primary template intentionally left empty
    template <typename Signature>
    class UDelegate;

    template <typename Signature>
    class MDelegate;

    template <typename Signature>
    struct Stub;

    class BadDelegateCall : public std::exception {
        public:
            [[nodiscard]] auto what() const noexcept -> const char* override {
                return "Bad delegate call";
            }
    };

    template <typename R, typename...Args>
    class UDelegate<R(Args...)> {
        protected:
            [[noreturn]] static auto stub_null(const void* _p, Args...) -> R {
                throw BadDelegateCall{};
            }

            using stub_function = R(*)(const void*, Args...);
            const void* instance = nullptr; ///< A pointer to the instance (if it exists)
            stub_function stub = &stub_null; ///< A pointer to the function to invoke

        public:
            UDelegate() = default;
            UDelegate(const UDelegate& other) = default;
            auto operator=(const UDelegate& other) -> UDelegate& = default;
            auto operator()(Args... _args) const -> R {
                return std::invoke(stub, instance, _args...);
            }


            // FREE FUNCTION
            template <auto Function, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(Function), Args...>>>
            auto bind() -> void {
                instance = nullptr;
                stub = static_cast<stub_function>([](const void*, Args... _args) -> R{
                    return std::invoke(Function, std::forward<Args>(_args)...);
                });
            }

            // CONST MEMBER FUNCTION
            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),const Class*, Args...>>>
            auto bind(const Class* _cls) -> void {
                instance = _cls;
                stub = static_cast<stub_function>([](const void* _p, Args... _args) -> R{
                    const auto* _c = static_cast<const Class*>(_p);
                    return std::invoke(MemberFunction, _c, _args...);
                });
            }

            // NON-CONST MEMBER FUNCTION
            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),Class*, Args...>>>
            auto bind(Class* _cls) -> void {
                instance = _cls;
                stub = static_cast<stub_function>([](const void* _p, Args... _args) -> R{
                    auto* _c = const_cast<Class*>(static_cast<const Class*>(_p));
                    return std::invoke(MemberFunction, _c, _args...);
                });
            }
    };


    template <typename R, typename...Args>
    struct Stub<R(Args...)> {
        using stub_function = R(*)(const void*, Args...);
        const void* instance = nullptr; ///< A pointer to the instance (if it exists)
        stub_function stub = &stub_null; ///< A pointer to the function to invoke

        [[noreturn]] static auto stub_null(const void* _p, Args...) -> R {
            throw BadDelegateCall{};
        }
    };

    template <typename R, typename...Args>
    class MDelegate<R(Args...)> {
        using stub_function = R(*)(const void*, Args...);
        private:
            std::vector<Stub<R(Args...)>> stubs;

        public:
            // Creates an unbound delegate
            MDelegate() = default;

            // We want the Delegate to be copyable, since its lightweight
            MDelegate(const MDelegate& other) = default;
            auto operator=(const MDelegate& other) -> MDelegate& = default;

            // Call the underlying bound function
            auto operator()(Args... args) const -> R {
                R _r;
                for(auto& _stub : stubs) _r = std::invoke(_stub.stub, _stub.instance, args...);
                return _r;
            }

            template <auto Function, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(Function),Args...>>>
            auto bind() -> void {
                Stub<R(Args...)> _s {
                    nullptr,
                    static_cast<stub_function>([](const void*, Args... _args) -> R {
                        return std::invoke(Function, std::forward<Args>(_args)...);
                    })
                };
                stubs.push_back(_s);
            }

            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),const Class*, Args...>>>
            auto bind(const Class* _cls) -> void {
                Stub<R(Args...)> _s {
                        _cls,
                        static_cast<stub_function>([](const void* _p, Args... _args) -> R {
                            const auto* _c = static_cast<const Class*>(_p);
                            return std::invoke(MemberFunction, _c, _args...);
                        })
                };
                stubs.push_back(_s);
            }

            template <auto MemberFunction, typename Class, typename = std::enable_if_t<std::is_invocable_r_v<R, decltype(MemberFunction),Class*, Args...>>>
            auto bind(Class* _cls) -> void {
                Stub<R(Args...)> _s {
                        _cls,
                        static_cast<stub_function>([](const void* _p, Args... _args) -> R {
                            auto* _c = const_cast<Class*>(static_cast<const Class*>(_p));
                            return std::invoke(MemberFunction, _c, _args...);
                        })
                };
                stubs.push_back(_s);
            }

            void clear() {
                stubs.clear();
            }
    };
}

#endif //ENGINE_DELEGATE_H


// EXAMPLE USAGE
//struct A {
//    bool myFunc(int _a, const std::string& _b) {
//        LOG_I("Called from A")
//        return false;
//    }
//};
//
//struct B {
//    bool myFunc(int _a, const std::string& _b) {
//        LOG_I("Called from B")
//        return false;
//    }
//};
//
//struct C {
//    bool myFunc(int _a, const std::string& _b) {
//        LOG_I("Called from C")
//        return false;
//    }
//};
//
//bool myFunc(int _a, const std::string& _b) {
//    LOG_I("Called from free")
//    return false;
//}
//
//
//engine::MDelegate<bool(int, const std::string&)> _myMDelegate;
//engine::UDelegate<bool(int, const std::string&)> _myUDelegate;
//A _a;
//C _c;
//
//_myUDelegate.bind<&myFunc>();
//_myUDelegate.bind<&A::myFunc>(&_a);
//
//_myMDelegate.bind<&myFunc>();
//_myMDelegate.bind<&A::myFunc>(&_a);
//_myMDelegate.bind<&A::myFunc>(&_a);
//_myMDelegate.bind<&C::myFunc>(&_c);
//_myMDelegate(0, "");
//_myMDelegate.clear();