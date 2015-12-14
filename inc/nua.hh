#pragma once
#include <lua.hpp>
#include <stdexcept>

namespace nua
{
    class Context
    {
    private:
        lua_State* lua_;
    
    public:
        Context(bool should_open_libs = true) explicit
        {
            lua_ = luaL_newstate();
            if(lua_ == nullptr)
                throw std::runtime_error{"initialize lua context failed"};
        }
    }; 
}
