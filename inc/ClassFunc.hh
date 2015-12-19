#pragma once
#include "BaseFunc.hh"

namespace nua
{
    template <typename T, typename Ret, typename... Args>
    class ClassFunc : public BaseFunc
    {
    public:
        using func_t = std::function<Ret(T*, Args...)>;        
        
    private:
        func_t func_;
        std::string name_;
        std::string metatable_name_;

    public:
        ClassFunc(lua_State *l, const std::string &name, const std::string &metatable_name, func_t func)
            : func_{func}, name_{name}, metatable_name_{metatable_name}
        {
            lua_pushlightuserdata(l, (void *)static_cast<BaseFunc*>(this));
            lua_pushcclosure(l, &BaseFunc::dispatcher, 1);
            lua_setfield(l, -2, name.c_str());
        }
        
        int apply(lua_State* l) override
        {
            T* ptr = *(T**)luaL_checkudata(l, 1, metatable_name_.c_str());
            using decay_ret = typename std::decay<Ret>::type;
            std::function<decay_ret(Args...)> func = [this, ptr](Args... args) -> decay_ret
            {
                return func_(ptr, args...);
            };

            detail::apply_n(l, func, std::make_index_sequence<sizeof...(Args)>());
            return !std::is_same<decay_ret, void>::value;         
        }   
    };
}
