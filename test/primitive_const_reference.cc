#include "UnitTest.hh"
#include "nua/nua.hh"

TEST_CASE(const_string_reference)
{
    nua::Context ctx; 
    ctx["test"] = [](const std::string& s) { TEST_CHECK(s == "nua"); };
    ctx["test"]("nua");
}

TEST_CASE(const_function_reference)
{
    nua::Context ctx;

    ctx["take_fun_arg"] = [](const nua::function<int(int, int)>& func, int a, int b) 
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

TEST_CASE(const_number_reference)
{
    nua::Context ctx; 
    ctx["test_int"] = [](const int& v) { TEST_CHECK(v == 47); };
    ctx["test_int"](47);
    ctx["test_long"] = [](const long& v) { TEST_CHECK(v == 233); };
    ctx["test_long"](233);
    ctx["test_float"] = [](const float& v) { TEST_CHECK(v == 2.5); };
    ctx["test_float"](2.5);
    ctx["test_double"] = [](const double& v) { TEST_CHECK(v == 3.14159); };
    ctx["test_double"](3.14159);
}

TEST_CASE(const_nullptr_type)
{
    nua::Context ctx; 
    ctx["test"] = [](const std::nullptr_t& v) { TEST_CHECK(v == nullptr); };
    ctx["test"](nullptr);
}
