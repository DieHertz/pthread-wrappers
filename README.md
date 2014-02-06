pthread-wrappers
================

I'm starting development of these classes because my employer refuses to move from
iOS 4.3 and Android 2.2 (which do not support C++11 concurrency library) and is also not
willing to use Boost.
Therefore I'm going to implement required functionality using libpthread, which is available on both
platforms natively
