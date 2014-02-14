#ifndef thread_h
#define thread_h

#include "utility.h"
#include <pthread.h>
#include <functional>
#include <stdexcept>
#include <memory>

namespace utility {

class thread {
public:
    using native_handle_type = pthread_t;

    class id {
        native_handle_type handle{};

    public:
        id() noexcept {}
        explicit id(native_handle_type handle) : handle{handle} {}

    private:
        friend class thread;

        friend bool operator==(thread::id lhs, thread::id rhs) noexcept {
            return lhs.handle == rhs.handle;
        }

        friend bool operator<(thread::id lhs, thread::id rhs) noexcept {
            return lhs.handle < rhs.handle;
        }
    };

    thread(thread&& other) noexcept {
        *this = move(other);
    }

    template<class Function, class... Args>
    explicit thread(Function&& f, Args&&... args) {
        auto pimpl = std::make_shared<impl>();

        pimpl->func = [=] { call(f, args...); };
        pimpl->this_ptr = pimpl;

        auto result = pthread_create(&id_.handle, nullptr, [] (void* pimpl_) {
            auto pimpl = static_cast<impl*>(pimpl_);
            pimpl->func();
            pimpl->this_ptr.reset();
            return static_cast<void*>(nullptr);
        }, pimpl.get());

        if (result) {
            pimpl->this_ptr.reset();
            throw std::runtime_error{"resources_unavailable_try_again"};
        }
    }

    ~thread() {
        if (joinable()) std::terminate();
    }

    thread() = default;
    thread(const thread&) = delete;

    thread& operator=(thread&& other) noexcept {
        if (joinable()) std::terminate();

        std::swap(id_, other.id_);

        return *this;
    }

    bool joinable() const noexcept { return !(id_ == id{}); }

    void join() {
        if (!joinable()) throw std::runtime_error{"invalid_argument"};
        pthread_join(id_.handle, nullptr);
        id_ = id{};
    }

    void detach() {
        if (!joinable()) throw std::runtime_error{"invalid_argument"};
        pthread_detach(id_.handle);
        id_ = id{};
    }

    id get_id() const noexcept { return id_; }

    native_handle_type native_handle() { return id_.handle; }

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
        std::function<void(void)> func;
        std::shared_ptr<impl> this_ptr;
    };
    id id_;
};

inline bool operator!=(thread::id lhs, thread::id rhs) noexcept {
    return !(lhs == rhs);
}

inline bool operator<=(thread::id lhs, thread::id rhs) noexcept {
    return !(rhs < lhs);
}

inline bool operator>(thread::id lhs, thread::id rhs) noexcept {
    return rhs < lhs;
}

inline bool operator>=(thread::id lhs, thread::id rhs) noexcept {
    return !(lhs < rhs);
}

} /* namespace utility */

#endif /* thread_h */
