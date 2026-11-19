// src/service/FinanceService.cpp
#include "service/FinanceService.h"
#include <algorithm>

namespace {
std::string toLowerKey(const std::string& raw) {
    std::string key = FinanceService::trimText(raw);
    for (char& c : key) {
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }
    return key;
}

template <typename T>
bool isAllowedName(const DataContainer<T>& list, const std::string& name, int exceptId) {
    std::string trimmed = FinanceService::trimText(name);
    if (trimmed.empty() || trimmed.size() > FinanceService::NAME_LIMIT) return false;
    if (!FinanceService::isPrintable(trimmed)) return false;

    std::string key = toLowerKey(trimmed);
    for (const auto& entry : list) {
        if (entry.Id() != exceptId && toLowerKey(entry.Data.Name()) == key)
            return false;
    }
    return true;
}

template <typename T>
bool addCategoryImpl(DataContainer<T>& list, const std::string& name) {
    if (list.size() >= FinanceService::ITEM_LIMIT) return false;
    if (!isAllowedName(list, name, NULL_ID)) return false;
    list.addEntry(T(FinanceService::trimText(name)));
    return true;
}

template <typename T>
bool renameCategoryImpl(DataContainer<T>& list, int id, const std::string& name) {
    if (!list.hasEntry(id)) return false;
    if (!isAllowedName(list, name, id)) return false;
    T updated = list.getEntry(id);
    updated.setName(FinanceService::trimText(name));
    list.writeEntry(id, updated);
    return true;
}

template <typename T>
int findIndexById(const DataContainer<T>& list, int id) {
    int l = 0, r = static_cast<int>(list.size());
    while (l < r) {
        int mid = (l + r) >> 1;
        int curId = list.atIndex(static_cast<size_t>(mid)).Id();
        if (curId < id) l = mid + 1;
        else if (curId > id) r = mid;
        else return mid;
    }
    return -1;
}

bool validIncome(const Income& tx) {
    return tx.Amount() > Currency(0)
        && FinanceService::isValidDescription(tx.Desc())
        && Data::wallet.hasEntry(tx.WalletId())
        && Data::incomeSource.hasEntry(tx.SourceId());
}

bool validExpense(const Expense& tx) {
    return tx.Amount() > Currency(0)
        && FinanceService::isValidDescription(tx.Desc())
        && Data::wallet.hasEntry(tx.WalletId())
        && Data::expenseCategory.hasEntry(tx.SourceId());
}

FinanceService::TransactionItem toViewItem(int id, const Income& tx) {
    return {id, true, tx.TransDate(), tx.Amount(), tx.SourceId(), tx.WalletId(), tx.Desc(), tx.RuleId()};
}

FinanceService::TransactionItem toViewItem(int id, const Expense& tx) {
    return {id, false, tx.TransDate(), tx.Amount(), tx.SourceId(), tx.WalletId(), tx.Desc(), tx.RuleId()};
}
} // namespace

namespace FinanceService {

std::string trimText(const std::string& text) {
    const char* ws = " \t\r\n\f\v";
    size_t start = text.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = text.find_last_not_of(ws);
    return text.substr(start, end - start + 1);
}

bool isPrintable(const std::string& text) {
    for (unsigned char c : text) {
        if (c < 32 || c == 127) return false;
    }
    return true;
}

bool isValidDescription(const std::string& desc) {
    return desc.size() <= DESCRIPTION_LIMIT && isPrintable(desc);
}

void seedDefaultMasterData() {
    if (Data::wallet.empty()) {
        addWallet("Cash");
        addWallet("Bank Account");
    }
    if (Data::incomeSource.empty()) {
        addIncomeSource("Salary");
        addIncomeSource("Bonus");
    }
    if (Data::expenseCategory.empty()) {
        addExpenseCategory("Food & Dining");
        addExpenseCategory("Transport");
        addExpenseCategory("Housing");
    }
}

bool addWallet(const std::string& name) { return addCategoryImpl(Data::wallet, name); }
bool renameWallet(int id, const std::string& name) { return renameCategoryImpl(Data::wallet, id, name); }

bool isWalletInUse(int id) {
    for (const auto& e : Data::income)           if (e.Data.WalletId() == id) return true;
    for (const auto& e : Data::expense)          if (e.Data.WalletId() == id) return true;
    for (const auto& e : Data::recurringIncome)  if (e.Data.WalletId() == id) return true;
    for (const auto& e : Data::recurringExpense) if (e.Data.WalletId() == id) return true;
    return false;
}

bool deleteWallet(int id) {
    if (!Data::wallet.hasEntry(id) || isWalletInUse(id)) return false;
    Data::wallet.eraseEntry(id);
    return true;
}

bool addIncomeSource(const std::string& name) { return addCategoryImpl(Data::incomeSource, name); }
bool renameIncomeSource(int id, const std::string& name) { return renameCategoryImpl(Data::incomeSource, id, name); }

bool isIncomeSourceInUse(int id) {
    for (const auto& e : Data::income)          if (e.Data.SourceId() == id) return true;
    for (const auto& e : Data::recurringIncome) if (e.Data.SourceId() == id) return true;
    return false;
}

bool deleteIncomeSource(int id) {
    if (!Data::incomeSource.hasEntry(id) || isIncomeSourceInUse(id)) return false;
    Data::incomeSource.eraseEntry(id);
    return true;
}

bool addExpenseCategory(const std::string& name) { return addCategoryImpl(Data::expenseCategory, name); }
bool renameExpenseCategory(int id, const std::string& name) { return renameCategoryImpl(Data::expenseCategory, id, name); }

bool isExpenseCategoryInUse(int id) {
    for (const auto& e : Data::expense)          if (e.Data.SourceId() == id) return true;
    for (const auto& e : Data::recurringExpense) if (e.Data.SourceId() == id) return true;
    return false;
}

bool deleteExpenseCategory(int id) {
    if (!Data::expenseCategory.hasEntry(id) || isExpenseCategoryInUse(id)) return false;
    Data::expenseCategory.eraseEntry(id);
    return true;
}

std::string walletName(int id) {
    return Data::wallet.hasEntry(id) ? Data::wallet.getEntry(id).Name() : "[Deleted wallet]";
}

std::string incomeSourceName(int id) {
    return Data::incomeSource.hasEntry(id) ? Data::incomeSource.getEntry(id).Name() : "[Deleted source]";
}

std::string expenseCategoryName(int id) {
    return Data::expenseCategory.hasEntry(id) ? Data::expenseCategory.getEntry(id).Name() : "[Deleted category]";
}

bool addIncome(const Income& tx) {
    if (Data::income.size() + Data::expense.size() >= TRANSACTION_LIMIT) return false;
    if (!validIncome(tx)) return false;
    Data::income.addEntry(tx);
    return true;
}

bool addExpense(const Expense& tx) {
    if (Data::income.size() + Data::expense.size() >= TRANSACTION_LIMIT) return false;
    if (!validExpense(tx)) return false;
    Data::expense.addEntry(tx);
    return true;
}

bool updateIncome(int id, const Income& tx) {
    if (!Data::income.hasEntry(id) || !validIncome(tx)) return false;
    Data::income.writeEntry(id, tx);
    return true;
}

bool updateExpense(int id, const Expense& tx) {
    if (!Data::expense.hasEntry(id) || !validExpense(tx)) return false;
    Data::expense.writeEntry(id, tx);
    return true;
}

bool deleteIncome(int id) {
    if (!Data::income.hasEntry(id)) return false;
    Data::income.eraseEntry(id);
    return true;
}

bool deleteExpense(int id) {
    if (!Data::expense.hasEntry(id)) return false;
    Data::expense.eraseEntry(id);
    return true;
}

DynamicArray<TransactionItem> getAllTransactionsSorted() {
    DynamicArray<TransactionItem> items;
    items.reserve(Data::income.size() + Data::expense.size());

    for (const auto& e : Data::income)  items.push_back(toViewItem(e.Id(), e.Data));
    for (const auto& e : Data::expense) items.push_back(toViewItem(e.Id(), e.Data));

    std::sort(items.begin(), items.end(), [](const TransactionItem& a, const TransactionItem& b) {
        if (a.date != b.date) return a.date > b.date;
        if (a.id != b.id) return a.id > b.id;
        return a.isIncome && !b.isIncome;
    });
    return items;
}

DashboardSummary getDashboardSummary(const Date& today) {
    DashboardSummary sum;
    sum.walletBalances.resize(Data::wallet.size(), Currency(0));

    for (const auto& e : Data::income) {
        const Income& tx = e.Data;
        sum.totalBalance += tx.Amount();
        if (tx.TransDate().year() == today.year() && tx.TransDate().month() == today.month())
            sum.monthIncome += tx.Amount();

        int idx = findIndexById(Data::wallet, tx.WalletId());
        if (idx >= 0) sum.walletBalances[static_cast<size_t>(idx)] += tx.Amount();
    }

    for (const auto& e : Data::expense) {
        const Expense& tx = e.Data;
        sum.totalBalance -= tx.Amount();
        if (tx.TransDate().year() == today.year() && tx.TransDate().month() == today.month())
            sum.monthExpense += tx.Amount();

        int idx = findIndexById(Data::wallet, tx.WalletId());
        if (idx >= 0) sum.walletBalances[static_cast<size_t>(idx)] -= tx.Amount();
    }

    DynamicArray<TransactionItem> sorted = getAllTransactionsSorted();
    size_t take = std::min<size_t>(2, sorted.size());
    for (size_t i = 0; i < take; ++i) sum.recent.push_back(sorted[i]);

    return sum;
}

} // namespace FinanceService