/*
To do:
- Add throws
*/

#include "DS/Date.h"
#include <iostream>
#include <string>

inline bool Date::isLeap(const Date &A)
{
    return A.Y % 400 == 0 || (A.Y % 4 == 0 && A.Y % 100 != 0);
}

inline int Date::monthLen(int M, const Date &A)
{
    if (M==2 && isLeap(A)) return 29;
    return MONTH_LEN[M-1];
}

bool Date::validateDate(const Date &A)
{
    if (A.Y < MIN_YEAR || A.Y > MAX_YEAR) return false;
    if (A.M < 1 || A.M > 12) return false;
    int thisMonthLen = monthLen(A.M, A);
    if (A.D < 1 || A.D > thisMonthLen) return false;
    return true;
}

Date::Date(int _day, int _month, int _year): D(_day), M(_month), Y(_year) 
{
    if (!validateDate(*this))
        throw std::invalid_argument("Invalid Date");
}

Date::Date(const std::string &x) /* Format dd-mm-yyyy */
{
    int dash1 = -1, dash2 = -1;
    for (int i = 0; i<(int)x.size(); i++) 
    {
        if (x[i]=='-')
        {
            if (dash1==-1) dash1 = i;
            else if (dash2==-1) dash2 = i;
            else throw std::invalid_argument("Invalid Date formatting");
        }
        else if (x[i]<'0' || x[i]>'9') throw std::invalid_argument("Invalid Date formatting");
    }
    if (dash2==-1) throw std::invalid_argument("Invalid Date formatting");
    if (dash1>2 || (dash2-dash1-1)>2 || ((int)x.size()-dash2-1)>4) throw std::invalid_argument("Invalid Date formatting");
    for (int i = 0; i<dash1; i++) D = D*10 + (x[i]-'0');
    for (int i = dash1+1; i<dash2; i++) M = M*10 + (x[i]-'0');
    for (int i = dash2+1; i<(int)x.size(); i++) Y = Y*10 + (x[i]-'0');
    if (!validateDate(*this))
        throw std::invalid_argument("Invalid Date");
}


int Date::test()
{
    std::cout << validateDate(*this) << '\n';
    return 0;
}