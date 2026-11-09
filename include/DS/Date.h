/* Created on 08/11/26 */
/*
# Supported functions:

*/
#pragma once
#include <string>
#include <compare>

class Date{
private:
    static constexpr int MIN_YEAR = 1970, MAX_YEAR = 3000;
    static constexpr int MONTH_LEN[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int Y, M, D; /* MUST BE IN THIS ORDER FOR <=> OVERLOAD */

    inline bool isLeap(int Y);
    inline int monthLen(int M, int Y);
    bool validateTuple(int _day, int _month, int _year);
    bool validateString(const std::string &X, int &_day, int &_month, int &_year);

    int date2num(const Date &A);
    Date num2date(int N);
public:
    Date(): D(1), M(1), Y(1970) {}
    Date(int _day, int _month, int _year);
    Date(const std::string &x);

    friend std::istream &operator>>(std::istream &fin, Date &A);
    friend std::ostream &operator<<(std::ostream &fout, const Date &A);

    auto operator<=>(const Date&) const = default;

    inline int day() const;
    inline int month() const;
    inline int year() const;

    Date operator+(int k);
    Date operator-(int k);
    int operator-(const Date &other);

    int test();
    
};
