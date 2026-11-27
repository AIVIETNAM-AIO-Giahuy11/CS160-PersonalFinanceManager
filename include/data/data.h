#pragma once
#include "core/DataContainer.h"
#include "domain/general.h"

namespace Data
{
    /* inline keyword for shared variable */
    inline DataContainer<Wallet> wallet;
    inline DataContainer<IncomeSource> incomeSource;
    inline DataContainer<ExpenseCategory> expenseCategory;
    inline DataContainer<Income> income;
    inline DataContainer<Expense> expense;
    inline DataContainer<RecurringIncome> recurringIncome;
    inline DataContainer<RecurringExpense> recurringExpense;

    void serializeData();
    // void deserializeData();
}