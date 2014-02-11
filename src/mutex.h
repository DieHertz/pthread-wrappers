#ifndef mutex_h
#define mutex_h

#include <pthread.h>

namespace utility {

class mutex {
public:
    constexpr mutex() noexcept : handle(PTHREAD_MUTEX_INITIALIZER) {};
    ~mutex() { pthread_mutex_destroy(&handle); }

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    void lock() { pthread_mutex_lock(&handle); }
    bool try_lock() { return pthread_mutex_trylock(&handle); }
    void unlock() { pthread_mutex_unlock(&handle); }

    using native_handle_type = pthread_mutex_t;
    native_handle_type native_handle() { return handle; }

private:
    pthread_mutex_t handle;
};

template<class Mutex> class lock_guard {
public:
    using mutex_type = Mutex;

    explicit lock_guard(mutex_type& m) : m(m) { m.lock(); }
    ~lock_guard() { m.unlock(); }

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;

private:
    mutex_type& m;  
};

} /* namespace utility */

#endif /* mutex_h */
