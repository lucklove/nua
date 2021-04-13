#pragma once
/**
 * \need:
 *      utils.hh for function utils::apply_n
 *      BaseFunc.hh for class BaseFunc
 */

namespace nua
{
    template <typename T, typename Ret, typename... Args>
    class BaseClassFunc : public BaseFunc
    {
    protected:
        using func_t = std::function<Ret(T*, Args...)>;

        func_t func_;
        std::string name_;
        std::string metatable_name_;

    public:
        BaseClassFunc(lua_State *l, const std::string &name, const std::string &metatable_name, func_t func)
            : func_{func}, name_{name}, metatable_name_{metatable_name}
        {
            lua_pushlightuserdata(l, (void *)static_cast<BaseFunc*>(this));
            lua_pushcclosure(l, &BaseFunc::dispatcher, 1);
            lua_setfield(l, -2, name.c_str());
        }
    };

    template <typename T, typename Ret, typename... Args>
    class ClassFunc : public BaseClassFunc<T, Ret, Args...>
    {
    protected:
        int apply_impl(lua_State* l, T* ptr)
        {
            std::function<Ret(Args...)> func = [this, ptr](Args... args) -> Ret
            {
                return this->func_(ptr, args...);
            };

            utils::apply_n(l, func, std::make_index_sequence<sizeof...(Args)>());
            return !std::is_same<Ret, void>::value;
        }

    public:
        using BaseClassFunc<T, Ret, Args...>::BaseClassFunc;

        int apply(lua_State* l) override
        {
            T* ptr = (T*)luaL_checkudata(l, 1, this->metatable_name_.c_str());
            return apply_impl(l, ptr);
        }
    };

    template <typename T, typename Ret, typename... Args>
    struct ClassFunc<std::reference_wrapper<T>, Ret, Args...> : ClassFunc<T, Ret, Args...>
    {
        using ClassFunc<T, Ret, Args...>::ClassFunc;

        int apply(lua_State* l) override
        {
            T* ptr = *(T**)luaL_checkudata(l, 1, this->metatable_name_.c_str());
            return this->apply_impl(l, ptr);
        }
    };

    template <typename T, typename... Rets, typename... Args>
    struct ClassFunc<T, std::tuple<Rets...>, Args...> : BaseClassFunc<T, std::tuple<Rets...>, Args...>
    {
    protected:
        int apply_impl(lua_State* l, T* ptr)
        {
            std::function<std::tuple<Rets...>(Args...)> func = [this, ptr](Args... args) -> std::tuple<Rets...>
            {
                return this->func_(ptr, args...);
            };

            utils::apply_n(l,
                func,
                std::make_index_sequence<sizeof...(Rets)>(),
                std::make_index_sequence<sizeof...(Args)>());
            return sizeof...(Rets);
        }

    public:
        using BaseClassFunc<T, std::tuple<Rets...>, Args...>::BaseClassFunc;

        int apply(lua_State* l) override
        {
            T* ptr = (T*)luaL_checkudata(l, 1, this->metatable_name_.c_str());
            return apply_impl(l, ptr);
        }
    };

    template <typename T, typename... Rets, typename... Args>
    struct ClassFunc<std::reference_wrapper<T>, std::tuple<Rets...>, Args...>
        : ClassFunc<T, std::tuple<Rets...>, Args...>
    {
    public:
        using ClassFunc<T, std::tuple<Rets...>, Args...>::ClassFunc;

        int apply(lua_State* l) override
        {
            T* ptr = *(T**)luaL_checkudata(l, 1, this->metatable_name_.c_str());
            return this->apply_impl(l, ptr);
        }
    };
}
