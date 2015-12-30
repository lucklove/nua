#pragma once
/**
 * \need:
 *      utils.hh for function utils::apply_n
 *      BaseFunc.hh for class BaseFunc
 */

namespace nua
{
    template <typename Ret, typename... Args>
    struct TrivialFunction : BaseFunc
    {
    public:
        using func_t = std::function<Ret(Args...)>;

        TrivialFunction(lua_State* l, func_t func) : func_{func} 
        {
            lua_pushlightuserdata(l, (void *)static_cast<BaseFunc*>(this));
            lua_pushcclosure(l, &BaseFunc::dispatcher, 1);
        }

    protected:
        func_t func_;
    };

    template <typename Ret, typename... Args>
    struct Func : public TrivialFunction<Ret, Args...>
    {
        using TrivialFunction<Ret, Args...>::TrivialFunction;

        int apply(lua_State* l) override
        {
            utils::apply_n(l, 
                TrivialFunction<Ret, Args...>::func_, 
                std::make_index_sequence<sizeof...(Args)>());        
            return !std::is_same<Ret, void>::value;
        }        
    };

    template <typename... Rets, typename... Args>
    struct Func<std::tuple<Rets...>, Args...> : public TrivialFunction<std::tuple<Rets...>, Args...>
    {
        using TrivialFunction<std::tuple<Rets...>, Args...>::TrivialFunction;

        int apply(lua_State* l) override
        {
            utils::apply_n(l, 
                TrivialFunction<std::tuple<Rets...>, Args...>::func_, 
                std::make_index_sequence<sizeof...(Rets)>(),  
                std::make_index_sequence<sizeof...(Args)>()); 
            return sizeof...(Rets);
        }        
    };
}
