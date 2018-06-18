#ifndef date_h
#define date_h

#include <time.h>
#include <cstdint>

class date_t {
private:
    tm _date;
    static const time_t seconds_in_one_day = 24 * 60 * 60;
    double time_diff(const date_t& other) {
        time_t lhs_t = mktime(&_date);
        time_t rhs_t = mktime(const_cast<tm*>(&(other._date)));
        return difftime(lhs_t, rhs_t); // rhs_t - lhs_t
    }

public:
    date_t(uint8_t day, uint8_t month, uint32_t year) : date_t() {
        _date.tm_mday = day;
        _date.tm_mon  = month - 1;
        _date.tm_year = year - 1900;
        mktime(&_date);
    }

    date_t() {
        time_t rawtime;
        time(&rawtime);
        _date = *(localtime(&rawtime));
    }

    date_t(const date_t& date) : date_t(date.day(), date.month(), date.year()) {}

    int day() const {
        return _date.tm_mday;
    }

    int month() const {
        return _date.tm_mon + 1;
    }

    int year() const {
        return _date.tm_year + 1900;
    }

    void add_days(int days) {
        time_t date_in_seconds = mktime(&_date);
        date_in_seconds += days * seconds_in_one_day;
        _date = *(localtime(&date_in_seconds));
    }

    bool operator<(const date_t& other) {
        return time_diff(other) < 0;
    }

    bool operator==(const date_t& other) {
        return time_diff(other) == 0;
    }

    bool operator>(const date_t& other) {
        return !(*this < other);
    }

    bool operator!=(const date_t& other) {
        return !(*this == other);
    }
};

#endif /* date_h */
