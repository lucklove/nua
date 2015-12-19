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

TEST_CASE(tuple_test)
{
    nua::Context ctx;
    ctx(R"(
        function test_tuple()
            return 233, "hello", "nua"
        end
    )");

    int num;
    std::string s1, s2;
    std::tie(num, s1, s2) = ctx["test_tuple"](1).get<int, std::string, std::string>();
    TEST_CHECK(num == 233);
    TEST_CHECK(s1 == "hello");
    TEST_CHECK(s2 == "nua");
}

TEST_CASE(store_test)
{
    nua::Context ctx;

    ctx["test_func"] = [](int x, int y) { return std::make_tuple(y, x); };

    ctx(R"(
        function run_test(x, y)
            return test_func(x, y)
        end
    )");

    int x, y;
    std::tie(x, y) = ctx["run_test"](3, 4).get<int, int>();
    TEST_CHECK(x == 4 && y == 3);
}

TEST_CASE(set_multi_class_with_member)
{
    nua::Context ctx;
    struct S
    {
        int vs;
    };

    struct T
    {
        int vt;
    };

    ctx["S"].setClass<S>("vs", &S::vs);
    ctx["T"].setClass<T>("vt", &T::vt);

    S s{1};
    T t{-1};

    ctx(R"(
        function exchange(s, t)
            local tmp = s:vs()
            s:set_vs(t:vt())
            t:set_vt(tmp)
            return true
        end
    )");

    TEST_REQUIRE(ctx["exchange"](s, t).get<bool>());
    TEST_CHECK(s.vs == -1);
    TEST_CHECK(t.vt == 1);
}
