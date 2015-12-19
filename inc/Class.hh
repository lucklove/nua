#pragma once
#include <lua.hpp>
#include "MetatableRegistry.hh"
#include "BaseFunc.hh"
#include "Func.hh"
#include "ClassFunc.hh"

namespace nua
{
    struct BaseClass
    {
        virtual ~BaseClass() = default;
    };

    template <typename T, typename... Members>
    class Class : public BaseClass
    {
    private:
        std::vector<std::unique_ptr<BaseFunc>> funcs_;
        std::string metatable_name_;

        void register_ctor(lua_State* l)
        {
            /** TODO: to be implemented */
        }

        void register_dtor(lua_State* l)
        {
            /** TODO: to be implemented */
        }

        /** member */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M T::*member)
        {
            register_member(l, name, member, typename std::is_const<M>::type{});
        }

        /** const member */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M T::*member, std::true_type)
        {
            std::function<M(T*)> lambda_get = [member](T *t) 
            {
                return t->*member;
            };
            funcs_.push_back(std::make_unique<ClassFunc<T, M>>(l, name, metatable_name_, lambda_get));
        }

        /** non-const member */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M T::*member, std::false_type)
        {
            register_member(l, name, member, std::true_type{});

            std::function<void(T*, M)> lambda_set  = [member](T *t, M v) 
            {
                t->*member = v;
            };
            funcs_.push_back(std::make_unique<ClassFunc<T, void, M>>(l, "set_" + name, metatable_name_, lambda_set));
        }

        template <typename Ret, typename... Args>
        void register_member(lua_State* l, const std::string& name, Ret(T::*fun)(Args...))
        {
std::cout << "Ret(T::*fun)(Args...)" << std::endl;
        }

        template <typename Ret, typename... Args>
        void register_member(lua_State* l, const std::string& name, Ret(T::*fun)(Args...) const)
        {
std::cout << "Ret(T::*fun)(Args...) const" << std::endl;
        }
        
        void register_members(lua_State* l)
        {
        }

        template <typename M, typename... Ms>
        void register_members(lua_State* l, const std::string& name, M member, Ms... members)
        {
            register_member(l, name, member);
            register_members(l, members...);                              
        }

    public:
        Class(lua_State* l, const std::string& name, Members... members)
        {
            metatable_name_ = name + "_lib";
            MetatableRegistry::push_new_metatable<T>(l, metatable_name_);
            register_ctor(l);
            register_dtor(l);
            register_members(l, members...);
            lua_pushvalue(l, -1);
            lua_setfield(l, -1, "__index");
        }
    };
}   
