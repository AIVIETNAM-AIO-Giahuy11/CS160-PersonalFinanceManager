// src/ui/RecurringView.cpp
#include "ui/RecurringView.h"
#include "core/Date.h"
#include "core/DynamicArray.h"
#include "data/data.h"
#include "service/FinanceService.h"
#include "service/RecurringService.h"
#include "ui/Console.h"
#include <iostream>

using std::cout;
using std::string;

namespace {
struct RuleRef {
    int id = NULL_ID;
    bool isIncome = true;
};

void printRuleRow(int rowNum, bool isIncome, const _recurring_transaction_base& r) {
    string wName = FinanceService::walletName(r.WalletId());
    string cName = isIncome
        ? FinanceService::incomeSourceName(r.SourceId())
        : FinanceService::expenseCategoryName(r.SourceId());
    string amt = (isIncome ? "+" : "-") + Format::money(r.Amount());
    string endStr = r.hasEndDate() ? r.DateEnd().toPaddedString() : "No end date";

    cout << "  " << Format::ink(std::to_string(rowNum) + ".", "accent") << " "
         << Format::ink("Day " + std::to_string(r.DateStr().day()) + " monthly", "muted") << "   "
         << Format::ink(amt, isIncome ? "income" : "expense")
         << "   " << cName << "  ·  " << wName << Format::ink("  ↻", "accent") << '\n';
    cout << "       " << Format::ink(r.DateStr().toPaddedString() + " -> " + endStr, "muted") << '\n';
    if (!r.Desc().empty())
        cout << "       " << Format::ink(r.Desc(), "muted") << '\n';
}

bool addSchedule(string& message) {
    if (Data::wallet.empty()) {
        message = "Add a wallet before creating a recurring schedule.";
        return false;
    }

    Display::notice("1  Recurring Income       2  Recurring Expense       0  Back");
    int type = Input::integer("Schedule type", 0, 2);
    if (type == 0) return false;
    bool isIncome = (type == 1);

    if (isIncome && Data::incomeSource.empty()) {
        message = "Add an income source before creating a recurring income.";
        return false;
    }
    if (!isIncome && Data::expenseCategory.empty()) {
        message = "Add an expense category before creating a recurring expense.";
        return false;
    }

    int walletId = Input::chooseItem(Data::wallet, "Wallet");
    int sourceId = isIncome
        ? Input::chooseItem(Data::incomeSource, "Income source")
        : Input::chooseItem(Data::expenseCategory, "Expense category");
    Currency amount = Input::amount();
    string desc = Input::description();
    Date start = Input::date("Start date", false, Date::today());
    Date end = Input::date("End date", true, Date(31, 12, Date::MAX_YEAR), start);

    bool added = isIncome
        ? RecurringService::addRecurringIncome(RecurringIncome(Frequency::MONTHLY, start, end, amount, sourceId, walletId, desc))
        : RecurringService::addRecurringExpense(RecurringExpense(Frequency::MONTHLY, start, end, amount, sourceId, walletId, desc));

    if (!added) {
        message = "Could not add this schedule. Check the details and schedule limit.";
        return false;
    }

    int posted = RecurringService::syncForCurrentMonth(Date::today());
    if (posted < 0) {
        message = "Schedule saved, but transaction limit prevented posting this month's entry.";
    } else if (posted > 0) {
        message = "Schedule saved. This month's transaction has been posted automatically.";
    } else {
        message = "Recurring schedule saved.";
    }
    return true;
}
} // namespace

namespace RecurringView {

bool show(string& message) {
    Display::title("Monthly recurring transactions");

    DynamicArray<RuleRef> refs;
    int row = 1;
    for (const auto& entry : Data::recurringIncome) {
        printRuleRow(row++, true, entry.Data);
        refs.push_back(RuleRef{entry.Id(), true});
    }
    for (const auto& entry : Data::recurringExpense) {
        printRuleRow(row++, false, entry.Data);
        refs.push_back(RuleRef{entry.Id(), false});
    }
    if (refs.empty()) Display::notice("No recurring schedules yet.");

    Display::section("Actions");
    Display::notice("1  Add schedule      2  Delete schedule      0  Back");
    int action = Input::integer("Action", 0, 2);
    if (action == 0) return false;
    if (action == 1) return addSchedule(message);

    if (refs.empty()) {
        message = "No schedules to delete.";
        return false;
    }

    int pick = Input::integer("Schedule number", 1, static_cast<int>(refs.size()));
    const RuleRef& target = refs[static_cast<size_t>(pick - 1)];
    if (Input::integer("Delete this schedule? [1] Delete  [0] Keep", 0, 1) == 0)
        return false;

    bool ok = target.isIncome
        ? RecurringService::deleteRecurringIncome(target.id)
        : RecurringService::deleteRecurringExpense(target.id);
    message = ok ? "Recurring schedule deleted." : "Could not delete schedule.";
    return ok;
}

} // namespace RecurringView