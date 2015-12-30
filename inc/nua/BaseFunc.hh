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
            try
            {
                BaseFunc* func_ptr = (BaseFunc *)lua_touserdata(l, lua_upvalueindex(1));
                if(!func_ptr) 
                    throw std::runtime_error{"no function infomation found while try to make a call"};
                return func_ptr->apply(l);
            }
            catch(...)
            {
                ExceptionHolder::set();
            }

            lua_pushstring(l, "exception in dispatcher");
            return lua_error(l);
        }
    };
}
