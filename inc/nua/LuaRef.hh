#pragma once

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
}
