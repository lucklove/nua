#include "UnitTest.hh"
#include "nua/nua.hh"

TEST_CASE(primitives)
{
    struct T{};
    TEST_CHECK(nua::is_primitive<bool>::value);
    TEST_CHECK(nua::is_primitive<int>::value);
    TEST_CHECK(nua::is_primitive<double>::value);
    TEST_CHECK(nua::is_primitive<std::string>::value);
    TEST_CHECK(nua::is_primitive<std::nullptr_t>::value);
    TEST_CHECK(!nua::is_primitive<T>::value);
}
