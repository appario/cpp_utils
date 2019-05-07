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
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool running_ = true;

public:
    bool try_push(T const& data) {
        {
            std::unique_lock<std::mutex> lock{mutex_, std::try_to_lock};
            if (!lock)
                return false;
            queue_.emplace_back(data);
        }
        cv_.notify_one();
        return true;
    }

    void push(T const& data) {
        {
            std::unique_lock<std::mutex> lock{mutex_};
            queue_.emplace_back(data);
        }
        cv_.notify_one();
    }

    bool empty() const {
        std::unique_lock<std::mutex> lock{mutex_};
        return queue_.empty();
    }

    bool try_pop(T& popped_value) {
        std::unique_lock<std::mutex> lock{mutex_, std::try_to_lock};
        if (!lock || queue_.empty())
            return false;
        popped_value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    bool wait_and_pop(T& popped_value) {
        std::unique_lock<std::mutex> lock{mutex_};
        cv_.wait(lock, [&] { return !queue_.empty() || !running_; });
        if (!running_)
            return false;
        popped_value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock{mutex_};
            running_ = false;
        }
        cv_.notify_all();
    }
};

#endif /*CONCURRENT_QUEUE_H*/
