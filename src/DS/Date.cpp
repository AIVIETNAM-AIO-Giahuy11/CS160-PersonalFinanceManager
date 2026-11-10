#include "DS/Date.h"
#include <iostream>
#include <stdexcept>
#include <ctime>

/* Helper functions */

bool Date::isLeap(int Y)
{
    return Y % 400 == 0 || (Y % 4 == 0 && Y % 100 != 0);
}

int Date::monthLen(int M, int Y)
{
    if (M==2 && isLeap(Y)) return 29;
    return MONTH_LEN[M-1];
}

bool Date::validateTuple(int _day, int _month, int _year)
{
    if (_year < MIN_YEAR || _year > MAX_YEAR) return false;
    if (_month < 1 || _month > 12) return false;
    if (_day < 1 || _day > monthLen(_month, _year)) return false;
    return true;
}

bool Date::validateString(const std::string &X, int &_day, int &_month, int &_year)
{
    int dash1 = -1, dash2 = -1;
    for (int i = 0; i<(int)X.size(); i++) /* Search the string for <=2 dash and deny any other non digits */
    {
        if (X[i]=='-')
        {
            if (dash1==-1) dash1 = i;
            else if (dash2==-1) dash2 = i;
            else return false;
        }
        else if (X[i]<'0' || X[i]>'9') return false;
    }
    if (dash2==-1) return false; /* <2 dash */
    if (dash1>2 || (dash2-dash1-1)>2 || ((int)X.size()-dash2-1)>4) return false; /* Deny if digits are too long */

    /* Extract digits */
    for (int i = 0; i<dash1; i++) _day = _day*10 + (X[i]-'0');
    for (int i = dash1+1; i<dash2; i++) _month = _month*10 + (X[i]-'0');
    for (int i = dash2+1; i<(int)X.size(); i++) _year = _year*10 + (X[i]-'0');
    return true;
}

/* Date to #day since march 1st 0000 conversion */
/* Credit: Howard Hinnant, other */
int Date::date2num(const Date &A)
{
    int d = A.day()-1;
    int m = (A.month()<3? A.month()+9 : A.month()-3);
    int y = (A.month()<3? A.year()-1 : A.year());
    return y*365 + y/4 - y/100 + y/400 + (153*m+2)/5 + d;
}

Date Date::num2date(int N)
{
    int t;
    int y = 400*(N/146097); N%=146097;

    t = std::min(N/36524, 3);
    y+= 100*t; N-= 36524 * t;

    y+= 4*(N/1461); N%=1461;

    t = std::min(N/365, 3);
    y+= t; N-=365 * t;

    int m = (5*N+2)/153;
    int d = N - (153*m+2)/5;
    y+= (m>=10); m = (m>=10? m-9 : m+3); d++;
    return Date(d, m, y);
}
/*--------------------------------------------*/

/* Constructors */

Date::Date(int _day, int _month, int _year)
{  
    if (!validateTuple(_day, _month, _year))
        throw std::invalid_argument("Invalid Date (value)");
    D = _day; M = _month; Y = _year;
}

Date::Date(const std::string &x) /* Format dd-mm-yyyy */
{
    int _day{}, _month{}, _year{};
    if (!validateString(x, _day, _month, _year))
        throw std::invalid_argument("Invalid Date (format)");
    if (!validateTuple(_day, _month, _year))
        throw std::invalid_argument("Invalid Date (value)");
    D = _day; M = _month; Y = _year;
}

/* Stream Extractors */

std::istream &operator>>(std::istream &fin, Date &A)
{
    std::string UserInput;
    
    if (fin >> UserInput)
    {
        try{
            A = Date(UserInput);
        }
        catch (const std::exception& e) {
            fin.setstate(std::ios_base::failbit);
        }
    }
    return fin;
}

std::ostream &operator<<(std::ostream &fout, const Date &A)
{
    fout << A.D << '-' << A.M << '-' << A.Y;
    return fout;
}

/* Arithmetic */
Date &Date::operator+=(int k)
{
    int t = (date2num(*this) + k);
    if (t<MIN_DAYNUM || t>MAX_DAYNUM)
        throw std::out_of_range("OOB Date operation");
    *this = num2date(t);
    return *this;
}

Date &Date::operator-=(int k)
{
    *this+=(-k);
    return *this;
}

Date operator+(Date A, int k) { A+=k; return A; }
Date operator-(Date A, int k) { A-=k; return A; }
Date &Date::operator++() { *this+=1; return *this; }
Date &Date::operator--() { *this-=1; return *this; }
Date Date::operator++(int)
{
    Date tmp = *this;
    ++*this;
    return tmp;
}
Date Date::operator--(int)
{
    Date tmp = *this;
    --*this;
    return tmp;
}

int Date::operator-(const Date &other) const
{
    return date2num(*this) - date2num(other);
}

/* Helper function to get today's date using <ctime> */
Date Date::today()
{
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);      // MSVC and MinGW-w64 (note: argument order differs from C11)
#else
    localtime_r(&t, &tm);      // POSIX, thread-safe
#endif
    return Date(tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}