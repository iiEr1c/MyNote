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
std::unique_lock<std::mutex> fn()
{
    extern std::mutex some_mutex;
    std::unique<std::mutex> lk(some_mutex);
    fn2();
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

// 或则这种

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

## 4. 同步操作