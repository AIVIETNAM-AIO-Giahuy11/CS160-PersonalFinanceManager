/* Created on 08/11/26 */
/*
Custom class for storing date in dd-mm-yyyy format.
Valid date range from 01-01-1970 to 31-12-3000

# Supported functions:
## Constructors:
    Date() : Default to 01-01-1970
    Date(int d, int m, int y) : Construct date with integer argument, throw on invalid date
    Date(string formatted_str) : Construct date with string argument (dd-mm-yyyy, 0 paddings are optional), throw on invalid date/format
    Copy, move, assignment, swap operator are implicitly generated
## Stream extractor:
    >> : Reads a string argument, handles invalid data and setting stream state. Invalid reads leave Date unchanged
    << : Output dd-mm-yyyy, without 0 padding
## Comparator:
    6 standard comparison operators supported through default spaceship operator
## Date arithmetic:
    With integer day count:
        +=, -=, +, -, ++, -- (prefix/postfix) supported, throw on out of bound results
    Date difference - : return day count between 2 given date
## Utility:
    today() : Get system's date using <ctime>
*/
#pragma once
#include <string>

class Date{
private:
    static constexpr int MIN_YEAR = 1970, MAX_YEAR = 3000;
    static constexpr int MIN_DAYNUM = 719468, MAX_DAYNUM = 1096032;
    static constexpr int MONTH_LEN[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int Y, M, D; /* MUST BE IN THIS ORDER FOR <=> OVERLOAD */

    static bool isLeap(int Y);
    static int monthLen(int M, int Y);
    static bool validateTuple(int _day, int _month, int _year);
    static bool validateString(const std::string &X, int &_day, int &_month, int &_year);

    static int date2num(const Date &A);
    static Date num2date(int N);
public:
    Date(): Y(MIN_YEAR), M(1), D(1) {}
    Date(int _day, int _month, int _year);
    Date(const std::string &x);

    friend std::istream &operator>>(std::istream &fin, Date &A);
    friend std::ostream &operator<<(std::ostream &fout, const Date &A);

    auto operator<=>(const Date&) const = default;

    int day() const   { return D; }
    int month() const { return M; }
    int year() const  { return Y; }

    Date &operator+=(int k);
    Date &operator-=(int k);
    Date &operator++();
    Date &operator--();
    Date operator++(int);
    Date operator--(int);
    int operator-(const Date &other) const;

    static Date today();
};

Date operator+(Date A, int k);
Date operator-(Date A, int k);
