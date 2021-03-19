#include <chrono>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <numeric>

template <class Vec>
void sort(Vec& vec)
{
    auto start = std::chrono::system_clock::now();
    std::sort(vec.begin(), vec.end());
    auto end = std::chrono::system_clock::now();
    std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()<<std::endl;
}


int main()
{
    std::vector<int> vec(1024 * 1024 * 64);
    std::iota(vec.begin(), vec.end(), 0);

    auto v1 = vec;
    auto v2 = vec;

    std::reverse(v1.begin(), v1.end());
    std::random_shuffle(v2.begin(), v2.end());

    sort(vec);
    sort(v1);
    sort(v2);

    assert(is_sorted(vec.begin(), vec.end()));
    assert(is_sorted(v1.begin(), v1.end()));
    assert(is_sorted(v2.begin(), v2.end()));
    return 0;
}