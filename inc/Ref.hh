#pragma once
#include <lua.hpp>
#include <memory>
#include "stack.hh"

namespace nua
{
    class Ref
    {
    private:
        lua_State* l_;
        int ref_;
    
    public:
        Ref(lua_State* ctx, int ref)
            : l_{ctx}, ref_{ref}
        {}
    
        Ref(lua_State* ctx)
            : Ref(ctx, LUA_REFNIL)
        {}

        Ref(const Ref&) = delete;

        Ref(Ref&& other) : l_{other.l_}, ref_{other.ref_}
        {
            other.l_ = nullptr;
            other.ref_ = 0;
        }
       
        ~Ref()
        {
            if(l_)
                luaL_unref(l_, LUA_REGISTRYINDEX, ref_);
        }

        void push() const
        {
            lua_rawgeti(l_, LUA_REGISTRYINDEX, ref_);
        }
    };

    using RefPtr = std::shared_ptr<Ref>;

    template <typename T>
    RefPtr make_ref(lua_State* ctx, T&& t)
    {
        stack::push(ctx, std::forward<T>(t));
        return std::make_shared<Ref>(ctx, luaL_ref(ctx, LUA_REGISTRYINDEX));
    }


    template <typename... Args>
    std::vector<RefPtr> make_refs(lua_State* ctx, Args&&... args)
    {
        return {make_ref(ctx, args)...};       
    }
}
