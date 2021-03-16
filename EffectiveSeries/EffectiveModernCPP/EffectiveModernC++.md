# Effective Modern C++

## Deducint Types
+ Item 1: Understand template type deduction
+ Item 2: Understand auto type deduction
+ Item 3: Understand decltyp
+ Item 4: Know how to view deduced types.

### Item 1: Understand template type deduction

模板函数形式如下:
```cpp
template<typename T>
void f(ParamType param);

f(expr);
```

三种情况:
1. ```ParamType```是个```非通用的```引用或者一个指针(```void f(const T& param) or void f(T* param)```, 与```const```无关)
   1. If ```expr```'s type is a reference, ignore the reference part.
   2. Then pattern-match ```expr's type``` against ParamType to determine T.
2. ```ParamType```是个通用的引用(```void f(T&& param)```)
   1. If expr is an lvalue, both T and ParamType are deduced to be lvalue references.
   2. If expr is an rvalue, the “normal” (i.e., Case 1) rules apply.
3. ```ParamType```既不是指针也不是引用```void f(T param)```, That means that param will be a copy of whatever is passed ```in—a completely new object```.
   1. As before, if expr’s type is a reference, ignore the reference part.
   2. If, after ignoring expr’s reference-ness, expr is const, ignore that, too. If it’s
volatile, also ignore that.

#### Array Arguments
#### Function Arguments

#### Things to Remember

+ During template type deduction, arguments that are references are treated as non-references, i.e., their reference-ness is ignored.
+ When deducing types for universal reference parameters, lvalue arguments get special treatment.
+ When deducing types for by-value parameters, const and/or volatile argu‐ments are treated as non-const and non-volatile.
+ During template type deduction, arguments that are array or function names decay to pointers, unless they’re used to initialize references.

### Item 2: Understand auto type deduction

deducing types for auto is, with only one exception (which we’ll discuss soon), the same as deducing types for templates.

```cpp
auto x3 = { 27 };   // type is std::initializer_list<int>
auto x4 = { 1, 2, 3.0 } // error! can't deduce type for T

auto createInitList()
{
    return { 1, 2, 3};  // error can't deduce type for { 1, 2, 3 }
}
```

#### Things to Remember
+ auto type deduction is usually ```the same as template type deduction```, but auto type deduction assumes that a braced initializer represents a std::initializer_list, and template type deduction doesn’t.
+ auto in a function return type or a lambda parameter implies template type deduction, not auto type deduction

## Item 3: Understand decltype

decltype is an odd creature. Given a name or an expression, decltype tells you the name’s or the expression’s type.

```cpp
std::vector<int>v;
decltype(v[0]);     // int&

std::vector<bool>v1;
decltype(v[0]);     // operator[] does not return a bool&. Instead, it returns a brand new objects

decltype(auto) func()
{
    return arr[0];      // int& func()
}

// cpp11
template<typename FuncType, typename MuxType, typename PtrType>
auto lockAndCall(FuncType func, MuxType& mutex, PtrType ptr) -> decltype(func(ptr))
{
    MuxGuard g(mutex);
    return func(ptr);
}

//->
//cpp14

template<typename FuncType, typename MuxType, typename PtrType>
decltype(auto) lockAndCall(FuncType func, MuxType& mutex, PtrType ptr)
{
    MuxGuard g(mutex);
    return func(ptr);
}
```

#### Things to Remember
+ decltype almost always yields the type of a variable or expression without any modifications.
+ For lvalue expressions of type T other than names, decltype always reports a type of T&.
+ C++14 supports decltype(auto), which, like auto, deduces a type from its initializer, but it performs the type deduction using the decltype rules.

## Item 4: Know how to view deduced types

+ IDE
+ c++filt

```cpp
typeid(T).name()
```

## Item 5: Prefer auto to explicit type declaration

### Things to Remember

+ auto variables must be initialized, are generally immune to type mismatches that can lead to portability or efficiency problems, can ease the process of refactoring, and typically require less typing than variables with explicitly specified types.
+ auto-typed variables are subject to the pitfalls described in Items 2 and 6.

## Item 6: Use the explicitly typed initializer idiom when auto deduces undesired types.

```cpp
std::vector<bool>vec;
auto flag = vec[0];     // flag 并不是 bool&类型
```

## Item 7: Distinguish between () and {} when creating objects.

+ 不可拷⻉的对象可以使⽤花括号初始化或者小括号初始化，但是不能使⽤"="初始化
+ 括号表达式有⼀个异常的特性，它不允许内置类型隐式的变窄转换

### 记住
+ 括号初始化是最⼴泛使⽤的初始化语法，它防⽌变窄转换，并且对于C++最令⼈头疼的解析有天⽣的免疫性
+ 在构造函数重载决议中，括号初始化尽最⼤可能与std::initializer_list参数匹配，即便其他构造函数看起来是更好的选择
+ 对于数值类型的std::vector来说使⽤花括号初始化和小括号初始化会造成巨⼤的不同
+ 在模板类选择使⽤小括号初始化或使⽤花括号初始化创建对象是⼀个挑战。

## Item 8: Prefer nullptr to 0 and NULL

```cpp
template<typename FuncType, typename MuxType, typename PtrType>
decltype(auto) lockAndCall(FuncType func, MuxType& mutex, PtrType ptr)
{
    MuxGuard g(mutex);
    return func(ptr);
}

auto result1 = lockAndCall(f1, f1m, 0); // 错误！
…
auto result2 = lockAndCall(f2, f2m, NULL); // 错误！
…
auto result3 = lockAndCall(f3, f3m, nullptr); // 没问题
```

### 记住
+ 优先考虑nullptr而⾮0和NULL
+ 避免重载指针和整型


## Item 9: Prefer alias declarations to typedefs

### 记住
+ typedef不⽀持模板化，但是别名声明⽀持。
+ 别名模板避免了使⽤"::type"后缀，而且在模板中使⽤typedef还需要在前⾯加上typename
+ C++14提供了C++11所有类型转换的别名声明版本

## Item 10: 优先考虑限域枚举而⾮未限域枚举
```cpp
enum Color {black, white, red};
auto white = false;
// ===>
enum class Color {black, white, red};
Color c = Color::white;
auto c = Color::white;
```

### 记住
+ C++98的枚举即⾮限域枚举
+ 限域枚举的枚举名仅在enum内可⻅。要转换为其它类型只能使⽤cast。
+ ⾮限域/限域枚举都⽀持基础类型说明语法，限域枚举基础类型默认是 int 。⾮限域枚举没有默认基础类型。
+ 限域枚举总是可以前置声明。⾮限域枚举仅当指定它们的基础类型时才能前置。


## Item 17: 优先考虑使⽤deleted函数而⾮使⽤未定义的私有声明

```cpp
// 除了class以外的用法
bool isLucky(int number); // 原始版本
bool isLucky(char) = delete; // 拒绝char
bool isLucky(bool) = delete; // 拒绝bool
bool isLucky(double) = delete; // 拒绝float和double

```
### 记住
+ 为重载函数加上 override
+ 成员函数限定让我们可以区别对待左值对象和右值对象（即 *this )

## Item 13: 优先考虑const_iterator而⾮iterator

### 记住
+ 优先考虑const_iterator而⾮iterator
+ 在最⼤程度通⽤的代码中，优先考虑⾮成员函数版本的begin，end，rbegin等，而⾮同名成员函数

## Item 14: 如果函数不抛出异常请使⽤noexcept

## Item 15: 尽可能的使⽤constexpr

### 记住
+ constexpr对象是cosnt，它的值在编译期可知
+ 当传递编译期可知的值时，cosntexpr函数可以产出编译期可知的结果

## Item 16: 让const成员函数```void func() const```线程安全

## Item 17: 理解特殊成员函数的⽣成

### 记住
+ 特殊成员函数是编译器可能⾃动⽣成的函数：默认构造，析构，拷⻉操作，移动操作。
+ 移动操作仅当类没有显式声明移动操作，拷⻉操作，析构时才⾃动⽣成。
+ 拷⻉构造仅当类没有显式声明拷⻉构造时才⾃动⽣成，并且如果⽤⼾声明了移动操作，拷⻉构造就是delete。拷⻉赋值运算符仅当类没有显式声明拷⻉赋值运算符时才⾃动⽣成，并且如果⽤⼾声明了移动操作，拷⻉赋值运算符就是delete。当⽤⼾声明了析构函数，拷⻉操作不再⾃动⽣成。


