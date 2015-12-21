#pragma once
#include "BaseFunc.hh"

namespace nua
{
    template <typename T>
    class Dtor : public BaseFunc
    {
    private:
        std::string metatable_name_;

    public:
        Dtor(lua_State* l, const std::string &metatable_name)
            : metatable_name_{metatable_name}
        {
            lua_pushlightuserdata(l, (void *)(this));
            lua_pushcclosure(l, &BaseFunc::dispatcher, 1);
            lua_setfield(l, -2, "__gc");
        }

        int apply(lua_State* l)
        {
std::cout << "gc" << std::endl;
            T *t = (T *)luaL_checkudata(l, 1, metatable_name_.c_str());
            t->~T();
            return 0;
        }
    };
}
