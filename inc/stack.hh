#pragma once
#include <lua.hpp>
#include "primitives.hh"
#include "MetatableRegistry.hh"
#include "types.hh"
#include "ScopeGuard.hh"

namespace nua
{
namespace stack
{
    class StackGuard
    {
    private:
        ScopeGuard guard_;

    public:
        StackGuard(lua_State* l) :
        guard_{[l, saved_top_index = lua_gettop(l)]
        {
            lua_settop(l, saved_top_index);
        }}
        {}
    };

    template <typename T>
    typename std::enable_if<
        !is_primitive<typename std::decay<T>::type>::value
        && !std::is_const<typename std::remove_reference<T>::type>::value,
    void>::type 
    push(lua_State* l, T& t)
    {
        using value_t = typename std::decay<T>::type;
        value_t** ptr = (value_t **)lua_newuserdata(l, sizeof(value_t *));
        nua::MetatableRegistry::set_metatable<value_t>(l);
        *ptr = &t;    
    }

    inline void push(lua_State* l, bool v)
    {
        lua_pushboolean(l, v);
    }

    inline void push(lua_State* l, int v)
    {
        lua_pushinteger(l, v);
    }

    inline void push(lua_State* l, lua_Number v)
    {
        lua_pushnumber(l, v);
    }

    inline void push(lua_State* l, const std::string& v)
    {
        lua_pushlstring(l, v.c_str(), v.size());   
    }

    inline void push(lua_State* l, const char* v)
    {
        lua_pushstring(l, v);   
    }

    inline void push(lua_State* l, std::nullptr_t)
    {
        lua_pushnil(l);    
    }

    template <typename T>
    void set(lua_State* l, int index, T&& v)
    {
        push(l, std::forward<T>(v));
        lua_replace(l, index);   
    }

    template <typename T>
    typename std::enable_if<!std::is_rvalue_reference<T>::value, T>::type
    get(lua_State* l, int index)
    {
        static_assert(!is_primitive<typename std::decay<T>::type>::value, "not implemented primitive for get");
        if(!check::is_type<T>(l, index))
        {
            std::cout << "can not get user type at index " << index << std::endl;
            throw std::bad_cast{};
        }

        using value_t = typename std::decay<T>::type;
        value_t** ptr = (value_t**)lua_touserdata(l, index);
        if(ptr == nullptr)
        {
            std::cout << "can not get user type at index " << index << std::endl;
            throw std::bad_cast{};
        }
        return **ptr;
    }

    template <>
    inline bool get<bool>(lua_State* l, int index)
    {
        if(!lua_isboolean(l, index))
        {
            std::cout << "can not get boolean at index " << index << std::endl;
            throw std::bad_cast{};
        }
        return lua_toboolean(l, index);
    }

    template <>
    inline int get<int>(lua_State* l, int index)
    {
        if(!lua_isinteger(l, index))
        {
            std::cout << "can not get integer at index " << index << std::endl;
            throw std::bad_cast{};
        }
        return lua_tointeger(l, index);
    }

    template <>
    inline lua_Number get<lua_Number>(lua_State* l, int index)
    {
        if(!lua_isnumber(l, index))
        {
            std::cout << "can not get number at index " << index << std::endl;
            throw std::bad_cast{};
        }
        return lua_tonumber(l, index);
    }

    template <>
    inline std::string get<std::string>(lua_State* l, int index)
    {
        if(!lua_isstring(l, index))
        {
            std::cout << "can not get string at index " << index << std::endl;
            throw std::bad_cast{};
        }
        return lua_tostring(l, index);
    }

    template <>
    inline std::nullptr_t get<std::nullptr_t>(lua_State* l, int index)
    {
        if(!lua_isnil(l, index))
        {
            std::cout << "can not get nil at index " << index << std::endl;
            throw std::bad_cast{};
        }

        return nullptr;
    }

    template <typename T>
    typename std::enable_if<!std::is_reference<T>::value, T>::type
    pop(lua_State* l)
    {
        T ret = get<T>(l, -1);
        lua_pop(l, 1);
        return ret;
    } 
}       /**< stack */
}       /**< nua */
