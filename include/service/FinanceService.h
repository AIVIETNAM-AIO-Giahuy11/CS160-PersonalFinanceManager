#pragma once
#include "core/Currency.h"
#include "core/Date.h"
#include "core/DynamicArray.h"
#include "data/data.h"
#include <string>

namespace FinanceService {

constexpr size_t NAME_LIMIT = 80;
constexpr size_t DESCRIPTION_LIMIT = 240;
constexpr size_t ITEM_LIMIT = 10000;
constexpr size_t TRANSACTION_LIMIT = 1000000;
constexpr size_t RULE_LIMIT = 10000;

struct TransactionItem {
    int id = NULL_ID;
    bool isIncome = true;
    Date date;
    Currency amount;
    int sourceId = NULL_ID;
    int walletId = NULL_ID;
    std::string description;
    int ruleId = NULL_ID;
};

struct DashboardSummary {
    Currency totalBalance;
    Currency monthIncome;
    Currency monthExpense;
    DynamicArray<Currency> walletBalances;      // khớp chỉ số với Data::wallet
    DynamicArray<TransactionItem> recent;       // tối đa 2 giao dịch gần nhất
};

// Kiểm tra chuỗi & danh mục
std::string trimText(const std::string& text);
bool isPrintable(const std::string& text);
bool isValidDescription(const std::string& desc);

// Khởi tạo dữ liệu mẫu cho lần chạy đầu tiên
void seedDefaultMasterData();

// Quản lý Master Data (Ví / Nguồn thu / Danh mục chi)
bool addWallet(const std::string& name);
bool renameWallet(int id, const std::string& name);
bool isWalletInUse(int id);
bool deleteWallet(int id);

bool addIncomeSource(const std::string& name);
bool renameIncomeSource(int id, const std::string& name);
bool isIncomeSourceInUse(int id);
bool deleteIncomeSource(int id);

bool addExpenseCategory(const std::string& name);
bool renameExpenseCategory(int id, const std::string& name);
bool isExpenseCategoryInUse(int id);
bool deleteExpenseCategory(int id);

// Tra cứu tên hiển thị an toàn từ ID
std::string walletName(int id);
std::string incomeSourceName(int id);
std::string expenseCategoryName(int id);

// Quản lý giao dịch Thu / Chi
bool addIncome(const Income& tx);
bool addExpense(const Expense& tx);
bool updateIncome(int id, const Income& tx);
bool updateExpense(int id, const Expense& tx);
bool deleteIncome(int id);
bool deleteExpense(int id);

// Truy vấn cho Dashboard & Lịch sử
DashboardSummary getDashboardSummary(const Date& today);
DynamicArray<TransactionItem> getAllTransactionsSorted();

} // namespace FinanceService