#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <queue>
#include <thread>
#include <vector>
#include <utility>
#include <algorithm>

template <typename T>
class concurrent_queue {
private:
    std::queue<T> the_queue;
    mutable std::mutex the_mutex;
    std::condition_variable the_condition_variable;
    bool is_running = true;

public:
    bool try_push(T const& data) {
        {
            std::unique_lock<std::mutex> lock{the_mutex, std::try_to_lock};
            if (!lock)
                return false;
            the_queue.emplace_back(data);
        }
        the_condition_variable.notify_one();
        return true;
    }

    void push(T const& data) {
        {
            std::unique_lock<std::mutex> lock{the_mutex};
            the_queue.emplace_back(data);
        }
        the_condition_variable.notify_one();
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock{the_mutex};
        return the_queue.empty();
    }

    bool try_pop(T& popped_value) {
        std::unique_lock<std::mutex> lock{the_mutex, std::try_to_lock};
        if (!lock || the_queue.empty())
            return false;
        popped_value = std::move(the_queue.front());
        the_queue.pop();
        return true;
    }

    bool wait_and_pop(T& popped_value) {
        std::unique_lock<std::mutex> lock{the_mutex};
        the_condition_variable.wait(lock, [&] { return !the_queue.empty() || !is_running; });
        if (!is_running)
            return false;
        popped_value = std::move(the_queue.front());
        the_queue.pop();
        return true;
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock{the_mutex};
            is_running = false;
        }
        the_condition_variable.notify_all();
    }
};

#endif /*CONCURRENT_QUEUE_H*/
