#include <memory>
#include <iostream>

int main()
{
    auto iptr = std::make_unique<int>(100);
    auto lbd = [iptr = std::move(iptr)]() -> void
    {
        std::cout<<*iptr<<std::endl;
    };
    lbd();
    return 0;
}