#pragma once
#include <lua.hpp>
#include "MetatableRegistry.hh"
#include "BaseFunc.hh"
#include "Func.hh"
#include "ClassFunc.hh"
#include "Ctor.hh"
#include "Dtor.hh"

namespace nua
{
    struct BaseClass
    {
        virtual ~BaseClass() = default;
    };

    template <typename T, typename R, typename A, typename... Members>
    class Class : public BaseClass
    {
    private:
        std::vector<std::unique_ptr<BaseFunc>> funcs_;
        std::string metatable_name_;
        std::unique_ptr<A> ctor_;
        std::unique_ptr<Dtor<R>> dtor_;

        void register_ctor(lua_State* l)
        {
            ctor_.reset(new A(l, metatable_name_.c_str()));
        }

        void register_dtor(lua_State* l)
        {
            dtor_.reset(new Dtor<R>(l, metatable_name_.c_str()));
        }

        /** member */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M R::*member)
        {
            register_member(l, name, member, typename std::is_const<M>::type{});
        }

        /** const member */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M R::*member, std::true_type)
        {
            std::function<M(R*)> lambda_get = [member](R *t) 
            {
                return t->*member;
            };
            funcs_.push_back(std::make_unique<ClassFunc<T, M>>(l, name, metatable_name_, lambda_get));
        }

        /** non-const member */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M R::*member, std::false_type)
        {
            register_member(l, name, member, std::true_type{});

            std::function<void(R*, M)> lambda_set  = [member](R *t, M v) 
            {
                t->*member = v;
            };
            funcs_.push_back(std::make_unique<ClassFunc<T, void, M>>(l, "set_" + name, metatable_name_, lambda_set));
        }

        template <typename Ret, typename... Args>
        void register_member(lua_State* l, const std::string& name, Ret(R::*func)(Args...))
        {
            std::function<Ret(R*, Args...)> lambda = [func](R *t, Args... args) -> Ret 
            {
                return (t->*func)(std::forward<Args>(args)...);      
            };
            funcs_.push_back(std::make_unique<ClassFunc<T, Ret, Args...>>(l, name, metatable_name_, lambda));   
        }

        template <typename Ret, typename... Args>
        void register_member(lua_State* l, const std::string& name, Ret(R::*func)(Args...) const)
        {
            std::function<Ret(const R*, Args...)> lambda = [func](const R* t, Args... args)
            {
                return (t->*func)(std::forward<Args>(args)...);      
            };
            funcs_.push_back(std::make_unique<ClassFunc<T, Ret, Args...>>(l, name, metatable_name_, lambda));   
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

        Class() = default;

    public:
        Class(lua_State* l, const std::string& name, Members... members)
        {
            metatable_name_ = name + "_lib";
            MetatableRegistry::push_new_metatable<T>(l, metatable_name_);
            if(std::is_same<T, R>::value)
            {
                register_ctor(l);
                register_dtor(l);
            }
            register_members(l, members...);
            lua_pushvalue(l, -1);
            lua_setfield(l, -1, "__index");
        }
    };
}   
