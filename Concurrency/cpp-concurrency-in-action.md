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

```thread```的构造函数

```cpp
template<typename _Callable, typename... _Args>
explicit
    thread(_Callable&& __f, _Args&&... __args)
    {
#ifdef GTHR_ACTIVE_PROXY
	// Create a reference to pthread_create, not just the gthr weak symbol.
	auto __depend = reinterpret_cast<void(*)()>(&pthread_create);
#else
	auto __depend = nullptr;
#endif
    _M_start_thread(_S_make_state(
        __make_invoker(std::forward<_Callable>(__f),
            std::forward<_Args>(__args)...)),
            __depend);
    }
// __make_invoker 的作用
// Returns a call wrapper that stores
// tuple{DECAY_COPY(__callable), DECAY_COPY(__args)...}.
```

这里的```__make_invoker```是类模板```_Invoker```的```public```成员, 作用是返回一个(新)可执行对象(包括: 原可执行对象 + 参数). ```_S_make_state```的任务是:使用```__make_invoker```返回的可调用对象, 创建一个```unique_ptr<_State>```指针并返回. 后续```thread```调用```thead```的构造函数(```thread(thread&& __t) noexcept```)创建```thread```对象即可. 


```cpp
// Abstract base class for types that wrap arbitrary functors to be
// invoked in the new thread of execution.
struct _State
    {
      virtual ~_State();
      virtual void _M_run() = 0;
    };
    using _State_ptr = unique_ptr<_State>;

template<typename _Callable>
static _State_ptr
    _S_make_state(_Callable&& __f)
    {
        using _Impl = _State_impl<_Callable>;
        return _State_ptr{new _Impl{std::forward<_Callable>(__f)}};
    }

template<typename _Callable>
    struct _State_impl : public _State
    {
        _Callable		_M_func;
        
        _State_impl(_Callable&& __f) : _M_func(std::forward<_Callable>(__f))
        { }
        void
        _M_run() { _M_func(); }
    };
```