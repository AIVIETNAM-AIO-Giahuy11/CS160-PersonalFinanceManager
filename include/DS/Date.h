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

    int D{}, M{}, Y{};

    inline bool isLeap(const Date &A);
    inline int monthLen(int M, const Date &A);
    bool validateDate(const Date &A);
public:
    Date(int _day = 1, int _month = 1, int _year = 1970);
    Date(const std::string &x);

    int test();
    
};