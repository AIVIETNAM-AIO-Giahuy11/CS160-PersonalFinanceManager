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
}