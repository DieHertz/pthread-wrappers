#ifndef synchronous_h
#define synchronous_h

#include "mutex.h"

namespace utility {

template<typename T> class synchronous {
    template<typename U> struct container { U u; };
    mutable container<T> t;
    mutable mutex m;

public:
    synchronous(T t = T{}) : t{t} {}

    template<typename F>
    auto operator()(F func) const -> decltype(func(t.u)) {
        lock_guard<mutex> _{ m };
        return func(t.u);
    }
};

} /* namespace utility */

#endif /* synchronous_h */
