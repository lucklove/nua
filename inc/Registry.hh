#pragma once
#include <lua.hpp>
#include <memory>
#include "MetatableRegistry.hh"
#include "Func.hh"

namespace nua
{
    class Registry
    {
    private:
        lua_State* ctx_;
        std::vector<std::unique_ptr<BaseFunc>> funcs_;

    public:
        Registry(lua_State* ctx) : ctx_(ctx) 
        {
            MetatableRegistry::create(ctx_);
        }

        template <typename Ret, typename... Args>
        BaseFunc* registerFunction(std::function<Ret(Args...)> func)
        {
            funcs_.push_back(std::make_unique<Func<Ret, Args...>>(func));
            return funcs_.back().get();
        }
    };
}
