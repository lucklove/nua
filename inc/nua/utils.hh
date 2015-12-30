#pragma once
/**
 * \need:
 *      stack.hh for stack operations
 *      LuaRef.hh for class LuaRef, class LuaRefPtr 
 */

namespace nua
{
    namespace utils
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
                stack::push(l, detail::forward_ref<Rets>(std::get<RetIs>(ret), typename std::is_lvalue_reference<Rets>::type{})), 
            0)...}; 
        }
    
        template <typename T>
        LuaRefPtr make_lua_ref(lua_State* ctx, T t)
        {
            stack::push(ctx, t);
            return std::make_shared<LuaRef>(ctx, luaL_ref(ctx, LUA_REGISTRYINDEX));
        }

        template <typename... Args>
        std::vector<LuaRefPtr> make_lua_refs(lua_State* ctx, Args&&... args)
        {
            return {make_lua_ref(ctx, args)...};       
        }

        template <typename... Args, size_t... Is>
        static std::tuple<Args...> get_n(lua_State* l, std::index_sequence<Is...>)
        {
            ScopeGuard sg([l]{ lua_pop(l, int(sizeof...(Is))); });
            return std::tuple<Args...>(stack::get<Args>(l, int(Is - sizeof...(Is)))...);
        }
    }
}
