#ifndef lrucache_h
#define lrucache_h

#include <list>
#include <iterator>
#include <unordered_map>

template <typename K, typename V>
class lru_cache_map {
    std::list<K> _l;
    std::unordered_map<K, typename std::list<V>::iterator> _m;
    std::unordered_map<K, V> _dict;
    const int c_cap;

public:
    lru_cache_map(int capacity) : c_cap(capacity) {}

    V get(K key) {
        if (_m.find(key) == _m.end())
            throw std::out_of_range("Key does not exist");
        _l.erase(_m[key]);
        _l.push_front(key);
        _m[key] = _l.begin();
        return _dict[key];
    }

    void put(K key, V value) {
        if (_l.size() > c_cap || (_l.size() == c_cap && _m.find(key) == _m.end())) {
            K last = _l.back();
            _l.erase(_m[last]);
            _dict.erase(last);
            _m.erase(last);
        }
        if (_m.find(key) != _m.end())
            _l.erase(_m[key]);
        _dict[key] = value;
        _l.push_front(key);
        _m[key] = _l.begin();
    }
};

#endif /* lrucache_h */
