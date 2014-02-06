#ifndef synchronous_h
#define synchronous_h

#include "mutex.h"

namespace utility {

template<class T> class synchronous {
    T t;
    mutable mutex m;

public:
    synchronous(T t = T{}) : t(t) {}

    template<typename F>
    auto operator()(F func) const -> decltype(func(t)) {
        lock_guard<mutex> _{ m };
        return func(t);
    }
};

} /* namespace utility */

#endif /* synchronous_h */
