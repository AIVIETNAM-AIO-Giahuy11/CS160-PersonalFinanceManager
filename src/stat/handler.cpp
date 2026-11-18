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