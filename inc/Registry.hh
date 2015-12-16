#pragma once
#include <lua.hpp>
#include <memory>
#include "MetatableRegistry.hh"

namespace nua
{
    class Registry
    {
    private:
        lua_State* ctx_;
        
    public:
        Registry(lua_State* ctx) : ctx_(ctx) 
        {
            MetatableRegistry::create(ctx_);
        }
    };
}
