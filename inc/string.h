#ifndef string_h
#define string_h

#define MAX_STRING_LEN 65535

#include <cstring>

class string_t {
private:
    char* _str      = nullptr;
    size_t _str_len = 0;

    void reset() {
        if (_str != nullptr) {
            delete[] _str;
            _str     = nullptr;
            _str_len = 0;
        }
    }

    const char* alloc_str(size_t len) {
        reset();
        _str_len = len > MAX_STRING_LEN ? MAX_STRING_LEN : len;
        _str     = new char[_str_len + 1]();
        return _str;
    }

    void copy_str(const char* other) {
        if (other == nullptr)
            return;
        size_t len = strlen(other);
        alloc_str(len);
        strncpy(_str, other, _str_len);
    }

public:
    string_t() {}

    string_t(const char* other) {
        copy_str(other);
    }

    string_t(string_t& other) {
        copy_str(other);
    }

    string_t(string_t&& other) {
        _str           = other._str;
        _str_len       = other._str_len;
        other._str     = nullptr;
        other._str_len = 0;
    }

    ~string_t() {
        reset();
    }

    string_t& operator=(string_t other) {
        copy_str(other.c_str());
        return *this;
    }

    string_t& operator+=(const char* other) {
        string_t output;
        output.alloc_str(strlen(other) + _str_len);
        std::strncpy(output._str, _str, _str_len);
        std::strncpy(output._str + _str_len, other, output._str_len - _str_len);
        copy_str(output);
        return *this;
    }

    string_t& operator+=(string_t& other) {
        *this += other.c_str();
        return *this;
    }

    const char operator[](size_t index) const {
        if (index >= _str_len)
            return 0;
        return _str[index];
    }

    bool operator==(const string_t& other) const {
        if (std::strncmp(_str, other._str, _str_len) == 0)
            return true;
        return false;
    }

    bool operator!=(const string_t& other) const {
        if (std::strncmp(_str, other._str, _str_len) != 0)
            return true;
        return false;
    }

    bool operator>(const string_t& other) const {
        if (std::strncmp(_str, other._str, _str_len) > 0)
            return true;
        return false;
    }

    bool operator<(const string_t& other) const {
        if (std::strncmp(_str, other._str, _str_len) < 0)
            return true;
        return false;
    }

    bool operator>=(const string_t& other) const {
        if (std::strncmp(_str, other._str, _str_len) >= 0)
            return true;
        return false;
    }

    bool operator<=(const string_t& other) const {
        if (std::strncmp(_str, other._str, _str_len) <= 0)
            return true;
        return false;
    }

    operator const char*() const {
        return _str;
    }

    const char* c_str() const {
        return _str;
    }

    size_t length() const {
        return _str_len;
    }
};

#endif /* string_h */
