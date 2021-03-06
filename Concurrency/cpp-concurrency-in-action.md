# C++ Concurrency in action

## 2. 线程管理

### 2.1 线程基本操作

启动线程: 等待线程结束or让其自主运行. 若是未作决定, 程序会终止. 因此, 即使有异常存在, 也要确保线程能够正确汇入(```join```)或者分离(```detached```). 尤其是要注意, 新旧线程各自对象的生命周期问题, 因为RAII机制的存在, 错误地访问数据会导致未定义行为

两种控制方式:

+ 分离```xxx.detach()```: 注意局部变量的有效性
+ 等待线程完成```xxx.join()```: 可以确保局部变量在线程完成后才销毁, ```joinable()```可判断状态
+ 其他灵活控制方式: 如查看一下进程是否结束, 等待一段时间
  + 条件变量
  + future

以下是```thread```的析构函数
```cpp
~thread()
{
    if (joinable())
        std::terminate();
}
```

### 2.2 传递参数
+ 拷贝传递
+ 引用传递```std::ref(...)```

### 2.3 转移所有权

```thread```可移动(```std::move```), 但不可复制. 析构函数中添加```join()```, ```jthread```(cpp20)

### 2.4 确定线程数量

```std::thread::hardware_concurrency()```


### 2.5 线程标识

即```std::thread::get_id()```

## 3. 共享数据

### 3.1 共享数据的问题

多个线程对同一个对象进行```写```操作.

#### 3.1.1 条件竞争

+ 时间敏感, 不易调试

#### 3.1.2 避免恶性条件竞争

+ 对```数据结构```保护机制 -> 确保只有修改线程才能看到不变量的中间状态, 对于其他访问线程来说, 修改要么完成, 要么还没开始
+ 对数据结构和不变量进行修改, 修改完的结构必须能完成一些列不可分割的变化, 也就保证了每个不变量的状态, 即所谓的无锁编程
+ 使用事务处理数据结构的更新
  + 类似数据库

### 3.2 使用互斥量

访问前上锁, 访问后解锁, 其他线程在不持有锁的时候无法访问访问数据

#### 3.2.1 互斥量

使用```std::mutex```创建互斥量实例, 成员函数```lock()```上锁, ```unlock()```解锁. 推荐使用RAII模板类```std::lock_guard```

```std::lock_guard```在C++17可以省略模板参数列表(```std::lock_guard guard(mutex)```), 同时提供了一种加强版数据保护机制```std::scoped_lock```

#### 3.2.2 保护共享数据

```std::lock_guard```在一些情况下不能保护共享数据, 如:

指针或引用 -> 只要没有成员函数通过返回值或者输出参数的形式向其调用者返回```受保护数据的指针或引用```, 数据就是安全的.

切勿将受保护数据的指针或引用传递到互斥锁作用域之外. 

```cpp
type data;
void fn()
{
    std::lock_guard<std::mutex>lk(mux);
    fn2(data);
}
```

### 3.2.3 接口间的条件竞争

+ stack的```empty()```和```pop()```接口

### 3.2.4 死锁: 问题描述及解决方案

+ 两个互斥量以相同的顺序上锁
+ 使用```std::lock()```可以一次性锁住多个(两个以上)互斥量, 并且没有副作用(```scoped_lock```)

```cpp
std::scoped_lock guard(mux1, mux2);
// std::scoped_lock<std::mutex, std::mutex> guard(mux1, mux2);
```

### 3.2.5 如何避免死锁

+ 避免嵌套锁 -> 倘若不得不获取多个锁,使用```std::lock```或```std::scoped_lock```
+ 避免在持有锁是调用外部代码
+ 使用固定顺序获取锁
+ 层次锁

### 3.2.6 std::unique_lock 灵活的锁(与3.2.7有关)

unique_lock可移动,但不可复制, unique_lock实例的体积通常比```std::lock_guard```实例大, 当使用```std::unique_lock```替代```std::lock_guard```, 会对标志进行更新或检查, 就会有一些轻微的性能惩罚

```cpp
void transfer(Box &from, Box &to, int num)
{
    // don't actually take the locks yet
    std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
    std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);
    // do somthing else..
    // ...

    // lock both unique_locks without deadlock
    std::lock(lock1, lock2);
 
    from.num_things -= num;
    to.num_things += num;
 
    // 'from.m' and 'to.m' mutexes unlocked in 'unique_lock' dtors
}
```

### 3.2.7 不同域中互斥量的传递

函数```fn```中的```return lk```不需要调用```std::move()```, 编译器会进行[返回值优化](https://en.wikipedia.org/wiki/Copy_elision#Return_value_optimization), ```fn2```函数直接转移```std::unique_lock```实例的所有权, 调用```do_something```可使用正确的数据. 挡在没有必要持有锁的时候, 可以在特定的代码分支对锁进行释放(```release()```)

```cpp
std::unique_lock<std::mutex> get_lock()
{
    extern std::mutex some_mutex;
    std::unique<std::mutex> lk(some_mutex);
    // do something...
    return lk;
}

void fun2()
{
    std::unique_lock<std::mutex> lk(get_lock());
    do_something();
}
```

### 3.2.8 锁的粒度

核心在于降低持有锁的时间, 如对文件输入/输出上锁特别耗时

## 3.3 保护共享数据的方式

### 3.3.1 保护共享数据的初始化过程

如何保证延迟初始化(Lazy initialization)的安全(可以用单例模式当作例子)

```cpp
std::once_flag resource_flag;
std::shared_ptr<some_resource>resource_ptr;

void init_resource()
{
    resource_ptr.reset(new some_resource);
}

void foo()
{
    std::call_once(resource_flag, init_resource);
    resource_ptr->do_something();
}

// 或者这种

class my_class;
my_class& get_my_class_instance()
{
    static my_class instance; // 线程安全的初始化过程
    return instance;
}

```

#### 3.3.2 保护不常更新的数据结构

读写锁, C++17提供了```std::shared_mutex```和```std::shared_timed_mutex```, C++14只提供了```std::shared_timed_mutex```


#### 3.3.3 嵌套锁

不推荐

### 3.4 本章总结

几种互斥体所有权包装器:

1. ```lock_guard```: 实现严格基于作用域的互斥体所有权包装器
2. ```unique_lock```: 通用互斥包装器, 允许延迟锁定、锁定的有时限尝试、递归锁定、所有权转移和与条件变量一同使用
3. ```scoped_lock```: 用于多个互斥体的免死锁 RAII 封装器, 它在作用域块的存在期间占有一或多个互斥

一些API:

1. ```lock```: 锁定给定的可锁定(Lockable)对象, 用免死锁算法避免死锁


一些规定:

+ BasicLockable: 要求描述提供为执行代理(即线程)提供排他性阻塞语义的类型的最小特征
+ Lockable: 要求扩展BasicLockable要求, 以包含有意图的锁定
+ TimedLockable: 要求描述为执行代理(线程、进程、任务)提供定时排他性阻塞语义的类型特征

|  | 表达式 | 效果 | 返回值 |
| :----: | :----: | :----: | :----: |
| BasicLockable | ```m.lock()```和```m.unlock()``` | 阻塞到能为当前执行代理(线程、进程、任务)获得锁为止.若抛异常,则不获得锁; 释放执行代理曾保有的锁.不抛异常. | 
| Lockable | ```m.try_lock()``` | 试图为当前执行代理(线程、进程、任务)取得锁,```而不阻塞```.若抛异常,则不获得锁. | 得到锁:```true```否则```false``` |
| TimedLockable | ```m.try_lock_for(duration)```和```m.try_lock_until(time_limit)``` | 阻塞提供的时长,或直至取得 m 上的锁; 阻塞直至提供的限制时间点,或直至取得 m 上的锁. |


## 4 同步操作

### 4.1 等待时间或条件

#### 4.1.1 等待条件达成

C++标准库对条件变量有两套实现:

+ ```std::condition_variable```
  + ```wait(unique_lock<mutex>& __lock) noexcept```: 睡眠
  + ```wait(unique_lock<mutex>& __lock, _Predicate __p) noexcept```: 有条件睡眠
  + ```notify_one() noexcept```: 唤醒一个正在```wait```的线程
+ ```std::condition_variable_any```

条件变量适用于多个线程重复等待条件成立的情况. 但是当条件只需满足一次即可的情况下, 可以使用```future```

使用```unique_lock```的原因: 等待线程必须在等待时间解锁互斥量, 并对互斥量再次上锁, 而```std::lock_guard```没有这么灵活. 如果互斥量在线程休眠期间```保持锁住状态```, 其他线程则无法锁住互斥量.

#### 4.2 使用future

当线程需要等待特定事件时, 某种程度上来说需要知道期望的结果. 之后, 线程会周期性(较短的周期, 为了保证较快的响应)的等待或检查时间是否触发, 检查期间也会执行其他任务. 另外, 等待任务期间也可以执行另外的任务, 直到对应的任务触发, 而后等待future的状态会变为```就绪状态```. future可能是和数据相关, 也可能不是. 当事件发生时(状态为就绪), 这个future就不能重置了.

C++标准库有两种future, 声明在```<future>```头文件中:

+ ```unique_future```: 只能与```指定事件```关联
+ ```shared_futures```: 能关联```多个事件```

#### 4.2.1 后台任务的返回值

```cpp
#include <future>
#include <vector>
#include <iostream>

class test
{
public:
    int func(int i) { return i; }
};

int main()
{
    auto fn = []() -> int { return 10; };
    test t2, t3, t4;

    std::vector<std::future<int>>vec;
    vec.emplace_back(std::async(std::launch::async, std::move(fn)));
    vec.emplace_back(std::async(std::launch::deferred, &test::func, std::ref(t2), 1));
    vec.emplace_back(std::async(&test::func, &t3, 2));
    vec.emplace_back(std::async(&test::func, t4, 3));
    for(auto& i : vec)
        std::cout<<i.get()<<std::endl;
    return 0;
}
```


### 4.2.2 future与任务关联 -> ```std::package_task<>```

std::packaged_task<> 会将future与函数或可调用对象进行绑定.当调用 std::packaged_task<> 对象时,就会调用相关函数或可调用对象,当future状态为就绪时,会存储返回值

```cpp
#include <future>
#include <vector>
#include <iostream>

int main()
{
    std::vector<std::packaged_task<int()>>vec;
    vec.emplace_back([]() -> int { return 1; });
    vec.emplace_back([]() -> int { return 2; });
    vec.emplace_back([]() -> int { return 3; });
    std::packaged_task<int()>task;
    
    auto beg = vec.begin();
    while (beg != vec.end())
    {
        task = std::move(*beg);
        task();
        beg = vec.erase(beg);
        auto result = task.get_future();
        std::cout<<result.get()<<std::endl;
    }
    // or use clear
    // vec.clear();

    return 0;
}
```

```packaged_task```若是需要与其他线程交互, 如获取返回值 -> 线程```return``` & ```packaged_task```模板参数需要定义, 无法通用.


### 4.2.3 使用std::promise & 异常存储于future中


```cpp
#include <future>
#include <thread>
#include <iostream>
#include <exception>

int main()
{
    auto fn = [](int val, std::promise<int> p) {
        try
        {
            throw std::runtime_error("Example");
        }
        catch(const std::exception& e)
        {
            std::cerr<<"Exception from the thread:"<< e.what()<<'\n';
            p.set_value(100);
        }
        
    };

    std::promise<int>p;
    auto fut = p.get_future();
    std::async(std::launch::async, fn, 10, std::move(p));
    try
    {
        std::cout<<fut.get()<<std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr <<"Exception from the thread: "<< e.what() << '\n';
    }
    
    return 0;
}
```


在多个线程等待的时候, 只有一个线程能获取结果. 当多个线程等待相同事件的结果时, 就需要使用```std::shared_future```来替代```std::future```了

### 4.2.5 多个线程的等待

使用```shared_future```即可

```cpp
std::promise<std::string> p;
std::shared_future<std::string> sf(p.get_future());     // 隐式转移所有权
// or
// std::future<std::string> f(p.get_future());
// std::shared_future<std::string>sf(std::move(f));
```

## 4.3 限时等待

本质是表示阻塞当前线程多长时间, 一种是"duration"(```_for```), 另一种是"point"(```_until```).

### 4.3.1 时钟

当前时间: ```chrono::time_point<system_clock, duration> std::chrono::system_clock::now()```

### 4.3.2 时间段

时间段使用标准库的```std::chrono```的```duration```即可.

```cpp
using namespace std::chrono_literals;
auto one_day = 24h;
auto half_an_hour = 30min;
auto max_time_between_messages = 30ms;
```

```future```的```wait_for```方法的返回值有三种:

+ ```std::future_status::ready```: 结果就绪
+ ```std::future_status::timeout```: 已经过时限
+ ```std::future_status::defered```: 要计算结果的函数仍未启动

注意:

+ clock, time_point, duration可能抛出异常, 但```std::chrono```不会抛出
+ 建议实现在调用前检测valid == false的情况抛出以```future_errc::no_state```为```error_condition```的```future_error```

```cpp
try
{
    auto n = fut.get();
}
catch(const std::future_error& e)
{
    std::cerr<<e.code()<<"\nmessage:"<<e.what()<<std::endl;
}
```

### 4.3.3 时间点

```cpp
std::condition_variable cv;
std::unique_lock<std::mutex>lk(m);
while(!done)
{
    if(cv.wait_until(lk, timeout)) == std::cv_status::timeout)
    {
        //...
    }
}
```

以上的代码为了处理假唤醒, 使得循环的整体长度有限. 

### 4.3.4 使用超时

使用超时的最简单方式, 就是对特定线程添加延迟处理. 当线程无所事事时, 就不会占用其他线程的处理时间

```cpp
sleep_for();
sleep_until();
```

当然, 休眠只是超时处理的一种形式, 超时可以配合条件变量和future一起使用. 超时甚至可以在获取互斥锁时(当互斥量支持超时时)使用. ```std::mutex```和```std::recursive_mutex```不支持超时, 而```std::time_mutex```和```std::recursive_timed_mutex```支持超时


可接受超时的函数

| 类型 | 函数 | 返回值 |
| :----: | :----: | :----: |
| ```std::this_thread```命名空间 | ```sleep_for(duration)``` | N/A |
| ```std::this_thread```命名空间 | ```sleep_until(time_point)``` | N/A |
| ```std::condition_variable```或```std::condition_variable_any``` | ```wait_for(lock, duration)``` | ```std::cv_status::time_out```或```std::cv_status::no_timeout``` |
| ```std::condition_variable```或```std::condition_variable_any``` | ```wait_until(lock, time_point)``` | ```std::cv_status::time_out```或```std::cv_status::no_timeout``` |
|| ```wait_for(lock, duration, predicate)``` | ```bool```--当唤醒时,返回谓词的结果 |
|| ```wait_until(lock, time_point, predicate)``` | ```bool```--当唤醒时,返回谓词的结果 |
| ```std::timed_mutex```或```std::recursive_timed_mutex``` | ```try_lock_for(duration)``` | ```bool```--获取锁时返回```true```, 否则返回```fasle``` |
| ```std::timed_mutex```或```std::recursive_timed_mutex``` | ```try_lock_until(time_point)``` | ```bool```--获取锁时返回```true```, 否则返回```fasle``` |
| ```std::unique_lock<TimedLockable>``` | ```unique_lock(lockable, duration)``` | N/A--对新构建的对象调用```owns_lock()``` |
| ```std::unique_lock<TimedLockable>``` | ```sleep_until(lockable, time_point)``` | 当获取锁时返回```true```, 否则返回```false``` |
| | ```try_lock_for(duration)``` | ```bool```--当获取锁时返回```true```, 否则返回```false``` |
| | ```try_lock_until(time_point)``` | ```bool```--当获取锁时返回```true```, 否则返回```false``` |
| ```std::future<ValueType>```或```std::shared_future<ValueType>``` | ```wait_for(duration)``` | 等待超时, 返回```std::future_status::timeout``` |
| ```std::future<ValueType>```或```std::shared_future<ValueType>```| ```wait_until(time_point)``` | 当期望值准备就绪时, 返回```std::future_status::ready``` |
| ```std::future<ValueType>```或```std::shared_future<ValueType>``` | ```wait_until(time_point)``` | 当期望值持有一个为启动的延迟函数, 返回```std::future_status::deferred``` |


## 4.4 简化代码

### 4.4.1 使用future的函数化编程

### 4.4.2 使用消息传递的同步操作

CSP: 没有共享数据时, 每个线程可以基于所接收到的信息独立运行. 每个线程就都有状态机: 收到一条信息, 会以某种方式更新状态, 并且可能向其他线程发出信息(消息处理机制依赖于线程的初始化状态)

### 4.4.5 等待多个future

假设有很多的数据需要处理, 每个数据都可以单独的进行处理, 这就是利用硬件的好机会. 可以使用异步任务组来处理数据项, 每个任务通过future返回处理结果. 不过, 需要等待所有任务完成, 才能得到最终的结果.对逐个future进行收集, 然后再整理结果, 总感觉不是很爽. 如果用异步任务来收集结果, 先要生成异步任务, 这样就会占用线程的资源, 并且需要不断的对future进行轮询, 当所有future状态为就绪时生成新的任务


### 4.4.7 锁存器和栅栏(std::latch和std::barrier)

```std::latch```: 锁存器是一种同步对象, 当计数器减为0时(没有可能增加或重置计数器), 就处于就绪态了. 锁存器是基于其输出特性--当处于就绪态时, 就会保持就绪态, 直到被销毁. 因此, 锁存器是为同步一系列事件的轻量级机制

```std::barrier```: 栅栏是一种可复用的同步机制, 其用于一组线程间的内部同步. 虽然, 锁存器不在乎是哪个线程使得计数器递减--同一个线程可以对计数器递减多次, 或多个线程对计数器递减一次, 再或是有些线程对计数器有两次的递减. 对于栅栏来说, 每一个线程```只能```在每个周期到达栅栏一次. 当线程都抵达栅栏时, 会对线程进行阻塞, 直到所有线程都达到栅栏处, 这时阻塞将会被解除. 栅栏可以复用--线程可以再次到达栅栏处, 等待下一个周期的所有线程.

```latch```可用于实现以下类似的操作:(C++20以上)

```cpp
#include <latch>
#include <vector>
#include <future>

int main()
{
    std::vector<std::future<void>> threads;
    const int threads_count = 10;
    int data[threads_count];
    std::latch done(threads_count);
    for(int i = 0; i < threads_count; ++i)
    {
        threads.emplace_back(std::async(std::launch::async, [&, i]{
            data[i] = i;
            done.count_down();
            //...
        }));
    }

    done.wait();
    //process_data...
}
```



