#include "UnitTest.hh"
#include "nua.hh"

TEST_CASE(constructor)
{
    struct T
    {
        int x;
        std::string y;

        T(int p1, const std::string& p2)
            : x{p1}, y{p2}
        {}

        T() : x{0}, y{"nua"} {}            
    };
    
    nua::Context ctx;
    ctx["T"].setClass<T>("x", &T::x, "y", &T::y);
    ctx(R"(
    )");
}

TEST_CASE(cons_and_des)
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
        ctx["T"].setClass<T>();
        ctx(R"(
            function func(t1, t2)
                local tmp1 = t1
                local tmp2 = tmp1
                local tmp3 = t2
                return t1, t2
            end
        )");
        ctx["func"](t, T{}).get<T, T>();
    }
    TEST_CHECK(count == 0);
}
