#pragma once
/**
 * \need:
 *      LuaRef.hh for class LuaRef
 *      ErrorHandler.hh for class ErrorHandler
 *      StackGuard.hh for class StackGuard
 */

namespace nua
{
    class function_base
    {
    public:
        function_base(LuaRefPtr ref_ptr, lua_State* l) : ref_ptr_{ref_ptr}, l_{l} {}

    protected:
        LuaRefPtr ref_ptr_;
        lua_State* l_;

        void protected_call(int num_args, int num_ret, int handler_index) const
        {
            int status = lua_pcall(l_, num_args, num_ret, handler_index);
            if(status != LUA_OK)
                ErrorHandler::handle(l_, status);
        }

        void push()
        {
            ref_ptr_->push();
        }
    };

    template <typename>
    class function {};

    template <typename Ret, typename... Args>
    struct function<Ret(Args...)> : function_base
    {
        using function_base::function_base;
        using function_base::push;

        Ret operator()(Args... args) const;
    };

    template <typename... Args>
    struct function<void(Args...)> : function_base
    {
        using function_base::function_base;
        using function_base::push;

        void operator()(Args... args) const;
    };

    template <typename... Rets, typename... Args>
    struct function<std::tuple<Rets...>(Args...)> : function_base
    {
        using function_base::function_base;
        using function_base::push;

        std::tuple<Rets...> operator()(Args... args) const;
    };
}

