/* Created on 08/11/26 */
/*
# Supported functions:

*/
#pragma once
#include <string>

class Date{
private:
    static constexpr int MIN_YEAR = 1970, MAX_YEAR = 3000;
    static constexpr int MONTH_LEN[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int D, M, Y;

    inline bool isLeap(int Y);
    inline int monthLen(int M, int Y);
    bool validateTuple(int _day, int _month, int _year);
    bool validateString(const std::string &X, int &_day, int &_month, int &_year);

public:
    Date(): D(1), M(1), Y(1970) {}
    Date(int _day, int _month, int _year);
    Date(const std::string &x);

    friend std::istream &operator>>(std::istream &fin, Date &A);
    friend std::ostream &operator<<(std::ostream &fout, const Date &A);

    int test();
    
};