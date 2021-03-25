# std::async源码.md

有两个模板:

```cpp
  /// async
  template<typename _Fn, typename... _Args>
    future<__async_result_of<_Fn, _Args...>>
    async(launch __policy, _Fn&& __fn, _Args&&... __args)
    {
      std::shared_ptr<__future_base::_State_base> __state;
      if ((__policy & launch::async) == launch::async)
	{
	  __try
	    {
	      __state = __future_base::_S_make_async_state(
		  std::thread::__make_invoker(std::forward<_Fn>(__fn),
					      std::forward<_Args>(__args)...)
		  );
	    }
#if __cpp_exceptions
	  catch(const system_error& __e)
	    {
	      if (__e.code() != errc::resource_unavailable_try_again
		  || (__policy & launch::deferred) != launch::deferred)
		throw;
	    }
#endif
	}
      if (!__state)
	{
	  __state = __future_base::_S_make_deferred_state(
	      std::thread::__make_invoker(std::forward<_Fn>(__fn),
					  std::forward<_Args>(__args)...));
	}
      return future<__async_result_of<_Fn, _Args...>>(__state);
    }



  /// async, potential overload
  template<typename _Fn, typename... _Args>
    inline future<__async_result_of<_Fn, _Args...>>
    async(_Fn&& __fn, _Args&&... __args)
    {
      return std::async(launch::async|launch::deferred,
			std::forward<_Fn>(__fn),
			std::forward<_Args>(__args)...);
    }

```