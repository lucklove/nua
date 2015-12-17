#pragma once
#include "BaseFunc.hh"
#include "stack.hh"
#include <tuple>
#include <functional>

namespace nua
{
namespace detail
{
    template <typename... Boolean>
    constexpr bool is_all_true(bool first, Boolean... left)
    {
        return first && is_all_true(left...);
    }

    template <>
    constexpr bool is_all_true<>(bool first)
    {
        return first;
    }

    template <typename Ret, typename... Args, size_t... Is>
    void apply_n(lua_State* l, std::function<Ret(Args...)> func, std::index_sequence<Is...>)
    {
        static_assert(is_primitive<Ret>::value, "not support return userdata temporarily");
        Ret ret = func(stack::get<Args>(l, int(Is - sizeof...(Is)))...);
        lua_pop(l, int(sizeof...(Is)));
        stack::push(l, ret);
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
        static_assert(is_all_true(is_primitive<Rets>::value...), "not support return userdata temporarily");
        std::tuple<Rets...> ret = func(stack::get<Args>(l, int(ArgIs - sizeof...(ArgIs)))...);
        lua_pop(l, int(sizeof...(ArgIs)));
        (void)std::initializer_list<int>{(stack::push(l, std::get<RetIs>(ret)), 0)...}; 
    }
}

    template <typename Ret, typename... Args>
    class Func : public BaseFunc
    {
    public:
        using func_t = std::function<Ret(Args...)>;

    private:
        func_t func_;

    public:
        Func(func_t func) : func_{func} {}

        int apply(lua_State* l) override
        {
            detail::apply_n(l, func_, std::make_index_sequence<sizeof...(Args)>());        
            return 1;
        }        
    };

    template <typename... Args>
    class Func<void, Args...> : public BaseFunc
    {
    public:
        using func_t = std::function<void(Args...)>;

    private:
        func_t func_;

    public:
        Func(func_t func) : func_{func} {}
    
        int apply(lua_State* l) override
        {
            detail::apply_n(l, func_, std::make_index_sequence<sizeof...(Args)>());        
            return 0;
        }        
    };

    template <typename... Rets, typename... Args>
    class Func<std::tuple<Rets...>, Args...> : public BaseFunc
    {
    public:
        using func_t = std::function<std::tuple<Rets...>(Args...)>;

    private:
        func_t func_;

    public:
        Func(func_t func) : func_{func} {}

        int apply(lua_State* l) override
        {
            detail::apply_n(l, func_, 
                std::make_index_sequence<sizeof...(Rets)>(),  
                std::make_index_sequence<sizeof...(Args)>()); 
            return sizeof...(Rets);
        }        
    };
}
