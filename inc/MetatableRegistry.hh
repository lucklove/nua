#pragma once
#include <lua.hpp>
#include <string>
#include <typeindex>
#include <cstring>
#include "primitives.hh"

namespace nua
{
    struct MetatableRegistry
    {
    private:
        static void create_table(lua_State *ctx, const std::string& name)
        {
            lua_pushlstring(ctx, name.c_str(), name.size());
            lua_newtable(ctx);
            lua_settable(ctx, LUA_REGISTRYINDEX);
        }

        static void push_names_table(lua_State* ctx)
        {
            lua_pushliteral(ctx, "nua_metatable_names");
            lua_gettable(ctx, LUA_REGISTRYINDEX);
        }

        static void push_metatable(lua_State* ctx)
        {
            lua_pushliteral(ctx, "nua_metatables");
            lua_gettable(ctx, LUA_REGISTRYINDEX);
        }

        template <typename T>
        static void push_typeinfo(lua_State* ctx)
        {
            lua_pushlightuserdata(ctx, reinterpret_cast<void *>(typeid(T).hash_code()));
        }

    public:
        template <typename T>
        static void get_metatable(lua_State* ctx)
        {
            push_metatable(ctx);
            push_typeinfo<T>(ctx);
            lua_gettable(ctx, -2);
            lua_remove(ctx, -2);
        }

        static void create(lua_State *ctx) 
        {
            create_table(ctx, "nua_metatable_names");   
            create_table(ctx, "nua_metatables");   
        }

        template <typename T>
        static void set_metatable(lua_State* ctx)
        {
            std::string name = get_typename<T>(ctx);
            if(name == "unregistered type")
                throw std::logic_error{"try to use a unregistered type"};
            
            luaL_setmetatable(ctx, name.c_str());
        }

        template <typename T>
        static void push_new_metatable(lua_State* ctx, const std::string& name)
        {
            push_names_table(ctx);
            push_typeinfo<T>(ctx);
            lua_pushlstring(ctx, name.c_str(), name.size());
            lua_settable(ctx, -3);
            lua_pop(ctx, 1);
            
            luaL_newmetatable(ctx, name.c_str());
            push_metatable(ctx);
            push_typeinfo<T>(ctx);
            lua_pushvalue(ctx, -3);
            lua_settable(ctx, -3);
            lua_pop(ctx, 1);    /**< XXX: 这里应该pop 2? */
        }

        template <typename T>
        static std::string get_typename(lua_State* ctx)
        {
            std::string name("unregistered type");
            
            push_names_table(ctx);
            push_typeinfo<T>(ctx);
            lua_gettable(ctx, -2);
            
            if(lua_isstring(ctx, -1)) 
            {
                size_t len = 0;
                char const * str = lua_tolstring(ctx, -1, &len);
                name.assign(str, len);
            }
            
            lua_pop(ctx, 2);
            return name;
        }

        static std::string get_typename(lua_State* ctx, int index)
        {
            std::string name;
            
            if(lua_getmetatable(ctx, index)) 
            {
                lua_pushliteral(ctx, "__name");
                lua_gettable(ctx, -2);
                
                if(lua_isstring(ctx, -1))
                {
                    size_t len = 0;
                    char const * str = lua_tolstring(ctx, -1, &len);
                    name.assign(str, len);
                }
                
                lua_pop(ctx, 2);
            }

            if(name.empty()) 
                name = lua_typename(ctx, lua_type(ctx, index));

            return name;
        }
    };
}
