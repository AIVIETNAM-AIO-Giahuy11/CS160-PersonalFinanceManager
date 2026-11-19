// src/service/RecurringService.cpp
#include "service/RecurringService.h"
#include "service/FinanceService.h"
#include <algorithm>

namespace {
bool isWithinRange(const _recurring_transaction_base& rule, const Date& due) {
    return due >= rule.DateStr() && due <= rule.DateEnd();
}

bool validRecurringCommon(const _recurring_transaction_base& rule) {
    return rule.Amount() > Currency(0)
        && rule.DateEnd() >= rule.DateStr()
        && FinanceService::isValidDescription(rule.Desc())
        && Data::wallet.hasEntry(rule.WalletId());
}

bool alreadyPostedIncome(int ruleId, const Date& due) {
    for (const auto& e : Data::income) {
        if (e.Data.RuleId() == ruleId && e.Data.TransDate() == due) return true;
    }
    return false;
}

bool alreadyPostedExpense(int ruleId, const Date& due) {
    for (const auto& e : Data::expense) {
        if (e.Data.RuleId() == ruleId && e.Data.TransDate() == due) return true;
    }
    return false;
}
} // namespace

namespace RecurringService {

Date scheduledDateForMonth(const Date& startDate, int year, int month) {
    int maxDay = Date::monthLen(month, year);
    int day = std::min(startDate.day(), maxDay);
    return Date(day, month, year);
}

bool addRecurringIncome(const RecurringIncome& rule) {
    if (Data::recurringIncome.size() + Data::recurringExpense.size() >= FinanceService::RULE_LIMIT)
        return false;
    if (!validRecurringCommon(rule) || !Data::incomeSource.hasEntry(rule.SourceId()))
        return false;
    Data::recurringIncome.addEntry(rule);
    return true;
}

bool addRecurringExpense(const RecurringExpense& rule) {
    if (Data::recurringIncome.size() + Data::recurringExpense.size() >= FinanceService::RULE_LIMIT)
        return false;
    if (!validRecurringCommon(rule) || !Data::expenseCategory.hasEntry(rule.SourceId()))
        return false;
    Data::recurringExpense.addEntry(rule);
    return true;
}

bool deleteRecurringIncome(int id) {
    if (!Data::recurringIncome.hasEntry(id)) return false;
    Data::recurringIncome.eraseEntry(id);
    return true;
}

bool deleteRecurringExpense(int id) {
    if (!Data::recurringExpense.hasEntry(id)) return false;
    Data::recurringExpense.eraseEntry(id);
    return true;
}

int syncForCurrentMonth(const Date& today) {
    DynamicArray<Income> pendingIncomes;
    DynamicArray<Expense> pendingExpenses;

    for (const auto& entry : Data::recurringIncome) {
        const RecurringIncome& rule = entry.Data;
        Date due = scheduledDateForMonth(rule.DateStr(), today.year(), today.month());
        if (due > today || !isWithinRange(rule, due) || alreadyPostedIncome(entry.Id(), due))
            continue;
        pendingIncomes.push_back(Income(due, rule.Amount(), rule.SourceId(), rule.WalletId(), rule.Desc(), entry.Id()));
    }

    for (const auto& entry : Data::recurringExpense) {
        const RecurringExpense& rule = entry.Data;
        Date due = scheduledDateForMonth(rule.DateStr(), today.year(), today.month());
        if (due > today || !isWithinRange(rule, due) || alreadyPostedExpense(entry.Id(), due))
            continue;
        pendingExpenses.push_back(Expense(due, rule.Amount(), rule.SourceId(), rule.WalletId(), rule.Desc(), entry.Id()));
    }

    size_t totalCurrent = Data::income.size() + Data::expense.size();
    size_t totalPending = pendingIncomes.size() + pendingExpenses.size();
    if (totalCurrent + totalPending > FinanceService::TRANSACTION_LIMIT)
        return -1;

    for (const auto& tx : pendingIncomes)  Data::income.addEntry(tx);
    for (const auto& tx : pendingExpenses) Data::expense.addEntry(tx);

    return static_cast<int>(totalPending);
}

} // namespace RecurringService