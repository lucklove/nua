#include "UnitTest.hh"
#include "nua/nua.hh"

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

TEST_CASE(update_member)
{
    static nua::Context ctx;

    ctx(R"(
        function update(t)
            t:set_member(47)
        end
    )");

    struct T
    {
        int member = 0;

        void update()
        {
            ctx["update"](std::ref(*this));            
        }

        T() = default;
        T(const T&) = delete;
        T(T&&) = delete;
    };   

    ctx.setClass<T>("member", &T::member);
    T t;
    TEST_REQUIRE(t.member == 0);
    t.update();
    TEST_CHECK(t.member == 47);
}

TEST_CASE(recursive_user_type)
{
    nua::Context ctx;

    struct T
    {
        int member = 0;
    };

    struct R
    {
        T t;
    };

    ctx.setClass<R>("t", &R::t);
    ctx.setClass<T>("member", &T::member);
    ctx(R"(
        function apply(r)
            local t = r:t()
            t:set_member(47)
            r:set_t(t)
        end
    )");

    R r;
    TEST_REQUIRE(r.t.member == 0);
    ctx["apply"](std::ref(r));
    TEST_REQUIRE(r.t.member == 47);
}

TEST_CASE(virtual_class)
{
    nua::Context ctx;
    static bool b_applyed = false;
    static bool t_applyed = false;
    struct B
    {
        virtual void apply()
        {
            b_applyed = true;
        }
        virtual ~B() = default;
    };

    struct T : B
    {
        void apply() override
        {
            t_applyed = true;
        }
    } t;

    ctx.setClass<B>("apply", &B::apply);
    ctx.setClass<T>("apply", &T::apply);
    ctx(R"(
        function apply(b)
            b:apply()
        end
    )");
    B& rt = t;
    TEST_REQUIRE(!b_applyed && !t_applyed);
    ctx["apply"](rt);
    TEST_CHECK(b_applyed && !t_applyed);
    ctx["apply"](std::ref(rt));
    TEST_CHECK(b_applyed && t_applyed);
}

TEST_CASE(const_reference)
{
    nua::Context ctx;
    struct T
    {
        int v;
        T(const T&) = delete;
        T(int v) : v{v} {}
    } t{47};
    const T& ct = t;
    ctx.setClass<T>();
    ctx(R"(
        function apply(t)
            return t
        end
    )");
    const T& r = ctx["apply"](std::ref(t)).get<const T&>();
    const T& cr = ctx["apply"](std::ref(ct)).get<const T&>();
    TEST_CHECK(r.v == 47);
    TEST_CHECK(&r == &ct);
    TEST_CHECK(&cr == &ct);
}
