#pragma once

namespace nua
{
namespace check
{
    template <typename T>
    bool is_type(lua_State* ctx, int index)
    {
        bool equal = false;

        if(lua_getmetatable(ctx, index))
        {
            MetatableRegistry::get_metatable<T>(ctx);
            equal = lua_istable(ctx, -1) && lua_rawequal(ctx, -1, -2);
            lua_pop(ctx, 2);
        }

        return equal;
    }

    template <>
    inline bool is_type<bool>(lua_State* ctx, int index)
    {
        return lua_isboolean(ctx, index);
    }

    template <>
    inline bool is_type<int>(lua_State* ctx, int index)
    {
        return lua_isinteger(ctx, index);
    }
 
    template <>
    inline bool is_type<lua_Number>(lua_State* ctx, int index)
    {
        return lua_isnumber(ctx, index);
    }

    template <>
    inline bool is_type<std::string>(lua_State* ctx, int index)
    {
        return lua_isstring(ctx, index);
    }

    template <>
    inline bool is_type<std::nullptr_t>(lua_State* ctx, int index)
    {
        return lua_isnil(ctx, index);
    }
}
}

