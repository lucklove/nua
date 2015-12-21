#include "UnitTest.hh"
#include "nua.hh"

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
    ctx["T"].setClass<T>("add", &T::add);
    ctx(R"(
        function add(t, x, y)
            local z = t
            return z:add(x, y)
        end
    )");
    TEST_CHECK(ctx["add"](T{}, 1, 2).get<int>() == 3);
}
