#include "UnitTest.hh"
#include "nua/nua.hh"

TEST_CASE(simple_call)
{
    struct T
    {
        int add(int x, int y) const
        {
            return x + y;
        }
    };
    nua::Context ctx;
    ctx.setClass<T>("add", &T::add);
    ctx(R"(
        function add(t, x, y)
            local z = t
            return z:add(x, y)
        end
    )");
    TEST_CHECK(ctx["add"](T{}, 1, 2).get<int>() == 3);
}

TEST_CASE(return_reference)
{
    struct T
    {
        T& forward_ref(T& t)
        {
            return t;
        }
    };
    nua::Context ctx;
    ctx.setClass<T>("forward_ref", &T::forward_ref);
    ctx(R"(
        function apply(t, x)
            local z = t
            return z:forward_ref(x)
        end
    )");
    T t;
    T& r = ctx["apply"](t, std::ref(t)).get<T&>();
    TEST_CHECK(&r == &t);
}

TEST_CASE(multi_return_value_with_reference)
{
    struct T
    {
        std::tuple<T&, T&> swap(T& x, T& y)
        {
            return std::tuple<T&, T&>(y, x);
        }
    };

    nua::Context ctx;
    ctx.setClass<T>("swap", &T::swap);
    ctx(R"(
        function swap(t, x, y)
            return t:swap(x, y)
        end
    )");
    T x, y;
    auto tup = ctx["swap"](T{}, std::ref(x), std::ref(y)).get<T&, T&>();
    TEST_CHECK(&std::get<0>(tup) == &y);
    TEST_CHECK(&std::get<1>(tup) == &x);
}
