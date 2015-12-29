#pragma once

namespace nua
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
}
