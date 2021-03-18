#include <iostream>

class Test
{
public:
    Test() { std::cout<<"ctor..."<<std::endl; }
    ~Test() { std::cout<<"dtor..."<<std::endl; }
};

void func2(Test t)
{

}

void func(Test t)
{
    func2(t);
}

void func2_(Test&& t)
{

}

void func_(Test&& t)
{
    func2_(std::forward<Test>(t));
}


int main()
{
    //Test t;
    //func(t);    // 拷贝构造两次
    func_(Test());    // 拷贝构造0
}