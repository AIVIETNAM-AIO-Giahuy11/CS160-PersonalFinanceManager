#include "manager/ExpenseCategoryManager.h"
#include "data/data.h"

bool ExpenseCategoryManager::isValidName(const std::string &name)
{
    return name.find_first_not_of(" \t\n\r\f\v") != std::string::npos;
}

bool ExpenseCategoryManager::addExpenseCategory(const std::string &name)
{
    if (!isValidName(name)) return false;
    Data::expenseCategory.addEntry(ExpenseCategory(name));
    Data::saveIfAuto();
    return true;
}

bool ExpenseCategoryManager::renameExpenseCategory(int id, const std::string &name)
{
    if (!Data::expenseCategory.hasEntry(id) || !isValidName(name)) return false;
    Data::expenseCategory.writeEntry(id, ExpenseCategory(name));
    Data::saveIfAuto();
    return true;
}

bool ExpenseCategoryManager::isExpenseCategoryInUse(int id)
{
    for (const auto &entry : Data::expense)
        if (entry.Data.SourceId() == id) return true;
    for (const auto &entry : Data::recurringExpense)
        if (entry.Data.SourceId() == id) return true;
    return false;
}

bool ExpenseCategoryManager::deleteExpenseCategory(int id)
{
    if (!Data::expenseCategory.hasEntry(id) || isExpenseCategoryInUse(id)) return false;
    Data::expenseCategory.eraseEntry(id);
    Data::saveIfAuto();
    return true;
}

std::string ExpenseCategoryManager::getExpenseCategoryNameFromId(int id)
{
    return Data::expenseCategory.getEntry(id).Name();
}

DynamicArray<IdData<ExpenseCategory>> ExpenseCategoryManager::getExpenseCategories()
{
    DynamicArray<IdData<ExpenseCategory>> categories;
    categories << Data::expenseCategory;
    return categories;
}
