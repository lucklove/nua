#pragma once
#include <stdexcept>
#include <lua.hpp>
#include <string>
#include "ScopeGuard.hh"
#include "ErrorHandler.hh"

namespace nua
{
    class Context
    {
    private:
        lua_State* lua_ctx_;
    
    public:
        explicit Context(bool should_open_libs = true)
        {
            lua_ctx_ = luaL_newstate();
            if(lua_ctx_ == nullptr)
                throw std::runtime_error{"initialize lua context failed"};
            if(should_open_libs)
                luaL_openlibs(lua_ctx_);
        }

        ~Context()
        {
            forceGC();
            lua_close(lua_ctx_);
        }

        Context(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(const Context&) = delete;
        Context& operator=(Context&&) = delete;

        void operator()(const char* code)
        {
            ScopeGuard reset_stack_on_exit([this, saved_top_index = lua_gettop(lua_ctx_)]
            {
                lua_settop(lua_ctx_, saved_top_index);
            });
            
            int status = luaL_dostring(lua_ctx_, code);
            if(status)
                ErrorHandler::handle(lua_ctx_, status);
        }

        void load(const std::string& file)
        {
            ScopeGuard reset_stack_on_exit([this, saved_top_index = lua_gettop(lua_ctx_)]
            {
                lua_settop(lua_ctx_, saved_top_index);
            });
           
            int status = luaL_loadfile(lua_ctx_, file.c_str()); 
            if(status)
            {
                ErrorHandler::handle(lua_ctx_, status);
                return;
            }

            status = lua_pcall(lua_ctx_, 0, LUA_MULTRET, 0);
            if(status)
                ErrorHandler::handle(lua_ctx_, status);
        }

        void forceGC()
        {
            lua_gc(lua_ctx_, LUA_GCCOLLECT, 0);
        }
    };
}
