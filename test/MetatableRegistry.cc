#include "UnitTest.hh"
#include "MetatableRegistry.hh"
#include "ScopeGuard.hh"

#include "types.hh"
TEST_CASE(get_typename)
{
    lua_State* l = luaL_newstate();
    TEST_REQUIRE(l);
    nua::ScopeGuard on_exit([&l]{ lua_close(l); });
    nua::MetatableRegistry::create(l);
    struct T {} t;
    nua::MetatableRegistry::push_new_metatable<T>(l, "T");
    lua_pushlightuserdata(l, &t);
    nua::MetatableRegistry::set_metatable<T>(l);
    TEST_CHECK(nua::MetatableRegistry::get_typename(l, -1) == "T");
    TEST_CHECK(nua::MetatableRegistry::get_typename<T>(l) == "T");
    lua_pushinteger(l, 233);
    TEST_CHECK(nua::MetatableRegistry::get_typename(l, -1) == "number");
}
