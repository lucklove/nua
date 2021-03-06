#include "UnitTest.hh"
#include "nua/nua.hh"

TEST_CASE(cons_and_des_check)
{
    static size_t count;    /**< 用来检测是否内存泄漏 */

    struct T
    {
        T()
        {
            ++count;
        }
        
        T(const T&)
        {
            ++count;
        }

        ~T()
        {
            --count;
        }
    };
    {
        T t;
        nua::Context ctx;
        ctx.setClass<T>();
        ctx(R"(
            function func(t1, t2)
                local tmp1 = t1
                local tmp2 = tmp1
                local tmp3 = t2
                return tmp2, tmp3
            end
        )");
        auto tup = ctx["func"](std::ref(t), T{}).get<T&, T>();
        TEST_CHECK(&std::get<0>(tup) == &t);
    }
    TEST_CHECK(count == 0);
}
