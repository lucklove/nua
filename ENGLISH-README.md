# nua

Simple C++11 friendly header-only bindings to Lua  

A component of [PM-Game-Server](https://github.com/lucklove/PM-Game-Server).   
Reference to [Selene](https://github.com/jeremyong/Selene) and fix some features to meet the PM-Game-Server's requirements   

---

## Requirements
- cmake 2.8+(This is not must if you don't want to compile tests)  
- lua 5.2+
- a compiler which support c++14(recommend gcc)  

---

## Features and Usage  
### Establishing lua context  
```c++
nua::Context ctx;
```
If you don't need standard Lua libraries:  
```c++
nua::Context ctx{false};
```
When a nua::Context destruct, the lua context is automatically destroyed in addition to all objects associated with it(including C++ objects). 

### Loading lua script from file  
```c++
ctx.load("path/to/your.lua");
```

### Executing lua code in c++ code    
```c++
ctx(R"(print("hello, world"))");
```

### Registering c++ function to lua  
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
Then you can call these functions in lua.    
### Calling lua function in c++  
```c++
ctx["foo"]();                                                   /**< no return, no parameter */
ctx["bar"](1, 2, "hello").get<int>();                           /**< get return value with type of int */
int x;
std::string y;
std::tie(x, y) = ctx["barz"](1, 2, 3).get<int, std::string>();  /**< get more than one return value */
```

### Registering Classes  
```c++
struct T
{
    int v;
    int func(int y);
};
ctx.setClass<T>("v", &T::v, "func", &T::func);
```
Then you can use this class in lua:
```lua
function bar(t)
    print(t:v())            --get T::v
    t:set_v(123)            --set T::v
    t:func(123)             --call T::func
end
```
You should not that t should be passed to lua by C++：  
```c++
T t;
ctx["bar"](t);
```

### Passing reference
Rigistered class can be passed to lua by reference  
```c++
struct T
{
    T(const T&) = delete;
} t;

ctx.setClass<T>();

ctx["foo"](std::ref(t));
ctx["bar"](std::cref(t));       /**< when you pass const reference, the set_xxx functions will be omited as well as non-const member function */
```

### Returning reference from c++  
```c++
ctx["foo"] = [&]() -> const T& { return t; };
```

### Accepting Lua functions as Arguments  
```lua
-- test.lua

function add(a, b)
    return a + b 
end

function pass_add(x, y)
    return take_fun_arg(add, x, y)
end
```

```c++
nua::Context ctx;
ctx["take_fun_arg"] = [](nua::function<int(int, int)> func, int a, int b)
{
    return func(a, b); 
};

ctx.load("test.lua");
assert(ctx["pass_add"](3, 5) == 8);
```

### Making use of c++ polymorphic functions  
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

## Note
You can *not* 
- pass reference of primitive type to nua
- return reference of primitive type in the function you registered to nua
- use non-const reference of primitive type as args in the function you registered to nua
- return reference of primitive type from lua to c++  
  
The primitive types include:
- all number type in c++(bool, char, unsigned char, int, etc).
- std::string
- std::nullptr_t
- nua::function  
  
```c++
ctx(R"(
    function foo(x)
    end 
)");
int x = 0;
ctx["foo"](x);             /**< ok */  
ctx["foo"](std::ref(x));   /**< runtime error, x is of primitive type */  
ctx["foo"](std::cref(x));  /**< runtime error, x is of primitive type */  
```
  
Note that std::string is also of primitive type:
```c++
ctx["foo"] = [](const std::string& s) {...};        /**< ok, receive const reference of primitive type */  
ctx["foo"] = []() -> const std::string& {...};      /**< runtime error, return reference of primitive type */  
```
  
An object can be return from lua to c++ as reference, only if it was passed to lua as reference before, and the const-qualiﬁers must be correct.  
  
```c++
struct T
{
};

ctx.setClass<T>();
ctx(R"(
    function forward_ref(x)
        return x
    end
)");

T t;
ctx["forward_ref"](t).get<const T&>();                  /**< runtime error, expect a reference of a non-reference object */
ctx["forward_ref"](t).get<T&>();                        /**< runtime error, expect a reference of a non-reference object */
ctx["forward_ref"](std::ref(t)).get<const T&>();        /**< ok, non-const reference can be translate to its' const version */
ctx["forward_ref"](std::cref(t)).get<const T&>();       /**< ok */
ctx["forward_ref"](std::cref(t)).get<T&>();             /**< runtime error, const reference can't be translate to non-const one */
```
