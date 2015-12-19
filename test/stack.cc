#include "UnitTest.hh"
#include "ScopeGuard.hh"
#include "stack.hh"

TEST_CASE(get_bool)
{
    lua_State* l = luaL_newstate();
    TEST_REQUIRE(l);
    ScopeGuard on_exit([&l]{ lua_close(l); });
    lua_pushboolean(l, true);
    TEST_CHECK(nua::stack::get<bool>(l, -1) == true); 
    lua_pushboolean(l, false);
    TEST_CHECK(nua::stack::get<bool>(l, -1) == false); 
}

TEST_CASE(get_integer)
{
    lua_State* l = luaL_newstate();
    TEST_REQUIRE(l);
    ScopeGuard on_exit([&l]{ lua_close(l); });
    lua_pushinteger(l, 123);
    lua_pushinteger(l, 456);
    TEST_CHECK(nua::stack::get<int>(l, -2) == 123);
    TEST_CHECK(nua::stack::get<int>(l, -1) == 456);
}

TEST_CASE(get_lua_number)
{
    lua_State* l = luaL_newstate();
    TEST_REQUIRE(l);
    ScopeGuard on_exit([&l]{ lua_close(l); });
    lua_pushnumber(l, 3.1415926);
    TEST_CHECK(nua::stack::get<lua_Number>(l, -1) == 3.1415926);
}

TEST_CASE(get_string)
{
    lua_State* l = luaL_newstate();
    TEST_REQUIRE(l);
    ScopeGuard on_exit([&l]{ lua_close(l); });
    lua_pushstring(l, "hello, nua");
    TEST_CHECK(nua::stack::get<std::string>(l, -1) == "hello, nua"); 
}

TEST_CASE(push_get_userdata)
{
    lua_State* l = luaL_newstate();
    TEST_REQUIRE(l);
    ScopeGuard on_exit([&l]{ lua_close(l); });
    nua::MetatableRegistry::create(l);
    struct X { int x; } a{1};
    struct Y { int y; } b{2};
    struct Z { int z; } c{3};
    nua::MetatableRegistry::push_new_metatable<X>(l, "X");
    lua_pop(l, 1);
    nua::MetatableRegistry::push_new_metatable<Y>(l, "Y");
    lua_pop(l, 1);
    nua::MetatableRegistry::push_new_metatable<Z>(l, "Z");
    lua_pop(l, 1);
    nua::stack::push(l, a);
    nua::stack::push(l, b);
    nua::stack::push(l, c);

    X& ar = nua::stack::get<X&>(l, 1);
    Y& br = nua::stack::get<Y&>(l, 2);
    Z& cr = nua::stack::get<Z&>(l, 3);

    TEST_CHECK(&a == &ar);
    TEST_CHECK(&b == &br);
    TEST_CHECK(&c == &cr);
    TEST_CHECK(ar.x == 1);
    TEST_CHECK(br.y == 2);
    TEST_CHECK(cr.z == 3);
}

TEST_CASE(push_pop)
{
    lua_State* l = luaL_newstate();
    TEST_REQUIRE(l);
    ScopeGuard on_exit([&l]{ lua_close(l); });
    nua::MetatableRegistry::create(l);

    struct X { int x; } a{1};
    nua::MetatableRegistry::push_new_metatable<X>(l, "X");
    lua_pop(l, 1);

    nua::stack::push(l, 1);
    nua::stack::push(l, a);
    nua::stack::push(l, true);
    nua::stack::push(l, nullptr);
    
    /** clang bug, will fail with clang */
    TEST_CHECK(nua::stack::pop<std::nullptr_t>(l) == nullptr);
    TEST_CHECK(nua::stack::pop<bool>(l) == true);   
    auto x = nua::stack::pop<X>(l);
    TEST_CHECK(x.x == 1);
    TEST_CHECK(nua::stack::pop<int>(l) == 1);
}

TEST_CASE(should_throw_bad_cast)
{
    lua_State* l = luaL_newstate();
    TEST_REQUIRE(l);
    ScopeGuard on_exit([&l]{ lua_close(l); });

    bool flag = false;
    lua_pushstring(l, "nua");
    try
    {
        std::cout << "there should be an exception, just ignore it:" << std::endl;
        TEST_CHECK(nua::stack::get<int>(l, -1) && false, "you should not see this");
    }
    catch(std::bad_cast&)
    {
        flag = true;
    }
    TEST_CHECK(flag);
}
