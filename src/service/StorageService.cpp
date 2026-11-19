// src/service/StorageService.cpp
#include "service/StorageService.h"
#include "data/data.h"
#include "service/FinanceService.h"
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fstream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace {
const char SIGNATURE[4] = {'P', 'F', 'M', '2'};

#ifdef _WIN32
HANDLE gSessionLock = INVALID_HANDLE_VALUE;
#else
int gSessionLock = -1;
#endif

struct Snapshot {
    DataContainer<Wallet> wallet;
    DataContainer<IncomeSource> incomeSource;
    DataContainer<ExpenseCategory> expenseCategory;
    DataContainer<Income> income;
    DataContainer<Expense> expense;
    DataContainer<RecurringIncome> recurringIncome;
    DataContainer<RecurringExpense> recurringExpense;
};

/* --- Đọc / ghi kiểu nguyên thủy (little-endian) --- */

void writeUInt(std::ofstream& file, uint64_t value, int width) {
    unsigned char bytes[8] = {};
    for (int i = 0; i < width; ++i) {
        bytes[i] = static_cast<unsigned char>(value & 0xFF);
        value >>= 8;
    }
    file.write(reinterpret_cast<const char*>(bytes), width);
}

bool readUInt(std::ifstream& file, int width, uint64_t& value) {
    unsigned char bytes[8] = {};
    if (!file.read(reinterpret_cast<char*>(bytes), width)) return false;
    value = 0;
    for (int i = width - 1; i >= 0; --i) {
        value = (value << 8) | bytes[i];
    }
    return true;
}

void writeInt32(std::ofstream& file, int32_t value) {
    writeUInt(file, static_cast<uint32_t>(value), 4);
}

bool readInt32(std::ifstream& file, int32_t& out) {
    uint64_t raw = 0;
    if (!readUInt(file, 4, raw)) return false;
    out = static_cast<int32_t>(static_cast<uint32_t>(raw));
    return true;
}

bool readBoundedInt(std::ifstream& file, int maxVal, int& out) {
    uint64_t raw = 0;
    if (!readUInt(file, 4, raw) || raw > static_cast<uint64_t>(maxVal)) return false;
    out = static_cast<int>(raw);
    return true;
}

void writeText(std::ofstream& file, const std::string& text) {
    writeUInt(file, text.size(), 4);
    if (!text.empty())
        file.write(text.data(), static_cast<std::streamsize>(text.size()));
}

bool readText(std::ifstream& file, size_t maxLen, std::string& out) {
    int len = 0;
    if (!readBoundedInt(file, static_cast<int>(maxLen), len)) return false;
    out.resize(static_cast<size_t>(len));
    if (len == 0) return true;
    return static_cast<bool>(file.read(&out[0], len));
}

void writeDate(std::ofstream& file, const Date& d) {
    writeUInt(file, static_cast<uint64_t>(d.day()), 1);
    writeUInt(file, static_cast<uint64_t>(d.month()), 1);
    writeUInt(file, static_cast<uint64_t>(d.year()), 2);
}

bool readDate(std::ifstream& file, Date& out) {
    uint64_t d = 0, m = 0, y = 0;
    if (!readUInt(file, 1, d) || !readUInt(file, 1, m) || !readUInt(file, 2, y))
        return false;
    try {
        out = Date(static_cast<int>(d), static_cast<int>(m), static_cast<int>(y));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool readPositiveCurrency(std::ifstream& file, Currency& out) {
    uint64_t raw = 0;
    if (!readUInt(file, 8, raw) || raw == 0) return false;
    try {
        out = Currency(static_cast<long long>(raw));
        return out > Currency(0);
    } catch (const std::exception&) {
        return false;
    }
}

/* --- Đọc / ghi từng DataContainer --- */

template <typename T>
void writeCategories(std::ofstream& file, const DataContainer<T>& list) {
    writeUInt(file, list.size(), 4);
    for (const auto& entry : list) {
        writeInt32(file, entry.Id());
        writeText(file, entry.Data.Name());
    }
}

template <typename T>
bool readCategories(std::ifstream& file, DataContainer<T>& list) {
    int count = 0;
    if (!readBoundedInt(file, static_cast<int>(FinanceService::ITEM_LIMIT), count)) return false;
    for (int i = 0; i < count; ++i) {
        int id = 0;
        std::string name;
        if (!readBoundedInt(file, INT_MAX, id) ||
            !readText(file, FinanceService::NAME_LIMIT, name) ||
            name.empty() || !FinanceService::isPrintable(name))
            return false;
        if (!list.loadEntry(id, T(name))) return false;
    }
    return true;
}

template <typename T>
void writeRecurringList(std::ofstream& file, const DataContainer<T>& list) {
    writeUInt(file, list.size(), 4);
    for (const auto& entry : list) {
        const T& r = entry.Data;
        writeInt32(file, entry.Id());
        writeUInt(file, static_cast<uint64_t>(r.Freq()), 1);
        writeDate(file, r.DateStr());
        writeDate(file, r.DateEnd());
        writeUInt(file, static_cast<uint64_t>(r.Amount().raw()), 8);
        writeInt32(file, r.SourceId());
        writeInt32(file, r.WalletId());
        writeText(file, r.Desc());
    }
}

template <typename T, typename CatT>
bool readRecurringList(std::ifstream& file, DataContainer<T>& list,
                       const DataContainer<Wallet>& wallets,
                       const DataContainer<CatT>& cats) {
    int count = 0;
    if (!readBoundedInt(file, static_cast<int>(FinanceService::RULE_LIMIT), count)) return false;
    for (int i = 0; i < count; ++i) {
        int id = 0;
        uint64_t freqRaw = 0;
        Date dStart, dEnd;
        Currency amt;
        int32_t sourceId = NULL_ID, walletId = NULL_ID;
        std::string desc;

        if (!readBoundedInt(file, INT_MAX, id) ||
            !readUInt(file, 1, freqRaw) || freqRaw > static_cast<uint64_t>(Frequency::YEARLY) ||
            !readDate(file, dStart) || !readDate(file, dEnd) || dEnd < dStart ||
            !readPositiveCurrency(file, amt) ||
            !readInt32(file, sourceId) || !readInt32(file, walletId) ||
            !readText(file, FinanceService::DESCRIPTION_LIMIT, desc) ||
            !FinanceService::isValidDescription(desc))
            return false;

        if (!wallets.hasEntry(walletId) || !cats.hasEntry(sourceId)) return false;

        T rule(static_cast<Frequency>(freqRaw), dStart, dEnd, amt, sourceId, walletId, desc);
        if (!list.loadEntry(id, rule)) return false;
    }
    return true;
}

template <typename T>
void writeTransactions(std::ofstream& file, const DataContainer<T>& list) {
    writeUInt(file, list.size(), 4);
    for (const auto& entry : list) {
        const T& tx = entry.Data;
        writeInt32(file, entry.Id());
        writeDate(file, tx.TransDate());
        writeUInt(file, static_cast<uint64_t>(tx.Amount().raw()), 8);
        writeInt32(file, tx.SourceId());
        writeInt32(file, tx.WalletId());
        writeInt32(file, tx.RuleId());
        writeText(file, tx.Desc());
    }
}

template <typename T, typename CatT>
bool readTransactions(std::ifstream& file, DataContainer<T>& list,
                      const DataContainer<Wallet>& wallets,
                      const DataContainer<CatT>& cats) {
    int count = 0;
    if (!readBoundedInt(file, static_cast<int>(FinanceService::TRANSACTION_LIMIT), count)) return false;
    for (int i = 0; i < count; ++i) {
        int id = 0;
        Date date;
        Currency amt;
        int32_t sourceId = NULL_ID, walletId = NULL_ID, ruleId = NULL_ID;
        std::string desc;

        if (!readBoundedInt(file, INT_MAX, id) ||
            !readDate(file, date) ||
            !readPositiveCurrency(file, amt) ||
            !readInt32(file, sourceId) || !readInt32(file, walletId) ||
            !readInt32(file, ruleId) || ruleId < NULL_ID ||
            !readText(file, FinanceService::DESCRIPTION_LIMIT, desc) ||
            !FinanceService::isValidDescription(desc))
            return false;

        if (!wallets.hasEntry(walletId) || !cats.hasEntry(sourceId)) return false;

        T tx(date, amt, sourceId, walletId, desc, ruleId);
        if (!list.loadEntry(id, tx)) return false;
    }
    return true;
}

void writeSnapshot(std::ofstream& file) {
    file.write(SIGNATURE, 4);
    writeCategories(file, Data::wallet);
    writeCategories(file, Data::incomeSource);
    writeCategories(file, Data::expenseCategory);
    writeRecurringList(file, Data::recurringIncome);
    writeRecurringList(file, Data::recurringExpense);
    writeTransactions(file, Data::income);
    writeTransactions(file, Data::expense);
}

bool readSnapshot(std::ifstream& file, Snapshot& snap) {
    char header[4];
    if (!file.read(header, 4) || std::memcmp(header, SIGNATURE, 4) != 0) return false;

    if (!readCategories(file, snap.wallet) ||
        !readCategories(file, snap.incomeSource) ||
        !readCategories(file, snap.expenseCategory))
        return false;

    if (!readRecurringList(file, snap.recurringIncome, snap.wallet, snap.incomeSource) ||
        !readRecurringList(file, snap.recurringExpense, snap.wallet, snap.expenseCategory))
        return false;

    if (!readTransactions(file, snap.income, snap.wallet, snap.incomeSource) ||
        !readTransactions(file, snap.expense, snap.wallet, snap.expenseCategory))
        return false;

    if (snap.income.size() + snap.expense.size() > FinanceService::TRANSACTION_LIMIT)
        return false;

    return file.peek() == std::ifstream::traits_type::eof() && !file.bad();
}

/* --- Tiện ích hệ điều hành (file tạm & kiểm tra tồn tại) --- */

bool fileIsMissing(const std::string& path) {
#ifdef _WIN32
    return GetFileAttributesA(path.c_str()) == INVALID_FILE_ATTRIBUTES &&
           GetLastError() == ERROR_FILE_NOT_FOUND;
#else
    struct stat info;
    return stat(path.c_str(), &info) != 0 && errno == ENOENT;
#endif
}

std::string temporaryPath(const std::string& path) {
#ifdef _WIN32
    return path + ".tmp." + std::to_string(GetCurrentProcessId());
#else
    return path + ".tmp." + std::to_string(getpid());
#endif
}

bool replaceFile(const std::string& from, const std::string& to) {
#ifdef _WIN32
    return MoveFileExA(from.c_str(), to.c_str(),
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return std::rename(from.c_str(), to.c_str()) == 0;
#endif
}
} // namespace

namespace StorageService {

bool lockData(const std::string& path, std::string& error) {
    error.clear();
    const std::string lockPath = path + ".lock";
#ifdef _WIN32
    if (gSessionLock != INVALID_HANDLE_VALUE) {
        error = "This session already has a ledger open.";
        return false;
    }
    gSessionLock = CreateFileA(lockPath.c_str(), GENERIC_READ | GENERIC_WRITE,
                               0, NULL, OPEN_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
    if (gSessionLock != INVALID_HANDLE_VALUE) return true;
#else
    if (gSessionLock != -1) {
        error = "This session already has a ledger open.";
        return false;
    }
    gSessionLock = open(lockPath.c_str(), O_CREAT | O_RDWR, 0600);
    if (gSessionLock != -1 && flock(gSessionLock, LOCK_EX | LOCK_NB) == 0) return true;
    if (gSessionLock != -1) close(gSessionLock);
    gSessionLock = -1;
#endif
    error = "Could not lock " + path + ". Close any other running session and check folder permissions.";
    return false;
}

void unlockData() {
#ifdef _WIN32
    if (gSessionLock != INVALID_HANDLE_VALUE) CloseHandle(gSessionLock);
    gSessionLock = INVALID_HANDLE_VALUE;
#else
    if (gSessionLock != -1) close(gSessionLock);
    gSessionLock = -1;
#endif
}

LoadStatus deserializeData(const std::string& path, std::string& error) {
    error.clear();
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file) {
        if (fileIsMissing(path)) return LoadStatus::Missing;
        error = "Could not open ledger file: " + path;
        return LoadStatus::LoadError;
    }

    Snapshot snap;
    if (!readSnapshot(file, snap)) {
        error = "Ledger file is incomplete, damaged, or unsupported: " + path;
        return LoadStatus::LoadError;
    }

    Data::wallet           = snap.wallet;
    Data::incomeSource     = snap.incomeSource;
    Data::expenseCategory  = snap.expenseCategory;
    Data::recurringIncome  = snap.recurringIncome;
    Data::recurringExpense = snap.recurringExpense;
    Data::income           = snap.income;
    Data::expense          = snap.expense;
    return LoadStatus::Loaded;
}

bool serializeData(const std::string& path, std::string& error) {
    error.clear();
    const std::string temp = temporaryPath(path);
    std::ofstream file(temp.c_str(), std::ios::binary | std::ios::trunc);
    if (!file) {
        error = "Could not create temporary file to save " + path + ".";
        return false;
    }
    writeSnapshot(file);
    file.close();
    if (!file || !replaceFile(temp, path)) {
        std::remove(temp.c_str());
        error = "Could not save " + path + ". Check disk space and folder permissions.";
        return false;
    }
    return true;
}

} // namespace StorageService