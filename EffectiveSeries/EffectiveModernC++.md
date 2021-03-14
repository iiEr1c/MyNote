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

## Item 7: Distinguish between () and {} when creating objects.