#pragma once
/**
 * \need:
 *      MetatableRegistry.hh for class MetatableRegistry
 *      BaseFunc.hh for class BaseFunc
 *      ClassFunc.hh for template class ClassFunc
 *      Dtor.hh for template class Dtor
 */

namespace nua
{
    struct BaseClass
    {
        virtual ~BaseClass() = default;
    };

    template <typename T, typename R, typename... Members>
    class Class : public BaseClass
    {
    private:
        std::vector<std::unique_ptr<BaseFunc>> funcs_;
        std::string metatable_name_;
        std::unique_ptr<Dtor<R>> dtor_;

        void register_dtor(lua_State* l)
        {
            dtor_.reset(new Dtor<R>(l, metatable_name_.c_str()));
        }

        /** member */
        template <typename M, typename IsConst>
        void register_member(lua_State* l, const std::string& name, M R::*member, IsConst is_const)
        {
            register_member(l, name, member, is_const, typename std::is_const<M>::type{});
        }

        /** 类和成员都是const */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M R::*member, std::true_type, std::true_type)
        {
            std::function<M(R*)> lambda_get = [member](R *t)
            {
                return t->*member;
            };
            funcs_.push_back(std::make_unique<ClassFunc<T, M>>(l, name, metatable_name_, lambda_get));
        }

        /** 类是const, 成员不是const */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M R::*member, std::true_type, std::false_type)
        {
            register_member(l, name, member, std::true_type{}, std::true_type{});
        }

        /** 类不是const, 成员是const */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M R::*member, std::false_type, std::true_type)
        {
            register_member(l, name, member, std::true_type{}, std::true_type{});
        }

        /** 类和成员都不是const */
        template <typename M>
        void register_member(lua_State* l, const std::string& name, M R::*member, std::false_type, std::false_type)
        {
            register_member(l, name, member, std::true_type{});

            std::function<void(R*, M)> lambda_set  = [member](R *t, M v)
            {
                t->*member = v;
            };
            funcs_.push_back(std::make_unique<ClassFunc<T, void, M>>(l, "set_" + name, metatable_name_, lambda_set));
        }

        /** 类是const */
        template <typename Ret, typename... Args>
        void register_member(lua_State* l, const std::string& name, Ret(R::*func)(Args...), std::true_type)
        {
        }

        /** 类不是const */
        template <typename Ret, typename... Args>
        void register_member(lua_State* l, const std::string& name, Ret(R::*func)(Args...), std::false_type)
        {
            std::function<Ret(R*, Args...)> lambda = [func](R *t, Args... args) -> Ret
            {
                return (t->*func)(std::forward<Args>(args)...);
            };
            funcs_.push_back(std::make_unique<ClassFunc<T, Ret, Args...>>(l, name, metatable_name_, lambda));
        }

        template <typename Ret, typename... Args, typename IsConst>
        void register_member(lua_State* l, const std::string& name, Ret(R::*func)(Args...) const, IsConst)
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
            register_member(l, name, member, typename std::is_const<R>::type{});
            register_members(l, members...);
        }

        Class() = default;

    public:
        Class(lua_State* l, Members... members) : metatable_name_{typeid(T).name()}
        {
            metatable_name_ += "_lib";
            MetatableRegistry::push_new_metatable<T>(l, metatable_name_);
            if(std::is_same<T, R>::value)
                register_dtor(l);
            register_members(l, members...);
            lua_pushvalue(l, -1);
            lua_setfield(l, -1, "__index");
        }
    };
}
