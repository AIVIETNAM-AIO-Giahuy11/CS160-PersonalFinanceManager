/*
A wrapper class for inputting and stylized formatting of the VND currency. 

Input format:
    - Amount must be between -10 ^ 16 to +10 ^ 16 (VND)
    - Sign character is optional for positive amount
    - You may include optional digit separators (.)
    - You may include a (case insensitive) multiplier suffix (k - thousand; m - million; b - billion)
    Ex: +520.000    42m    67k    251K    -5000

# Supported functions:
## Constructors:
    Currency() : 0VND
    Currency(long long m) : m VND
    Currency(string str) : Formatted string as described above
 ## Stream extractors:
    >> : read a Currency using the string constructor, handles stream state
    << : output a Currency in +x.xxx.xxxVND format
## Comparator:
    Standard long long comparison operators
## Arithmetic:
    +=, -=, +, - : Currency - Currency addition and subtraction, handles OOB
    *=, * : Multiply a currency by a non-negative time k, handles OOB. (useful for repeating a currency)
*/

#pragma once
#include <string>
#include <iostream>
#include <compare>

class Currency{
private:
    static constexpr std::string SUFFIX = "VND";
    static constexpr char DELIM = '.';
    static constexpr long long MAX_AMOUNT = 10'000'000'000'000'000;

    static bool validateAmount(long long m);
    static bool validateFormat(const std::string &x, long long &M);

    long long M;
public:
    Currency(): M(0) {}
    explicit Currency(long long _M);
    Currency(const std::string &x);

    auto operator<=>(const Currency&) const = default;

    friend std::istream &operator>>(std::istream &fin, Currency &A);
    friend std::ostream &operator<<(std::ostream &fout, const Currency &A);

    Currency &operator+=(const Currency &other);
    Currency &operator-=(const Currency &other);
    Currency &operator*=(long long k);
    Currency operator-();

};

Currency operator+(Currency A, const Currency &B);
Currency operator-(Currency A, const Currency &B);
Currency operator*(Currency A, long long k);