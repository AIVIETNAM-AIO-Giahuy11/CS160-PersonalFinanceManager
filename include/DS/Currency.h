/*
A simple wrapper class for inputting and stylized formatting of the VND currency. 

Input format:
    - Sign character is optional for positive amount
    - You may include optional digit separators (.)
    - You may include a (case insensitive) multiplier suffix (k - thousand; m - million; b - billion)
    Ex: +520.000    42m    67k    251K    -5000

# Supported functions:

*/
#pragma once
#include <string>
#include <iostream>
#include <compare>

class Currency{
private:
    static constexpr std::string SUFFIX = "VND";
    static constexpr char DELIM = '.';
    static constexpr long long MAX_AMOUNT = 1'000'000'000'000'000;

    static bool validateAmount(long long m);
    static bool validateSuffixFormat(const std::string &x, long long &M);
    static bool validateFormat(const std::string &x, long long &M);

    long long M;
public:
    Currency(): M(0) {}
    Currency(long long _M);
    Currency(const std::string &x);

    auto operator<=>(const Currency&) const = default;

    friend std::istream &operator>>(std::istream &fin, Currency &A);
    friend std::ostream &operator<<(std::ostream &fout, const Currency &A);

};