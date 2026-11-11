#include "DS/Currency.h"
#include <stdexcept>
#include <utility>
#include <string>
#include <algorithm>

/* Helper functions */
bool Currency::validateAmount(long long m)
{
    if (m < -MAX_AMOUNT || m > MAX_AMOUNT)
        return false;
    return true;
}

int getSign(const std::string &x)
{
    int c = 0;
    for (int i = 0; i<(int)x.size(); i++) if (x[i]=='+' || x[i]=='-')
        c++;
    if (c>1 || (c==1 && x[0]!='+' && x[0]!='-')) return 0;
    if (x[0]=='-') return -1;
    return 1;
}

long long getMul(const std::string &x)
{
    long long ret = 1;
    switch (x.back())
    {
    case('k'): case('K'):
        ret = 1'000;
        break;
    case('m'): case('M'):
        ret = 1'000'000;
        break;
    case('b'): case('B'):
        ret = 1'000'000'000;
        break;
    default:
        break;
    }
    return ret;
}

bool Currency::validateFormat(const std::string &x, long long &M)
{
    int sign = getSign(x);
    if (sign==0) return false;
    long long tmp = 0, mul = getMul(x);

    for (int i = 0; i<(int)x.size(); i++)
    {
        if (mul!=1 && i==(int)x.size()-1) continue;
        if (x[i]=='+' || x[i]=='-') continue;
        if (x[i]==Currency::DELIM) continue;
        if (x[i]<'0' || x[i]>'9') return false; /* Not sign, not delimiter, not digits */
        tmp*=10;
        tmp+= x[i]-'0';
        tmp = std::min(tmp, MAX_AMOUNT+1);
    }
    if (tmp>MAX_AMOUNT/mul) tmp = MAX_AMOUNT+1;
    else tmp*=mul;
    M = tmp * sign;
    return true;
}

/* Constructors */
Currency::Currency(long long _M)
{
    if (!validateAmount(M))
        throw std::invalid_argument("Invalid currency (value)");
    M = _M;
}

Currency::Currency(const std::string &x)
{
    long long tmp = 0;
    if (!validateFormat(x, tmp))
        throw std::invalid_argument("Invalid currency (format)");
    if (!validateAmount(tmp))
        throw std::invalid_argument("Invalid currency (value)");
    M = tmp;
}

/* Stream Extractors */
std::istream &operator>>(std::istream &fin, Currency &A)
{
    std::string UserInput;
    if (fin >> UserInput)
    {
        try{
            A = Currency(UserInput);
        }
        catch (const std::exception& e) {
            fin.setstate(std::ios_base::failbit);
        }
    }
    return fin;
}

std::ostream &operator<<(std::ostream &fout, const Currency &A)
{
    std::string ret = "";
    long long M = A.M;
    if (M>0) fout << '+';
    else if (M<0) 
    {
        fout << '-';
        M = -M;
    }
    else fout << "0";

    int c = 0;
    while (M)
    {
        ret+= char(M%10 + '0');
        M/=10;
        c++;
        if (M && c%3==0) ret+= Currency::DELIM;
    }
    std::reverse(ret.begin(), ret.end());
    fout << ret << Currency::SUFFIX;
    return fout;
}

/* Arithmetic */
Currency &Currency::operator+=(const Currency &other)
{
    if (std::abs(this->M+other.M) > MAX_AMOUNT)
        throw std::out_of_range("OOB Currency operation");
    this->M+= other.M;
    return *this;
}

Currency &Currency::operator-=(const Currency &other)
{
    if (std::abs(this->M-other.M) > MAX_AMOUNT)
        throw std::out_of_range("OOB Currency operation");
    this->M-= other.M;
    return *this;
}

Currency &Currency::operator*=(long long k)
{
    if (k<0)
        throw std::out_of_range("OOB Currency operation");
    if (k>0 && std::abs(this->M) > MAX_AMOUNT/k)
        throw std::out_of_range("OOB Currency operation");
    this->M*=k;
    return *this;
}

Currency Currency::operator-()
{
    return Currency(-M);
}

Currency operator+(Currency A, const Currency &B) { A+=B; return A; }
Currency operator-(Currency A, const Currency &B) { A-=B; return A;}
Currency operator*(Currency A, long long k) { A*=k; return A;}