#include "stat/handler.h"
#include "core/Date.h"
#include "core/Currency.h"
#include <exception>

Currency Stat::evaluateIncome(const Income &A, Date d0, Date d1)
{
    if (A.TransDate()<d0 || A.TransDate()>d1) return Currency(0);
    return A.Amount();
}

Currency Stat::evaluateExpense(const Expense &A, Date d0, Date d1)
{
    if (A.TransDate()<d0 || A.TransDate()>d1) return Currency(0);
    return A.Amount();
}