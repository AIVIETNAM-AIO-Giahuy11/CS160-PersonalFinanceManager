#pragma once
#include "core/Currency.h"
#include "core/Date.h"
#include "domain/general.h"

namespace Stat{
    Currency evaluateIncome(const Income &A, Date d0, Date d1);
    Currency evaluateExpense(const Expense &A, Date d0, Date d1);
}