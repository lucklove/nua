#pragma once
/**
 * \need:
 *      ExceptionHolder.hh for class ExceptionHolder
 */

namespace nua
{
    struct ErrorHandler
    {
    private:
        static int traceback(lua_State* l) noexcept
        {
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

        [[noreturn]]
        static int atpanic(lua_State* l)
        {
            const char *reason = "<unknown panic reason>";
            if(!lua_isnil(l, -1))
            {
                reason = lua_tostring(l, -1);
                if(reason == nullptr)
                    reason = "<unknown panic raason>";
            }

            lua_pop(l, lua_gettop(l));
            throw std::runtime_error{reason};
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
            lua_pushcfunction(l, traceback);
            return lua_gettop(l);
        }

        static lua_CFunction set_atpanic(lua_State* l)
        {
            return lua_atpanic(l, atpanic);
        }
    };
}
