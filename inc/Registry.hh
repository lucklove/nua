#pragma once
#include <lua.hpp>
#include <memory>
#include "MetatableRegistry.hh"
#include "Func.hh"
#include "Class.hh"

namespace nua
{
    class Registry
    {
    private:
        lua_State* ctx_;
        std::vector<std::unique_ptr<BaseFunc>> funcs_;
        std::vector<std::unique_ptr<BaseClass>> classes_;
        
    public:
        Registry(lua_State* ctx) : ctx_(ctx) 
        {
            MetatableRegistry::create(ctx_);
        }

        template <typename Ret, typename... Args>
        void registerFunction(lua_State* l, std::function<Ret(Args...)> func)
        {
            funcs_.push_back(std::make_unique<Func<Ret, Args...>>(l, func));
        }

        template <typename T, typename... Args, typename... Funcs>
        void registerClass(lua_State*l, const std::string& name, Funcs... funcs)
        {
            classes_.push_back(std::make_unique<Class<T, Funcs...>>(l, name, funcs...));
        }
    };
}
