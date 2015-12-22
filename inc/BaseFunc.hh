#pragma once
#include <lua.hpp>

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

    template <typename Ret, typename... Args, size_t... Is>
    void apply_n(lua_State* l, std::function<Ret(Args...)> func, std::index_sequence<Is...>)
    {
        Ret ret = func(stack::get<Args>(l, int(Is - sizeof...(Is)))...);
        lua_pop(l, int(sizeof...(Is)));
        if(std::is_lvalue_reference<Ret>::value)
        {
            stack::push(l, std::ref(ret));
        }
        else
        {
            stack::push(l, ret);
        }
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
//        static_assert(is_all_true(is_primitive<Rets>::value...), "not support return userdata temporarily");
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

        static int dispatcher(lua_State *l)
        {
            BaseFunc* func_ptr = (BaseFunc *)lua_touserdata(l, lua_upvalueindex(1));
            if(!func_ptr) 
                throw std::runtime_error{"no function infomation found while try to make a call"};
            return func_ptr->apply(l);
        }
    };
}
