# About typename

先看一组示例:

```cpp
#include <iostream>
#include <vector>


template<typename T>
T::value_type my_sum(const T& cont)
{
    T::value_type res = {};
    for(const auto& r : cont)
        res += r;
    return res;
}

int main()
{
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout<<my_sum(vec);
    return 0;
}
```

编译时报错如下:

```
error: need 'typename' before 'T::value_type' because 'T' is a dependent scope
 T::value_type my_sum(const T& cont)
```

在模板的定义中, T的成员类型是未知的, ```T::value_type```依赖模板的形参```T```, 编译器不把它当作一个类型, 而是把它当作其他情况(如函数, 对象等), 因此如果要编译器把它当作类型, 需要显示添加```typename```

```cpp
#include <iostream>
#include <vector>

template<typename T>
typename T::value_type my_sum(const T& cont)
{
    typename T::value_type res = {};
    for(const auto& r : cont)
        res += r;
    return res;
}

int main()
{
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::cout<<my_sum(vec);
    return 0;
}
```