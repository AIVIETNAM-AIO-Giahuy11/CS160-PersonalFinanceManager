#pragma once
#include "core/Currency.h"
#include "core/Date.h"
#include "domain/general.h"
#include <concepts>

namespace Stat{
    template<typename T>
    requires std::same_as<T,Income> || std::same_as<T,Expense>
    Currency evaluateTransaction(const T &A, Date d0, Date d1)
    {
        if (A.TransDate()<d0 || A.TransDate()>d1) return Currency(0);
        return A.Amount();
    }

    template<typename T>
    requires std::same_as<T,RecurringIncome> || std::same_as<T,RecurringExpense>
    Currency evaluateRecurringTransaction(const T &A, Date d0, Date d1)
    {
        Currency amount = A.Amount();
        Date td0 = A.DateStr(), td1 = A.DateEnd();

        /*
            process here
        */
        return Currency("67k");
    }
}