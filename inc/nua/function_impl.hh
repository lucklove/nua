#pragma once
#include "function.hh"
#include "utils.hh"
#include "StackGuard.hh"
#include "ErrorHandler.hh"

namespace nua
{
    template <typename Ret, typename... Args>
    Ret function<Ret(Args...)>::operator()(Args... args)
    {
        StackGuard sg{l_};
        int handler_index = ErrorHandler::set_error_handler(l_);
        ref_ptr_->push();
        (void)std::initializer_list<int>{(stack::push(l_, args), 0)...};
        protected_call(sizeof...(Args), 1, handler_index);
        return stack::get<Ret>(l_, -1);
    }

    template <typename... Args>
    void function<void(Args...)>::operator()(Args... args)
    {
        StackGuard sg{l_};
        int handler_index = ErrorHandler::set_error_handler(l_);
        ref_ptr_->push();
        (void)std::initializer_list<int>{(stack::push(l_, args), 0)...};
        protected_call(sizeof...(Args), 0, handler_index);
    }

    template <typename... Rets, typename... Args>
    std::tuple<Rets...> function<std::tuple<Rets...>(Args...)>::operator()(Args... args)
    {
        StackGuard sg{l_};
        int handler_index = ErrorHandler::set_error_handler(l_);
        ref_ptr_->push();
        (void)std::initializer_list<int>{(stack::push(l_, args), 0)...};
        protected_call(sizeof...(Args), sizeof...(Rets), handler_index);
        lua_remove(l_, handler_index);
        return utils::get_n<Rets...>(l_, std::make_index_sequence<sizeof...(Rets)>{});
    }
}
