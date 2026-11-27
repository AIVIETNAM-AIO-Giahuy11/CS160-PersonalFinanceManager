#pragma once

#include <string>
#include "core/DataContainer.h"
#include "domain/ExpenseCategory.h"

class ExpenseCategoryManager {
public:
    static bool addExpenseCategory(const std::string &name);
    static bool renameExpenseCategory(int id, const std::string &name);
    static bool deleteExpenseCategory(int id);

    // Throws std::invalid_argument if the ID does not exist.
    static std::string getExpenseCategoryNameFromId(int id);
    static DynamicArray<IdData<ExpenseCategory>> getExpenseCategories();
    static bool isExpenseCategoryInUse(int id);

private:
    static bool isValidName(const std::string &name);
};
