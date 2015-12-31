#pragma once
/**
 * \need:
 *      MetatableRegistry.hh for class MetatableRegistry
 *      primitives.hh for template class is_primitive
 *      types.hh for template function check::is_type
 *      ScopeGuard.hh for class ScopeGuard
 *      ErrorHandler.hh for class ErrorHandler
 *      LuaRef.hh for class LuaRef
 *      function.hh for template class function
 */

namespace nua
{
    namespace stack
    {
        template <typename T>
        typename std::enable_if<std::is_copy_constructible<T>::value && !is_primitive<T>::value, void>::type 
        push(lua_State* l, const T& t)
        {
            T* ptr = (T*)lua_newuserdata(l, sizeof(T));
            MetatableRegistry::set_metatable<T>(l);
            new(ptr) T{t};
        }
    
        template <typename T>
        void push(lua_State* l, std::reference_wrapper<T> t)
        {
            T** ptr = (T **)lua_newuserdata(l, sizeof(T *));
            MetatableRegistry::set_metatable<std::reference_wrapper<T>>(l);
            *ptr = &t.get();    
        }
    
        inline void push(lua_State* l, bool v)
        {
            lua_pushboolean(l, v);
        }
    
        inline void push(lua_State* l, int v)
        {
            lua_pushinteger(l, v);
        }
   
        inline void push(lua_State* l, lua_Number v)
        {
            lua_pushnumber(l, v);
        }
    
        inline void push(lua_State* l, const std::string& v)
        {
            lua_pushlstring(l, v.c_str(), v.size());   
        }
    
        inline void push(lua_State* l, const char* v)
        {
            lua_pushstring(l, v);   
        }
    
        inline void push(lua_State* l, std::nullptr_t)
        {
            lua_pushnil(l);    
        }
    
        template <typename R, typename... Args>
        void push(lua_State *l, nua::function<R(Args...)> func)
        {
            func.push_to_stack();
        }
    
        template <typename T>
        void set(lua_State* l, int index, T&& v)
        {
            push(l, std::forward<T>(v));
            lua_replace(l, index);   
        }
            
        template <typename T>
        typename std::enable_if<!is_primitive<T>::value, T>::type
        get(lua_State* l, int index)
        {
            static_assert(!std::is_rvalue_reference<T>::value, "not support rvalue reference");
            using value_t = typename std::remove_reference<T>::type;
            if(std::is_lvalue_reference<T>::value 
                && !(check::is_type<std::reference_wrapper<value_t>>(l, index) 
                    || check::is_type<std::reference_wrapper<typename std::remove_cv<value_t>::type>>(l, index)))
                goto fail;
    
            if(check::is_type<value_t>(l, index))
            {
                value_t* ptr = (value_t*)lua_touserdata(l, index);
                if(ptr == nullptr) throw std::runtime_error{"get userdata failed"};
                return *ptr;   
            }
            else if(check::is_type<std::reference_wrapper<value_t>>(l, index)
                || check::is_type<std::reference_wrapper<typename std::remove_cv<value_t>::type>>(l, index))
            {
                value_t** ptr = (value_t**)lua_touserdata(l, index);
                if(ptr == nullptr) throw std::runtime_error{"get userdata failed"};
                return **ptr;   
            }
    
            fail:
            std::string metatable_name = MetatableRegistry::get_typename<T>(l);
            ErrorHandler::set_atpanic(l);
            luaL_checkudata(l, index, metatable_name.c_str());
            throw "should not reach here";
        }
    
        template <typename T>
        typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, T>::type
        get(lua_State* l, int index)
        {
            if(lua_isboolean(l, index))
            {
                return lua_toboolean(l, index);
            }
            else if(lua_isinteger(l, index))
            {
                return lua_tointeger(l, index);
            }
    
            ErrorHandler::set_atpanic(l);
            return luaL_checknumber(l, index);
        }
    
        template <typename T>
        typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type
        get(lua_State* l, int index)
        {
            ErrorHandler::set_atpanic(l);
            return luaL_checkstring(l, index);
        }
    
        template <typename T>
        typename std::enable_if<std::is_same<T, std::nullptr_t>::value, std::nullptr_t>::type
        get(lua_State* l, int index)
        {
            if(!lua_isnil(l, index))
            {
                ErrorHandler::set_atpanic(l);
                luaL_checktype(l, index, LUA_TNIL);
                throw "should not reach here";
            }
    
            return nullptr;
        }
    
        template <typename T>
        typename std::enable_if<std::is_base_of<nua::function_base, T>::value, T>::type
        get(lua_State* l, int index)
        {
            lua_pushvalue(l, index);
            return T{std::make_shared<nua::LuaRef>(l, luaL_ref(l, LUA_REGISTRYINDEX)), l};
        }
    
        template <typename T>
        T pop(lua_State* l)
        {
            ScopeGuard pop_on_exit([l]
            {
                lua_pop(l, 1);
            });
    
            return get<T>(l, -1);
        } 
    }
}       /**< nua */
