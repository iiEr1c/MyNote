#include <iostream>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>

class Singleton
{
public:
    static Singleton* getInstance();
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
private:
};

/*
// thread unsafe
std::shared_ptr<Singleton> Singleton::getInstance()
{
    if(m_instance == nullptr)
        m_instance = std::make_shared<Singleton>();
    else
        return m_instance;
}

// thread unsafe
std::shared_ptr<Singleton> Singleton::getInstance()
{
    if(m_instance == nullptr)
    {
        std::lock_guard<std::mutex>lk(lock);
            m_instance = std::make_shared<Singleton>();
    }
    else
        return m_instance;
}

// thread safe but ineffective
std::shared_ptr<Singleton> Singleton::getInstance()
{
    std::lock_guard<std::mutex>lk(lock);
    if(m_instance == nullptr)
    {
        
        if(m_instance == nullptr)
            m_instance = std::make_shared<Singleton>();
    }
    else
        return m_instance;
}
*/

// It isn't TriviallyCopyable when static std::atomic<std::shared_ptr<Singleton>> m_instance;
// static_assert(__is_trivially_copyable(_Tp), "std::atomic requires a trivially copyable type");
/*

Fence-fence synchronization

A release fence FA in thread A synchronizes-with an acquire fence FB in thread B, if
+ There exists an atomic object M,
+ There exists an atomic write X (with any memory order) that modifies M in thread A
+ FA is sequenced-before X in thread A
+ There exists an atomic read Y (with any memory order) in thread B
+ Y reads the value written by X (or the value would be written by release sequence headed by X if X were a release operation)
+ Y is sequenced-before FB in thread B

In this case, all non-atomic and relaxed atomic stores that are sequenced-before FA in thread A will happen-before all non-atomic and relaxed atomic loads from the same locations made in thread B after FB

*/
/*
static std::mutex lock;
static std::atomic<Singleton*> m_instance;

Singleton* Singleton::getInstance()
{
    auto tmp = m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);            // https://stackoverflow.com/questions/62947709/why-c-singleton-need-memory-order-acquire
    if(tmp == nullptr)
    {
        std::lock_guard<std::mutex>lk(lock);
        tmp = m_instance.load(std::memory_order_relaxed);
        if(tmp == nullptr)
        {
            tmp = new Singleton();                                  // new operation's instructions may reordered...
            std::atomic_thread_fence(std::memory_order_release);    // ensure m_instalce's value
            m_instance.store(tmp, std::memory_order_relaxed);
        }
    }
    return tmp;
}
*/


/*
static std::mutex lock;
static std::atomic<Singleton*> m_instance;
Singleton* Singleton::getInstance()
{
    auto tmp = m_instance.load(std::memory_order_acquire);
    if(tmp == nullptr)
    {
        std::lock_guard<std::mutex>lk(lock);
        tmp = m_instance.load(std::memory_order_relaxed);
        if(tmp == nullptr)
        {
            tmp = new Singleton();
            m_instance.store(tmp, std::memory_order_relaxed);
        }
    }
    return tmp;
}
*/

static std::atomic<Singleton*> m_instance;
Singleton* Singleton::getInstance()
{
    static std::once_flag once;
    std::call_once(once, [&]() -> void{ m_instance = new Singleton(); });
    return m_instance;
}

int main()
{
    auto func = []() -> void {
        auto instance = Singleton::getInstance();
        std::cout<<instance<<std::endl;
    };
    for(int i = 0; i < 100; ++i)
    {
        auto t = std::thread{func};
        t.join();
    }
    return 0;
}