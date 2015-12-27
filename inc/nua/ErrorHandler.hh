#pragma once
#include <lua.hpp>
#include <stdexcept>
#include <string>
#include "ExceptionHolder.hh"

namespace nua
{
    struct ErrorHandler
    {
    private:
        static int error_handler(lua_State* l) noexcept
        {
            /** trace back */
            const char* msg = "<not set>";
            if(!lua_isnil(l, -1))
            {
                msg = lua_tostring(l, -1);
                if(msg == nullptr)
                    msg = "<not set>";
            }

            lua_pushstring(l, msg);
            lua_getglobal(l, "debug");
            lua_getfield(l, -1, "traceback");
            lua_pushvalue(l, -3);
            lua_pushinteger(l, 2);
            lua_call(l, 2, 1);
            return 1;
        }   

    public:
        [[noreturn]]
        static void handle(lua_State* l, int status_code)
        {
            ExceptionHolder::check();

            if(lua_isstring(l, -1))
            {
                throw std::runtime_error{lua_tostring(l, -1)};
            }
            else
            {
                std::cout << "error code: " << status_code << std::endl;
                static std::string msg;
                msg = "unexpected error, error code: " + std::to_string(status_code);
                throw std::runtime_error{msg.c_str()};
            }
        }

        static int set_error_handler(lua_State* l)
        {
            lua_pushcfunction(l, error_handler);
            return lua_gettop(l);
        }
    };
}
