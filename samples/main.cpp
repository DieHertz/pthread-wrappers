#include "thread.h"
#include "mutex.h"
#include "synchronous.h"
#include "threadlocal.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace utility;

threadlocal<int> tlint;
const synchronous<ostream&> sync_cout{ cout };

int main() {
    auto tasks = vector<thread>{};

    for (auto i = 0; i < 10; ++i) {
        tasks.push_back(thread{[&] (const int id) {
            for (auto i = 0; i < 2; ++i) {
                sync_cout([=] (ostream& cout) {
                    if (tlint) cout << "thread_local for thread " << id << " is " << tlint.get() << endl;
                    else {
                        cout << "no thread_local for thread " << id << endl;
                        tlint = id * id + 1;
                    }
                });
            }
        }, i });
    }

    for (auto& t : tasks) t.join();
    cout << "done" << endl;
}
