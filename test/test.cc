#define TEST_MAIN
#include "UnitTest.hh"
#include "nua.hh"

TEST_CASE(nua_test)
{
    nua::Context ctx{true};
    ctx("print(\"hello nua!\")");
    ctx.load("build/test.lua");
}
