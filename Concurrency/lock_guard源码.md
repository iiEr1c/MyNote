# std::lock_guard源码

以下是```std::lock_guard```的实现, 这里的```adopt_lock_t```假设调用方线程已拥有互斥的所有权, 锁策略(支持```std::lock_guard```, ```std::unique_lock```, ```std::shared_lock```)如下: 

| 类型 | 效果 |
| :----: | :----: |
| defer_lock_t | 不获得互斥的所有权 |
| try_to_lock_t | 尝试获得互斥锁的所有权而不阻塞 |
| adopt_lock_t | 假设调用方线程已拥有互斥的所有权 |

## lock_guard源码
```cpp
template<typename _Mutex>
    class lock_guard
    {
    public:
        typedef _Mutex mutex_type;
        
        explicit lock_guard(mutex_type& __m) : _M_device(__m)
        { _M_device.lock(); }
        
        lock_guard(mutex_type& __m, adopt_lock_t) noexcept : _M_device(__m)
        { } // calling thread owns mutex
        
        ~lock_guard()
        { _M_device.unlock(); }
        
        lock_guard(const lock_guard&) = delete;
        lock_guard& operator=(const lock_guard&) = delete;
        
    private:
        mutex_type&  _M_device;
    };
```

## scoped_lock源码

use std::scoped_lock to acquire two locks without worrying about other calls to assign_lunch_partner deadlocking us and it also provides a convenient RAII-style mechanism.

```cpp
#if __cplusplus >= 201703L
#define __cpp_lib_scoped_lock 201703
  /** @brief A scoped lock type for multiple lockable objects.
   *
   * A scoped_lock controls mutex ownership within a scope, releasing
   * ownership in the destructor.
   */
  template<typename... _MutexTypes>
    class scoped_lock
    {
    public:
      explicit scoped_lock(_MutexTypes&... __m) : _M_devices(std::tie(__m...))
      { std::lock(__m...); }
      explicit scoped_lock(adopt_lock_t, _MutexTypes&... __m) noexcept
      : _M_devices(std::tie(__m...))
      { } // calling thread owns mutex
      ~scoped_lock()
      {
        std::apply([](_MutexTypes&... __m) {
          char __i[] __attribute__((__unused__)) = { (__m.unlock(), 0)... };
        }, _M_devices);
      }
      scoped_lock(const scoped_lock&) = delete;
      scoped_lock& operator=(const scoped_lock&) = delete;
    private:
      tuple<_MutexTypes&...> _M_devices;
    };
  template<>
    class scoped_lock<>
    {
    public:
      explicit scoped_lock() = default;
      explicit scoped_lock(adopt_lock_t) noexcept { }
      ~scoped_lock() = default;
      scoped_lock(const scoped_lock&) = delete;
      scoped_lock& operator=(const scoped_lock&) = delete;
    };
  template<typename _Mutex>
    class scoped_lock<_Mutex>
    {
    public:
      using mutex_type = _Mutex;
      explicit scoped_lock(mutex_type& __m) : _M_device(__m)
      { _M_device.lock(); }
      explicit scoped_lock(adopt_lock_t, mutex_type& __m) noexcept
      : _M_device(__m)
      { } // calling thread owns mutex
      ~scoped_lock()
      { _M_device.unlock(); }
      scoped_lock(const scoped_lock&) = delete;
      scoped_lock& operator=(const scoped_lock&) = delete;
    private:
      mutex_type&  _M_device;
    };
#endif // C++17
```