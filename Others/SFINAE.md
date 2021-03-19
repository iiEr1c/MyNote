# SFINAE

## 重载问题

先看一个示例:

```cpp
#include <string>
#include <cstdio>

std::string to_string(long long val) { return "to_string(long long val)\n"; }
std::string to_string(unsigned long long val) { return "to_stringunsigned long long)\n"; }
std::string to_string(long double val) { return "to_stringlong double)\n"; }

int main()
{
    auto str = to_string(1234);
    auto str2 = to_string(-1234);
    auto str3 = to_string(12.34);
    return 0;
}
```

在编译时, to_string会出现ambiguous, 虽然我们可以为每一种类型都写一个```to_string```, 但这种方案不仅工作量大, 而且因为函数体内转换方式基本一致, 因此可以考虑其他方案: 通过类型推导```tag```来解决重载问题

```cpp
#include <string>
#include <cstdio>

std::string to_string(std::true_type, std::true_type, long long val)
{ return "to_string(long long val)\n"; }
std::string to_string(std::true_type, std::false_type, unsigned long long val)
{ return "to_stringunsigned long long)\n"; }
std::string to_string(std::false_type, std::true_type, long double val)
{ return "to_stringlong double)\n"; }

template<typename T>
std::string T_to_string(const T& t)
{
    return to_string(std::is_integral<T>(), std::is_signed<T>(), t);
}


int main()
{
    auto str = T_to_string(1234);
    auto str2 = T_to_string(-1234);
    auto str3 = T_to_string(12.34);
    return 0;
}
```

## std:enable_if

下面看一组示例:

```cpp
#include <iostream>
#include <string>
#include <vector>

class Buffer
{
private:
public:
    void append(char c, size_t n = 1)               // [1]
    { std::cout<<"append(char, size_t)\n"; }
    void append(const char* s, size_t n)            // [2]
    { std::cout<<"append(const char*, size)t)\n"; }

    template<typename Iter>                         // [3]
    void append(Iter beg, Iter end)
    { std::cout<<"template append(...)\n"; }
};

int main()
{
    char ch = 'T';
    std::vector<char> vec{'a','b','v'};
    auto beg = vec.cbegin();
    auto end = vec.cend();
    Buffer buf;
    buf.append(ch, 10);         // call [3] 而不是 [1]
}
```

我们应该如何解决```append```的调用问题呢?(重载决议)

```cpp
#include <iostream>
#include <string>
#include <vector>

class Buffer
{
private:
public:
    void append(char c, size_t n = 1)               // [1]
    { std::cout<<"append(char, size_t)\n"; }
    void append(const char* s, size_t n)            // [2]
    { std::cout<<"append(const char*, size)t)\n"; }

    /* cpp11
    template<typename Iter, typename = typename std::enable_if_t<
        !std::is_integral<Iter>::value
        >::type>
    void append(Iter beg, Iter end)                 // [3]
    { std::cout<<"template append(...)\n"; }
    */
    // -std = c++17(is_integral_v)
    // 方式 1
    /*
    template<typename Iter, typename = typename std::enable_if_t<
        !std::is_integral_v<Iter>>>
    void append(Iter beg, Iter end)                 // [3]
    { std::cout<<"template append(...)\n"; }
    */

    // 方式 2
    template<typename Iter, std::enable_if_t<
        !std::is_integral_v<Iter>> = 0>
    void append(Iter beg, Iter end)                 // [3]
    { std::cout<<"template append(...)\n"; }
};

int main()
{
    char ch = 'T';
    std::vector<char> vec{'a','b','v'};
    auto beg = vec.cbegin();
    auto end = vec.cend();
    Buffer buf;
    buf.append(ch, 10);         // call [1]
}
```

在```C++17```中, 定义了```is_integral_v```, 类似```C++14```的```enable_if_t```

```cpp
template< bool B, class T = void >
using enable_if_t = typename enable_if<B,T>::type; // c++14


template< bool B, class T = void >
using enable_if_t = typename enable_if<B,T>::type   // c++17
```

# reference
+ [An introduction to C++'s SFINAE concept: compile-time introspection of a class member](https://jguegant.github.io/blogs/tech/sfinae-introduction.html)
+ [现代C++之SFINAE](https://blog.csdn.net/guangcheng0312q/article/details/103884392)