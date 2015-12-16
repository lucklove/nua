#pragma once
#include <lua.hpp>
#include <string>
#include <vector>
#include "Ref.hh"

namespace nua
{
    class Selector
    {
    private:
        lua_State* l_;
        std::string name_;
        RefPtr key_;

        std::vector<RefPtr> functor_arguments_;
        bool functor_active_ = false;

        void evaluate_retrieve(int num_results)
        {
            lua_pushglobaltable(l_);
            key_->push();
            lua_gettable(l_, -2);
            lua_remove(l_, lua_absindex(l_, -2));
            evaluate_function_call(num_results);
        }
        
        void evaluate_function_call(int num_results)
        {
            if(!functor_active_)
                return;
            functor_active_ = false;
    
            int handler_index = ErrorHandler::set_error_handler(l_);
            int func_index = handler_index - 1;
            lua_pushvalue(l_, func_index);
            lua_copy(l_, handler_index, func_index);
            lua_replace(l_, handler_index);
            
            for(const RefPtr& arg : functor_arguments_)
                arg->push();

            int status = lua_pcall(l_, functor_arguments_.size(), num_results, handler_index - 1);
            lua_remove(l_, handler_index - 1);
            
            if(status != LUA_OK)
                ErrorHandler::handle(l_, status);
        }

    public:
        Selector(lua_State* l, const std::string& name)
            : l_{l}, name_{name}, key_{make_ref(l, name)}
        {}

        Selector(const Selector& other) = default;

        template <typename... Args>
        Selector operator()(Args&&... args)
        {
            Selector copy{*this};
            copy.functor_arguments_ = make_refs(l_, std::forward<Args>(args)...);
            copy.functor_active_ = true;
            return copy;
        }

        template <typename T>
        T get()
        {
            ScopeGuard reset_stack_on_exit([this, saved_top_index = lua_gettop(l_)]
            {
                lua_settop(l_, saved_top_index);
            });
            evaluate_retrieve(1);
            return stack::pop<T>(l_);
        }
/*
        template <typename... Args>
        std::tuple<Args...> get()
        {
        }
*/
    };
}
