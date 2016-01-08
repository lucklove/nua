#include "UnitTest.hh"
#include "nua/nua.hh"

TEST_CASE(user_type)
{
    struct T {};
    struct F {}; 

    nua::Context ctx;
    ctx.setClass<T>();
    ctx.setClass<F>();

    ctx["foo"] = [](T){};

    bool f = false;
    try
    {
        ctx["foo"](F{}).get();
        TEST_REQUIRE(false, "期待一个异常，但是没有检测到");
    }
    catch(...)
    {
        f = true;
    }
    TEST_CHECK(f, "异常测试失败");

    ctx(R"(
        function bar()
            return 123
        end
    )");

    f = false;
    try
    {
        ctx["bar"]().get<T>();
        TEST_REQUIRE(false, "期待一个异常，但是没有检测到");
    }
    catch(...)
    {
        f = true;
    }
    TEST_CHECK(f, "异常测试失败");

    f = false;
    try
    {
        ctx(R"(
            function apply(x)
                foo(x)
            end
        )");
        ctx["apply"](F{});
        TEST_REQUIRE(false, "期待一个异常，但是没有检测到");
    }
    catch(...)
    {
        f = true;
    }
    TEST_CHECK(f, "异常测试失败");
}

TEST_CASE(primitive_type)
{
    nua::Context ctx;
    ctx["foo"] = [](bool){};
   
    bool f = false; 
    try
    {
        ctx["foo"]("error"); 
        TEST_REQUIRE(false, "期待一个异常，但是没有检测到");
    }
    catch(...)
    {
        f = true;
    }
    TEST_CHECK(f, "异常测试失败");

    ctx["bar"] = [](std::nullptr_t){};
    f = false;
    try
    {
        ctx(R"(
            bar("error");
        )");
        TEST_REQUIRE(false, "期待一个异常，但是没有检测到");
    }
    catch(...)
    {
        f = true;
    }
    TEST_CHECK(f, "异常测试失败");
}
