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

    inline const std::string SAVE_PATH = "data.dat";
    inline bool autoSave = true;
    
    void serializeData();
    void deserializeData();
    void saveIfAuto();
}