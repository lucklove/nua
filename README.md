#nua

Simple C++11 friendly header-only bindings to Lua  

A component of PM-Game-Server.   
Reference to [Selene](https://github.com/jeremyong/Selene) and fix some features to meet the PM-Game-Server's demand   

---

##依赖
- cmake 2.8+(可选, 如果不编译测试用例就不用)  
- lua 5.2+  
- 一个支持C++14的编译器(最好不要是clang, 因为用它过不了测试用例, 推荐gcc)  

---

##特性与用法  
###创建lua context  
```c++
nua::Context ctx;
```
如果在创建时不需要打开lua标准库:  
```c++
nua::Context ctx{false};
```  
当nua::Context析构时, 所有其关联的资源都会被回收(包括通过值传入的C++对象)   

###载入lua文件  
```c++
ctx.load("path/to/your.lua");
```

###直接执行lua代码  
```c++
ctx(R"(print("hello, world"))");
```

###向lua注册c++的函数  
``` c++
ctx["foo"] = []{ std::cout << "hello, nua" << std::endl; };     /**< lambda */
ctx["bar"] = std::function<...>(...);                           /**< stl function */
ctx["barz"] = std::bind(...);                                    
struct T
{
    type operator()(...)
    {
        ...
    }
} callable;

ctx["func"] = callable;                                         /**< callable object */
```  
之后你可以在lua中调用这些函数  

###在C++中调用lua的函数  
```c++
ctx["foo"]();                                                   /**< 没有返回值和参数 */
ctx["bar"](1, 2, "hello").get<int>();                           /**< 获取int返回值 */
int x;
std::string y;
std::tie(x, y) = ctx["barz"](1, 2, 3).get<int, std::string>();  /**< 接收多返回值 */
```

###注册C++的类  
```c++
struct T
{
    int v;
    int func(int y);
};
ctx.setClass<T>("v", &T::v, "func", &T::func);
```
然后可以在lua中这样访问这个类:
```lua
function bar(t)
    print(t:v())            --访问T::v
    t:set_v(123)            --设置T::v
    t:func(123)             --调用T::func
end
```
当然, t需要由C++传给lua：  
```c++
T t;
ctx["bar"](t);
```

###传递引用
通过setClass向nua注册过的类都可以传递引用  
```c++
struct T
{
    T(const T&) = delete;
} t;

ctx["foo"](std::ref(t));
ctx["bar"](std::cref(t));       /**< 当传递const ref时不会生成相应的set_xxx, 同时非const成员函数也会无效 */
```

###向lua返回引用  
```c++
ctx["foo"] = [&]() -> const T& { return t; };
```

###lua中利用C++的多态  
```c++
nua::Context ctx;

struct B
{
    virtual void apply() const
    {
        std::cout << "apply B" << std::endl;
    }
};

struct T : B
{
    void apply() const override
    {
        std::cout << "apply T" << std::endl;
    }
};

ctx.setClass<B>("apply", &B::apply);

T t;
B& rt = t;

ctx(R"(
    function apply(b)
        b:apply()
    end
)");
ctx["apply"](std::cref(rt));
```
