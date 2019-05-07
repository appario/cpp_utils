#ifndef lrucache_h
#define lrucache_h

#include <list>
#include <iterator>
#include <unordered_map>

template <typename K, typename V>
class lru_cache_map {
    std::list<K> list_;
    std::unordered_map<K, typename std::list<V>::iterator> map_;
    std::unordered_map<K, V> dict_;
    const int c_cap;

public:
    lru_cache_map(int capacity) : c_cap(capacity) {}

    V get(K key) {
        if (map_.find(key) == map_.end())
            throw std::out_of_range("Key does not exist");
        list_.erase(map_[key]);
        list_.push_front(key);
        map_[key] = list_.begin();
        return dict_[key];
    }

    void put(K key, V value) {
        if (list_.size() > c_cap || (list_.size() == c_cap && map_.find(key) == map_.end())) {
            K last = list_.back();
            list_.erase(map_[last]);
            dict_.erase(last);
            map_.erase(last);
        }
        if (map_.find(key) != map_.end())
            list_.erase(map_[key]);
        dict_[key] = value;
        list_.push_front(key);
        map_[key] = list_.begin();
    }
};

#endif /* lrucache_h */
