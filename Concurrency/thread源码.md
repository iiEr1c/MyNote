# thread分析

```thread```模板不定参数构造函数

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


thread(thread&& __t) noexcept
    { swap(__t); }

void
    swap(thread& __t) noexcept
    { std::swap(_M_id, __t._M_id); }
```