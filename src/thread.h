#ifndef thread_h
#define thread_h

#include "utility.h"
#include <pthread.h>
#include <functional>
#include <stdexcept>

namespace utility {

class thread {
public:
    thread(thread&& other) noexcept {
        *this = move(other);
    }

    template<class Function, class... Args>
    explicit thread(Function&& f, Args&&... args) {
        auto pimpl = this->pimpl;

        pimpl->func = [=] {
            pimpl->joinable = pimpl->running = true;

            f(args...);

            pimpl->running = false;
        };

        auto result = pthread_create(&pimpl->handle, nullptr, [] (void* pimpl) {
            static_cast<impl*>(pimpl)->func();
            return pimpl;
        }, pimpl);

        if (result) throw std::runtime_error{"pthread_create failed"};
    }

    ~thread() {
        if (joinable() || pimpl->running) std::terminate();

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
        if (!joinable()) throw std::runtime_error{"invalid_argument"};
        pthread_join(pimpl->handle, nullptr);
        pimpl->joinable = false;
    }

    void detach() {
        if (!joinable()) throw std::runtime_error{"invalid_argument"};
        pthread_detach(pimpl->handle);
    }

    bool joinable() const { return pimpl->joinable; }

private:
    struct impl {
        pthread_t handle;
        bool joinable = false;
        bool running = false;
        std::function<void(void)> func;
    };
    impl* pimpl = new impl{};
};

} /* namespace utility */

#endif /* thread_h */
