// src/ui/TransactionView.cpp
#include "ui/TransactionView.h"
#include "core/Date.h"
#include "data/data.h"
#include "service/FinanceService.h"
#include "ui/Console.h"
#include <algorithm>
#include <iostream>

using std::cout;
using std::string;

namespace {
void printTransaction(int rowNum, const FinanceService::TransactionItem& tx) {
    string wName = FinanceService::walletName(tx.walletId);
    string cName = tx.isIncome
        ? FinanceService::incomeSourceName(tx.sourceId)
        : FinanceService::expenseCategoryName(tx.sourceId);
    string amt = (tx.isIncome ? "+" : "-") + Format::money(tx.amount);

    cout << "  " << Format::ink(std::to_string(rowNum) + ".", "accent") << " "
         << Format::ink(tx.date.toPaddedString(), "muted") << "   "
         << Format::ink(amt, tx.isIncome ? "income" : "expense")
         << "   " << cName << "  ·  " << wName;
    if (tx.ruleId != NULL_ID) cout << Format::ink("  ↻", "accent");
    cout << '\n';
    if (!tx.description.empty())
        cout << "       " << Format::ink(tx.description, "muted") << '\n';
}

bool askTransactionFields(bool& isIncome, Date& date, int& walletId,
                          int& sourceId, Currency& amount, string& desc,
                          bool askType, string& message) {
    if (Data::wallet.empty()) {
        message = "Add a wallet before recording a transaction.";
        return false;
    }

    if (askType) {
        Display::notice("1  Income       2  Expense       0  Back");
        int type = Input::integer("Transaction type", 0, 2);
        if (type == 0) return false;
        isIncome = (type == 1);
    }

    if (isIncome && Data::incomeSource.empty()) {
        message = "Add an income source before recording income.";
        return false;
    }
    if (!isIncome && Data::expenseCategory.empty()) {
        message = "Add an expense category before recording an expense.";
        return false;
    }

    date = Input::date("Transaction date", false, Date::today());
    walletId = Input::chooseItem(Data::wallet, "Wallet");
    sourceId = isIncome
        ? Input::chooseItem(Data::incomeSource, "Income source")
        : Input::chooseItem(Data::expenseCategory, "Expense category");
    amount = Input::amount();
    desc = Input::description();
    return true;
}

bool editSelectedTransaction(const FinanceService::TransactionItem& tx, string& message) {
    Display::title(tx.isIncome ? "Edit income" : "Edit expense");
    bool isIncome = tx.isIncome;
    Date date;
    int walletId = NULL_ID, sourceId = NULL_ID;
    Currency amount;
    string desc;

    if (!askTransactionFields(isIncome, date, walletId, sourceId, amount, desc, false, message))
        return false;

    bool ok = isIncome
        ? FinanceService::updateIncome(tx.id, Income(date, amount, sourceId, walletId, desc, tx.ruleId))
        : FinanceService::updateExpense(tx.id, Expense(date, amount, sourceId, walletId, desc, tx.ruleId));

    message = ok ? "Transaction updated." : "Could not update this transaction.";
    return ok;
}
} // namespace

namespace TransactionView {

bool recordTransaction(string& message) {
    Display::title("Add transaction");
    bool isIncome = true;
    Date date;
    int walletId = NULL_ID, sourceId = NULL_ID;
    Currency amount;
    string desc;

    if (!askTransactionFields(isIncome, date, walletId, sourceId, amount, desc, true, message))
        return false;

    bool ok = isIncome
        ? FinanceService::addIncome(Income(date, amount, sourceId, walletId, desc))
        : FinanceService::addExpense(Expense(date, amount, sourceId, walletId, desc));

    if (!ok) {
        message = "Could not record this transaction. Check the fields and transaction limit.";
        return false;
    }

    message = "Recorded " + Format::money(amount)
            + (isIncome ? " in income for " : " in expenses for ")
            + FinanceService::walletName(walletId) + ".";
    return true;
}

bool showHistory(string& message) {
    constexpr int PAGE_SIZE = 6;
    int page = 0;

    while (true) {
        Display::title("Transaction history");
        DynamicArray<FinanceService::TransactionItem> items = FinanceService::getAllTransactionsSorted();

        if (items.empty()) {
            Display::notice("No transactions yet. Choose Add Transaction on the dashboard to record one.");
            Input::pause();
            return false;
        }

        int total = static_cast<int>(items.size());
        int pages = (total + PAGE_SIZE - 1) / PAGE_SIZE;
        if (page >= pages) page = pages - 1;

        int start = page * PAGE_SIZE;
        int end = std::min(total, start + PAGE_SIZE);

        Display::section("Newest first  ·  Page " + std::to_string(page + 1) + "/" + std::to_string(pages));
        for (int i = start; i < end; ++i)
            printTransaction(i - start + 1, items[static_cast<size_t>(i)]);

        Display::section("Actions");
        Display::notice("1  Older    2  Newer    3  Edit    4  Delete    0  Back");
        int action = Input::integer("Select", 0, 4);

        if (action == 0) return false;
        if (action == 1) { if (page + 1 < pages) ++page; continue; }
        if (action == 2) { if (page > 0) --page; continue; }

        int pick = Input::integer("Row number on this page", 1, end - start);
        const auto& target = items[static_cast<size_t>(start + pick - 1)];

        if (action == 3) {
            return editSelectedTransaction(target, message);
        }
        if (action == 4) {
            if (Input::integer("Delete this transaction? [1] Delete  [0] Keep", 0, 1) == 0)
                continue;
            bool ok = target.isIncome
                ? FinanceService::deleteIncome(target.id)
                : FinanceService::deleteExpense(target.id);
            message = ok ? "Transaction deleted." : "Could not delete transaction.";
            return ok;
        }
    }
}

} // namespace TransactionView