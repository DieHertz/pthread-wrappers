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

        pimpl->func = [=] { call(f, args...); };

        pimpl->joinable = true;
        auto result = pthread_create(&pimpl->handle, nullptr, [] (void* pimpl_) {
            auto pimpl = static_cast<impl*>(pimpl_);
            pimpl->func();
            delete pimpl;

            return static_cast<void*>(nullptr);
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
        if (!joinable()) throw std::runtime_error{"invalid_argument"};
        pthread_join(pimpl->handle, nullptr);
        pimpl->joinable = false;
    }

    void detach() {
        if (!joinable()) throw std::runtime_error{"invalid_argument"};
        pthread_detach(pimpl->handle);
        pimpl->joinable = false;
        pimpl = nullptr;
    }

    bool joinable() const { return pimpl && pimpl->joinable; }

private:
    template<class Function, class... Args>
    static void call(Function&& f, Args&&... args) {
        f(args...);
    }

    template<class R, class Object, class... Args1, class... Args2>
    static void call(R (Object::*f)(Args1...), Object* obj, Args2&&... args) {
        (obj->*f)(args...);
    }

    struct impl {
        pthread_t handle;
        bool joinable = false;
        std::function<void(void)> func;
    };
    impl* pimpl = new impl{};
};

} /* namespace utility */

#endif /* thread_h */
