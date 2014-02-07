#ifndef threadlocal_h
#define threadlocal_h

#include <pthread.h>
#include <utility>

namespace utility {

/** \brief Provides thread local storage, intended to be used in
    static context (global scope, static function-local or static member).
    The underlying object is dynamically allocated and has to be copy
    or move constructible
*/
template<typename T> class threadlocal {
public:
    using stored_type = T;

    /** \brief Creates an instance of thread-local container,
        no object is stored for each thread until operator= or set
        has been called explicitly
    */
    threadlocal() {
        pthread_key_create(&key, [] (void* obj) {
            delete static_cast<stored_type*>(obj);
        });
    }

    /** \brief Destroys thread-local container, invoking destructor
        for each stored object
    */
    ~threadlocal() {
        pthread_key_delete(key);
    }

    /** \brief Thread-local container isn't copy- nor move-assignable
    */
    threadlocal(const threadlocal&) = delete;
    threadlocal& operator=(const threadlocal&) = delete;

    /** \brief Copies or moves provided object into thread-local storage
        for current thread
    */ 
    template<typename U> threadlocal& operator=(U&& other) {
        set(std::forward<U>(other));
        return *this;
    }

    explicit operator bool() const {
        return !empty();
    }

    bool empty() const {
        return !pthread_getspecific(key);
    }

    stored_type* operator&() {
        return get_pointer();
    }

    template<typename U> void set(U&& other) {
        delete get_pointer();
        pthread_setspecific(key, new T{std::forward<U>(other)});
    }

    stored_type& get() {
        return *get_pointer();
    }

    stored_type* get_pointer() {
        return static_cast<stored_type*>(pthread_getspecific(key));
    }

private:
    pthread_key_t key;
};

} /* namespace utility */

#endif /* threadlocal_h */
