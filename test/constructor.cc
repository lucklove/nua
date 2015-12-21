#include "UnitTest.hh"
#include "nua.hh"

TEST_CASE(cons_and_des)
{
    static size_t count = 0;       /**< 检查是否存在资源泄漏 */
    struct T
    {
        T()
        {
            ++count;
        }

        T(const T& t)
        {
            v = t.v;
            ++count;
        }

        ~T()
        {
            --count;
        }
        
        int v{3};
    };
    {
        nua::Context ctx;
        ctx["T"].setClass<T>("v", &T::v);
        T t;
        ctx["x"] = t;
        ctx(R"(
            print(x:v())
            x:set_v(1)
            print(x:v())
        )");
        std::cout << t.v << std::endl;
        ctx["v"] = 9;
        std::cout << ctx["v"].get<int>() << std::endl;
    }
    TEST_CHECK(count == 0);
}
