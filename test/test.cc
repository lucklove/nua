#define TEST_MAIN
#include "UnitTest.hh"
#include "nua/nua.hh"

TEST_CASE(test)
{
    nua::Context st{true};
    struct Obj2
    { void print() { std::cout << "obj2" << std::endl; } };
    struct Obj
    {
        Obj2& get_obj2()
        {return obj2; }
        void print()
        { std::cout << "obj1" << std::endl; }
        Obj2 obj2;
    };

    st.setClass<Obj>("obj2", &Obj::get_obj2, "print", &Obj::print);
    st.setClass<Obj2>("print", &Obj2::print);
    st(R"(
        function test(cpp_obj)
            cpp_obj:print()
            local cpp_obj2 = cpp_obj:obj2()
            cpp_obj:print()
            cpp_obj2:print()
        end
    )");
    Obj o;
    st["test"](o);
}
