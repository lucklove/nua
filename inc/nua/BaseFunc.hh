#pragma once
/**
 * \need:
 *      ExceptionHolder.hh for class ExceptionHolder 
 */

namespace nua
{
    struct BaseFunc
    {
        virtual int apply(lua_State*) = 0;
        virtual ~BaseFunc() = default;

        static int dispatcher(lua_State *l) noexcept
        {
            int expected_type = LUA_TNONE;
            const char *wrong_meta_table = nullptr;
            int param_index = 0;

            try
            {
                BaseFunc* func_ptr = (BaseFunc *)lua_touserdata(l, lua_upvalueindex(1));
                if(!func_ptr) 
                    throw std::runtime_error{"no function infomation found while try to make a call"};
                return func_ptr->apply(l);
            }
            catch(NuaPrimitiveTypeError& e)
            {
                expected_type = e.expected_type;
                param_index = e.index;
            }
            catch(NuaUserDataTypeError& e)
            {
                wrong_meta_table = lua_pushlstring(l, e.metatable_name.c_str(), e.metatable_name.length());
                param_index = e.index;
            }
            catch(...)
            {
                lua_pushstring(l, "exception in dispatcher");
                ExceptionHolder::set();
            }

            if(expected_type != LUA_TNONE)
            {
                luaL_checktype(l, param_index, expected_type);
            }
            else if(wrong_meta_table)
            {
                luaL_checkudata(l, param_index, wrong_meta_table);
            }
 
            return lua_error(l);
        }
    };
}
