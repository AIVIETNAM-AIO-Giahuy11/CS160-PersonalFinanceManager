// src/ui/MasterDataView.cpp
#include "ui/MasterDataView.h"
#include "data/data.h"
#include "service/FinanceService.h"
#include "ui/Console.h"

using std::string;

namespace {
using Kind = MasterDataView::Kind;

string titleOf(Kind kind) {
    switch (kind) {
    case Kind::Wallet:          return "Wallets";
    case Kind::IncomeSource:    return "Income Sources";
    case Kind::ExpenseCategory: return "Expense Categories";
    }
    return "";
}

size_t countOf(Kind kind) {
    switch (kind) {
    case Kind::Wallet:          return Data::wallet.size();
    case Kind::IncomeSource:    return Data::incomeSource.size();
    case Kind::ExpenseCategory: return Data::expenseCategory.size();
    }
    return 0;
}

int idAtIndex(Kind kind, size_t idx) {
    switch (kind) {
    case Kind::Wallet:          return Data::wallet.atIndex(idx).Id();
    case Kind::IncomeSource:    return Data::incomeSource.atIndex(idx).Id();
    case Kind::ExpenseCategory: return Data::expenseCategory.atIndex(idx).Id();
    }
    return NULL_ID;
}

string nameOfId(Kind kind, int id) {
    switch (kind) {
    case Kind::Wallet:          return FinanceService::walletName(id);
    case Kind::IncomeSource:    return FinanceService::incomeSourceName(id);
    case Kind::ExpenseCategory: return FinanceService::expenseCategoryName(id);
    }
    return "";
}

bool inUse(Kind kind, int id) {
    switch (kind) {
    case Kind::Wallet:          return FinanceService::isWalletInUse(id);
    case Kind::IncomeSource:    return FinanceService::isIncomeSourceInUse(id);
    case Kind::ExpenseCategory: return FinanceService::isExpenseCategoryInUse(id);
    }
    return false;
}

bool addItem(Kind kind, const string& name) {
    switch (kind) {
    case Kind::Wallet:          return FinanceService::addWallet(name);
    case Kind::IncomeSource:    return FinanceService::addIncomeSource(name);
    case Kind::ExpenseCategory: return FinanceService::addExpenseCategory(name);
    }
    return false;
}

bool renameItem(Kind kind, int id, const string& name) {
    switch (kind) {
    case Kind::Wallet:          return FinanceService::renameWallet(id, name);
    case Kind::IncomeSource:    return FinanceService::renameIncomeSource(id, name);
    case Kind::ExpenseCategory: return FinanceService::renameExpenseCategory(id, name);
    }
    return false;
}

bool deleteItem(Kind kind, int id) {
    switch (kind) {
    case Kind::Wallet:          return FinanceService::deleteWallet(id);
    case Kind::IncomeSource:    return FinanceService::deleteIncomeSource(id);
    case Kind::ExpenseCategory: return FinanceService::deleteExpenseCategory(id);
    }
    return false;
}
} // namespace

namespace MasterDataView {

bool show(Kind kind, string& message) {
    Display::title(titleOf(kind));
    switch (kind) {
    case Kind::Wallet:          Display::itemList(Data::wallet); break;
    case Kind::IncomeSource:    Display::itemList(Data::incomeSource); break;
    case Kind::ExpenseCategory: Display::itemList(Data::expenseCategory); break;
    }

    Display::section("Actions");
    Display::notice("1  Add      2  Rename      3  Delete      0  Back");
    int choice = Input::integer("Action", 0, 3);
    if (choice == 0) return false;

    size_t count = countOf(kind);
    if (choice != 1 && count == 0) {
        message = "No items yet.";
        return false;
    }

    int targetId = NULL_ID;
    if (choice != 1) {
        int pick = Input::integer("Item number", 1, static_cast<int>(count));
        targetId = idAtIndex(kind, static_cast<size_t>(pick - 1));
    }

    if (choice == 3) {
        string oldName = nameOfId(kind, targetId);
        if (inUse(kind, targetId)) {
            message = "Cannot delete \"" + oldName + "\". A transaction or recurring schedule uses this item.";
            return false;
        }
        if (Input::integer("Delete \"" + oldName + "\"?  [1] Delete  [0] Keep", 0, 1) == 0)
            return false;
        deleteItem(kind, targetId);
        message = "Deleted \"" + oldName + "\".";
        return true;
    }

    if (choice == 1 && count >= FinanceService::ITEM_LIMIT) {
        message = "This list is full.";
        return false;
    }

    while (true) {
        string name = Input::line("New name");
        bool changed = (choice == 1) ? addItem(kind, name) : renameItem(kind, targetId, name);
        if (changed) {
            message = string(choice == 1 ? "Added \"" : "Renamed to \"") + name + "\".";
            return true;
        }
        Display::problem("Enter a unique name of 1 to 80 bytes without control characters.");
    }
}

} // namespace MasterDataView