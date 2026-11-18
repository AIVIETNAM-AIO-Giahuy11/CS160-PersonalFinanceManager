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

    int _daily_transaction_eval(Date td0, Date td1, Date d0, Date d1);
    int _weekly_transaction_eval(Date td0, Date td1, Date d0, Date d1);

    template<typename T>
    requires std::same_as<T,RecurringIncome> || std::same_as<T,RecurringExpense>
    Currency evaluateRecurringTransaction(const T &A, Date d0, Date d1)
    {
        Date td0 = A.DateStr(), td1 = A.DateEnd();

        int t = 0;
        switch(A.Freq())
        {
            case Frequency::DAILY:
                t = _daily_transaction_eval(td0, td1, d0, d1);
                break;
            case Frequency::WEEKLY:
                t = _weekly_transaction_eval(td0, td1, d0, d1);
                break;
        }
        
        return A.Amount()*t;
    }
}