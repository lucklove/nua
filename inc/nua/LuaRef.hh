#pragma once
#include <lua.hpp>
#include <memory>
#include "stack.hh"

namespace nua
{
    class LuaRef
    {
    private:
        lua_State* l_;
        int ref_;
    
    public:
        LuaRef(lua_State* ctx, int ref)
            : l_{ctx}, ref_{ref}
        {}
    
        LuaRef(lua_State* ctx)
            : LuaRef(ctx, LUA_REFNIL)
        {}

        LuaRef(const LuaRef&) = delete;

        LuaRef(LuaRef&& other) : l_{other.l_}, ref_{other.ref_}
        {
            other.l_ = nullptr;
            other.ref_ = 0;
        }
       
        ~LuaRef()
        {
            if(l_)
                luaL_unref(l_, LUA_REGISTRYINDEX, ref_);
        }

        void push() const
        {
            lua_rawgeti(l_, LUA_REGISTRYINDEX, ref_);
        }
    };

    using LuaRefPtr = std::shared_ptr<LuaRef>;

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
}
