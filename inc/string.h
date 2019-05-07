#ifndef string_h
#define string_h

#define MAX_STRING_LEN 65535

#include <cstring>

class string_t {
private:
    char* str_ = nullptr;
    size_t str_len_ = 0;

    void reset() {
        if (str_ != nullptr) {
            delete[] str_;
            str_ = nullptr;
            str_len_ = 0;
        }
    }

    const char* alloc_str(size_t len) {
        reset();
        str_len_ = len > MAX_STRING_LEN ? MAX_STRING_LEN : len;
        str_ = new char[str_len_ + 1]();
        return str_;
    }

    void copy_str(const char* other) {
        if (other == nullptr)
            return;
        size_t len = strlen(other);
        alloc_str(len);
        strncpy(str_, other, str_len_);
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
        str_ = other.str_;
        str_len_ = other.str_len_;
        other.str_ = nullptr;
        other.str_len_ = 0;
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
        output.alloc_str(strlen(other) + str_len_);
        std::strncpy(output.str_, str_, str_len_);
        std::strncpy(output.str_ + str_len_, other, output.str_len_ - str_len_);
        copy_str(output);
        return *this;
    }

    string_t& operator+=(string_t& other) {
        *this += other.c_str();
        return *this;
    }

    const char operator[](size_t index) const {
        if (index >= str_len_)
            return 0;
        return str_[index];
    }

    bool operator==(const string_t& other) const {
        if (std::strncmp(str_, other.str_, str_len_) == 0)
            return true;
        return false;
    }

    bool operator!=(const string_t& other) const {
        if (std::strncmp(str_, other.str_, str_len_) != 0)
            return true;
        return false;
    }

    bool operator>(const string_t& other) const {
        if (std::strncmp(str_, other.str_, str_len_) > 0)
            return true;
        return false;
    }

    bool operator<(const string_t& other) const {
        if (std::strncmp(str_, other.str_, str_len_) < 0)
            return true;
        return false;
    }

    bool operator>=(const string_t& other) const {
        if (std::strncmp(str_, other.str_, str_len_) >= 0)
            return true;
        return false;
    }

    bool operator<=(const string_t& other) const {
        if (std::strncmp(str_, other.str_, str_len_) <= 0)
            return true;
        return false;
    }

    operator const char*() const {
        return str_;
    }

    const char* c_str() const {
        return str_;
    }

    size_t length() const {
        return str_len_;
    }
};

#endif /* string_h */
