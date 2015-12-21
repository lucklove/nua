#pragma once
#include "BaseFunc.hh"

namespace nua
{
    template <typename T, typename... Args>
    class Ctor : public BaseFunc
    {
    private:
        using func_t = std::function<void(lua_State* l, Args...)>;
        func_t func_;

    public:
        Ctor(lua_State* l, const std::string& metatable_name)
            : func_([metatable_name](lua_State* l, Args... args)
            {
                void *addr = lua_newuserdata(l, sizeof(T));
                new(addr) T(args...);
                luaL_setmetatable(l, metatable_name.c_str());
            })
        {
            lua_pushlightuserdata(l, (void *)static_cast<BaseFunc*>(this));
            lua_pushcclosure(l, &BaseFunc::dispatcher, 1);
            lua_setfield(l, -2, "new");
        }

        int apply(lua_State* l)
        {
std::cout << "new" << std::endl;
            detail::apply_n(l, func_, std::make_index_sequence<sizeof...(Args) + 1>());
            return 1;
        }
    };
}
