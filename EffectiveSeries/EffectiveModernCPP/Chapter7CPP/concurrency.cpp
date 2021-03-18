#include <iostream>
#include <future>
#include <thread>
#include <vector>

constexpr auto tenMillion = 10'000'000;
bool doWork(std::function<bool(int)>filter, int maxVal = tenMillion)
{
    std::vector<int> goodVals;
    std::thread t([&filter, maxVal, &goodVals]
                  {
                      for(auto i = 0; i <= maxVal; ++i)
                        if(filter(i))
                            goodVals.push_back(i);
                  });
}

int main()
{
    

    return 0;
}