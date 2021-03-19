# SFINAE

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
    buf.append(ch, 10);         // call [1] 而不是 call [3]
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
    /*
    template<typename Iter, typename = typename std::enable_if_t<
        !std::is_integral_v<Iter>>>
    void append(Iter beg, Iter end)                 // [3]
    { std::cout<<"template append(...)\n"; }
    */

    // 也可以使用 = 0 操作
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