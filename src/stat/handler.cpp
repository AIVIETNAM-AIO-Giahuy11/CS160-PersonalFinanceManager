#include "stat/handler.h"
#include "core/Date.h"
#include "core/Currency.h"
#include <exception>
#include <algorithm>

int Stat::_daily_transaction_eval(Date td0, Date td1, Date d0, Date d1)
{
    Date id0 = std::max(td0, d0), id1 = std::min(td1, d1); // intersected date
    if (id0>id1) return 0;
    return id1-id0+1;
}

int Stat::_weekly_transaction_eval(Date td0, Date td1, Date d0, Date d1)
{
    Date id0 = std::max(td0, d0), id1 = std::min(td1, d1); // intersected date
    if (id0>id1) return 0;
    return ((id1-td0+7)/7) - ((id0-td0+6)/7); // count #day congruent with td0 modulo 7
}

Date getTruncatedDate(int D, int M, int Y) // normalize to end of month if date exceed maximum day count of that month
{
    D = std::min(Date::monthLen(M, Y), D);
    return Date(D, M, Y);
}

int Stat::_yearly_transaction_eval(Date td0, Date td1, Date d0, Date d1)
{
    Date id0 = std::max(td0, d0), id1 = std::min(td1, d1); // intersected date
    if (id0>id1) return 0;
    Date spec = getTruncatedDate(td0.day(), td0.month(), id0.year()); // date on year id0
    int ret = 0;
    if (spec>=id0 && spec<=id1) ret++;
    if (id0.year() < id1.year())
    {
        spec = getTruncatedDate(td0.day(), td0.month(), id1.year()); // date on year id1
        if (spec>=id0 && spec<=id1) ret++;
        ret+= id1.year() - id0.year() - 1; // years inbetween
    }
    return ret;
}