#include "UnitTest.hh"
#include "nua.hh"

TEST_CASE(selector_call)
{
    nua::Context ctx;
    ctx(R"(
        function test_add(a, b, c, d)
            sum = 0.0
            if(a) then
                sum = sum + a
            end
            if(b) then
                sum = sum + b
            end
            if(c) then
                sum = sum + c
            end
            if(d) then
                sum = sum + d
            end
            return math.ceil(sum)
        end
    )");
    TEST_CHECK(ctx["test_add"](1).get<int>() == 1);
    TEST_CHECK(ctx["test_add"](1, 2).get<int>() == 3);
    TEST_CHECK(ctx["test_add"](1, 2, 3).get<int>() == 6);
    TEST_CHECK(ctx["test_add"](1, 2, 3, 4).get<int>() == 10);
    TEST_CHECK(ctx["test_add"](1, 2, 3, 4, 5).get<int>() == 10);
}
