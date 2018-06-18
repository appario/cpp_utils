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
