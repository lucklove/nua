#pragma once
#include "LuaRef.hh"
#include "stack.hh"

namespace nua
{
    namespace utils
    {
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
