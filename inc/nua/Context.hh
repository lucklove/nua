#pragma once
#include <stdexcept>
#include <lua.hpp>
#include <string>
#include "stack.hh"
#include "ErrorHandler.hh"
#include "Selector.hh"
#include "Registry.hh"

namespace nua
{
    class Context
    {
    private:
        lua_State* lua_ctx_;
        std::unique_ptr<Registry> registry_;

    public:
        explicit Context(bool should_open_libs = true)
        {
            lua_ctx_ = luaL_newstate();
            if(lua_ctx_ == nullptr)
                throw std::runtime_error{"initialize lua context failed"};

            ErrorHandler::set_atpanic(lua_ctx_);

            registry_ = std::make_unique<Registry>(lua_ctx_); 
 
            if(should_open_libs)
                luaL_openlibs(lua_ctx_);
        }

        ~Context()
        {
            forceGC();
            lua_close(lua_ctx_);
        }

        Context(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(const Context&) = delete;
        Context& operator=(Context&&) = delete;

        void operator()(const char* code)
        {
            stack::StackGuard sg{lua_ctx_};
            
            int status = luaL_dostring(lua_ctx_, code);
            if(status)
                ErrorHandler::handle(lua_ctx_, status);
        }

        Selector operator[](const std::string& name) 
        {
            return Selector(lua_ctx_, registry_.get(), name);
        }

        void load(const std::string& file)
        {
            stack::StackGuard sg{lua_ctx_};
           
            int status = luaL_loadfile(lua_ctx_, file.c_str()); 
            if(status)
                ErrorHandler::handle(lua_ctx_, status);     /**< handle is noreturn */

            status = lua_pcall(lua_ctx_, 0, LUA_MULTRET, 0);
            if(status)
                ErrorHandler::handle(lua_ctx_, status);
        }

        void forceGC()
        {
            lua_gc(lua_ctx_, LUA_GCCOLLECT, 0);
        }

        template <typename T, typename... Funcs>
        typename std::enable_if<!is_primitive<T>::value, void>::type
        setClass(Funcs... funcs)
        {
            registry_->registerClass<T>(lua_ctx_, funcs...);   
            registry_->registerClass<std::reference_wrapper<T>>(lua_ctx_, funcs...);   
            registry_->registerClass<std::reference_wrapper<const T>>(lua_ctx_, funcs...);   
        }
    };
}
