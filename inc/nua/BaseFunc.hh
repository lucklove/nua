#pragma once
#include <lua.hpp>
#include "ExceptionHolder.hh"

namespace nua
{
namespace detail
{
    template <typename T>
    constexpr T forward_ref(T t, std::false_type)
    {
        return t;
    }

    template <typename T>
    constexpr std::reference_wrapper<typename std::decay<T>::type> forward_ref(T t, std::true_type)
    {
        return std::reference_wrapper<typename std::decay<T>::type>(t);
    }

    template <typename T>
    void apply_push(lua_State* l, T& t, std::true_type)
    {
        stack::push(l, std::ref(t));
    }

    template <typename T>
    void apply_push(lua_State* l, const T& t, std::false_type)
    {
        stack::push(l, t);
    }

    template <typename Ret, typename... Args, size_t... Is>
    void apply_n(lua_State* l, std::function<Ret(Args...)> func, std::index_sequence<Is...>)
    {
        Ret ret = func(stack::get<Args>(l, int(Is - sizeof...(Is)))...);
        lua_pop(l, int(sizeof...(Is)));
        apply_push(l, ret, typename std::is_lvalue_reference<Ret>::type{});
    }

    template <typename... Args, size_t... Is>
    void apply_n(lua_State* l, std::function<void(Args...)> func, std::index_sequence<Is...>)
    {
        func(stack::get<Args>(l, int(Is - sizeof...(Is)))...);
        lua_pop(l, int(sizeof...(Is)));
    }

    template <typename... Rets, typename... Args, size_t... RetIs, size_t... ArgIs>
    void apply_n(lua_State* l, 
        std::function<std::tuple<Rets...>(Args...)> func, 
        std::index_sequence<RetIs...>, 
        std::index_sequence<ArgIs...>)
    {
        std::tuple<Rets...> ret = func(stack::get<Args>(l, int(ArgIs - sizeof...(ArgIs)))...);
        lua_pop(l, int(sizeof...(ArgIs)));
        (void)std::initializer_list<int>{(
            stack::push(l, forward_ref<Rets>(std::get<RetIs>(ret), typename std::is_lvalue_reference<Rets>::type{})), 
        0)...}; 
    }
}

    struct BaseFunc
    {
        virtual int apply(lua_State*) = 0;
        virtual ~BaseFunc() = default;

        static int dispatcher(lua_State *l) noexcept
        {
            try
            {
                BaseFunc* func_ptr = (BaseFunc *)lua_touserdata(l, lua_upvalueindex(1));
                if(!func_ptr) 
                    throw std::runtime_error{"no function infomation found while try to make a call"};
                return func_ptr->apply(l);
            }
            catch(...)
            {
                ExceptionHolder::set();
            }

            lua_pushstring(l, "exception in dispatcher");
            return lua_error(l);
        }
    };
}
