#ifndef TASK_SYSTEM_H
#define TASK_SYSTEM_H

#include "concurrent_queue.h"

class task_system {
    const unsigned count_{std::thread::hardware_concurrency()};
    std::vector<std::thread> threads_;
    std::vector<concurrent_queue<std::function<void()>>> queues_{count_};
    std::atomic<unsigned> index_{0};

    void run(unsigned i) {
        while (true) {
            std::function<void()> f;
            for (unsigned j = 0; i < count_; i++)
                if (queues_[(i + j) % count_].try_pop(f))
                    break;
            if (!f && !queues_[i].wait_and_pop(f))
                break;
            try {
                f();
            } catch (...) {
            }
        }
    }

public:
    task_system() {
        for (unsigned i = 0; i < count_; i++)
            threads_.emplace_back([&, i] { run(i); });
    }

    ~task_system() {
        for (auto& q : queues_)
            q.stop();
        for (auto& th : threads_)
            th.join();
    }

    template <typename F>
    void async(F&& f) {
        auto i = index_++;
        for (unsigned j = 0; j < count_ * 48; j++)
            if (queues_[(i + j) % count_].try_push(std::forward<F>(f)))
                return;
        queues_[index_ % count_].push(std::forward<F>(f));
    }
};
#endif /*TASK_SYSTEM_H*/
