#include "UnitTest.hh"
#include "nua/nua.hh"

TEST_CASE(return_none)
{
    nua::Context ctx;

    ctx(R"(
        function foo()
            return function(n)
                assert(n == 47)
            end
        end
    )");

    auto f = ctx["foo"]().get<nua::function<void(int)>>();
    f(47);
}

TEST_CASE(return_one)
{
    nua::Context ctx;
    ctx(R"(
        function foo()
            return function(any)
                return any
            end
        end
    )");
    auto fi = ctx["foo"]().get<nua::function<int(int)>>();
    TEST_CHECK(fi(11) == 11);
    auto fs = ctx["foo"]().get<nua::function<std::string(std::string)>>();
    TEST_CHECK(fs("nua") == "nua");
}

TEST_CASE(return_multi)
{
    nua::Context ctx;
    ctx(R"(
        function foo()
            return function(a, b, c)
                return a, b, c
            end
        end
    )");
    auto f = ctx["foo"]().get<nua::function<std::tuple<int, int, int>(int, int, int)>>();
    auto t = f(1, 2, 3);
    TEST_CHECK(std::get<0>(t) == 1);
    TEST_CHECK(std::get<1>(t) == 2);
    TEST_CHECK(std::get<2>(t) == 3);
}

TEST_CASE(as_arg)
{
    nua::Context ctx;

    ctx["take_fun_arg"] = [](nua::function<int(int, int)> func, int a, int b) 
    {
        return func(a, b);
    };

    ctx(R"(
        function add(a, b)
            return a + b
        end

        function pass_add(x, y)
            return take_fun_arg(add, x, y)
        end
    )");

    TEST_CHECK(ctx["pass_add"](3, 5).get<int>() == 8);
}

TEST_CASE(closure)
{
    nua::Context ctx;
    ctx(R"(
        function add_this(x)
            return function(y)
                return x + y
            end
        end       
    )");
    auto f = ctx["add_this"](5).get<nua::function<int(int)>>();
    TEST_CHECK(f(6) == 11);
}
