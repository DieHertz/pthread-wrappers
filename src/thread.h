#ifndef thread_h
#define thread_h

#include <pthread.h>
#include <functional>
#include <stdexcept>

namespace utility {

class thread {
public:
    thread(thread&& other) noexcept {
        *this = std::move(other);
    }

    template<class Function, class... Args>
    explicit thread(Function&& f, Args&&... args) {
        auto pimpl = this->pimpl;

        pimpl->func = [=] {
            pimpl->running = pimpl->joinable = true;

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
        pimpl->joinable = false;
    }

    bool joinable() const { return pimpl->joinable; }

private:
    struct impl {
        pthread_t handle;
        bool running = false;
        bool joinable = false;
        std::function<void(void)> func;
    };
    impl* pimpl = new impl{};
};

} /* namespace utility */

#endif /* thread_h */
