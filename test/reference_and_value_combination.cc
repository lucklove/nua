#include "UnitTest.hh"
#include "nua/nua.hh"

struct T
{
};

void useVal(T)
{
    /** nop */
}

void useRef(T&)
{
    /** nop */
}

T make()
{
    return T{};
}

T& global()
{
    static T g;
    return g;
}

TEST_CASE(value_and_reference_combination)
{
    nua::Context ctx;
    ctx.setClass<T>();

    ctx["useVal"] = useVal;
    ctx["useRef"] = useRef;
    ctx["make"] = make;
    ctx["global"] = global;

    ctx(R"(
        useVal(make())
        useVal(global())
        -- useRef(make())                --this will throw runtime error.
        useRef(global())
    )");
}
