#pragma once
#include <lua.hpp>
#include <stdexcept>
#include <string>

namespace nua
{
    struct ErrorHandler
    {
        [[noreturn]]
        static void handle(lua_State* l, int status_code)
        {
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
    };
}
