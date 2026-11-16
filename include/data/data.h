#pragma once
#include "core/DataContainer.h"
#include "domain/general.h"

namespace Data
{
    DataContainer<Wallet> wallet;
    DataContainer<IncomeSource> incomeSource;
    DataContainer<ExpenseCategory> expenseCategory;
    DataContainer<Income> income;
    DataContainer<Expense> expense;
}

void serializeData();
void deserializeData();