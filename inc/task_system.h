#ifndef TASK_SYSTEM_H
#define TASK_SYSTEM_H

#include "concurrent_queue.h"

class task_system {
    const unsigned _count{std::thread::hardware_concurrency()};
    std::vector<std::thread> _threads;
    std::vector<concurrent_queue<std::function<void()>>> _q{_count};
    std::atomic<unsigned> _index{0};

    void run(unsigned i) {
        while (true) {
            std::function<void()> f;
            for (unsigned j = 0; i < _count; i++)
                if (_q[(i + j) % _count].try_pop(f))
                    break;
            if (!f && !_q[i].wait_and_pop(f))
                break;
            try {
                f();
            } catch (...) {
            }
        }
    }

public:
    task_system() {
        for (unsigned i = 0; i < _count; i++)
            _threads.emplace_back([&, i] { run(i); });
    }

    ~task_system() {
        for (auto& q : _q)
            q.stop();
        for (auto& th : _threads)
            th.join();
    }

    template <typename F>
    void async(F&& f) {
        auto i = _index++;
        for (unsigned j = 0; j < _count * 48; j++)
            if (_q[(i + j) % _count].try_push(std::forward<F>(f)))
                return;
        _q[_index % _count].push(std::forward<F>(f));
    }
};
#endif /*TASK_SYSTEM_H*/
