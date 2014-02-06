#ifndef thread_h
#define thread_h

#include "mutex.h"
#include <pthread.h>
#include <functional>
#include <stdexcept>

namespace utility {

class thread {
public:
    thread(thread&& other) noexcept : pimpl{new impl} {
        *this = std::move(other);
    }

    template<class Function, class... Args>
    explicit thread(Function&& f, Args&&... args)
    : pimpl{new impl} {
        auto pimpl = this->pimpl;

        pimpl->func = [=] {
            {
                lock_guard<mutex> hold(pimpl->m);
                pimpl->running = pimpl->joinable = true;
            }

            f(args...);

            lock_guard<mutex> hold(pimpl->m);
            pimpl->running = false;
        };

        auto result = pthread_create(&pimpl->handle, nullptr, [] (void* pimpl) {
            static_cast<impl*>(pimpl)->func();
            return pimpl;
        }, pimpl);

        if (result) throw std::runtime_error{"pthread_create failed"};
    }

    ~thread() {
        if (joinable()) std::terminate();

        delete pimpl;
    }

    thread() = default;
    thread(const thread&) = delete;

    thread& operator=(thread&& other) noexcept {
        if (joinable()) throw std::runtime_error{"moving to joinable thread"};

        std::swap(pimpl, other.pimpl);

        return *this;
    }

    void join() {
        pthread_join(pimpl->handle, nullptr);
        lock_guard<mutex> hold(pimpl->m);
        pimpl->joinable = false;
    }

    bool joinable() const { return pimpl->joinable; }

private:
    struct impl {
        mutex m;
        bool running = false;
        bool joinable = false;
        pthread_t handle;
        std::function<void(void)> func;
    };
    impl* pimpl;
    
};

} /* namespace utility */

#endif /* thread_h */
