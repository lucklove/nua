#include "UnitTest.hh"
#include "nua.hh"

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

    ctx.setClass<S>("vs", &S::vs);
    ctx.setClass<T>("vt", &T::vt);

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

    TEST_REQUIRE(ctx["exchange"](std::ref(s), std::ref(t)).get<bool>());
    TEST_CHECK(s.vs == -1);
    TEST_CHECK(t.vt == 1);
}
