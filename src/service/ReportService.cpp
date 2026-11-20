// src/service/ReportService.cpp
#include "service/ReportService.h"
#include <algorithm>

namespace {
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

DynamicArray<int> normalizeYears(const DynamicArray<int>& rawYears) {
    DynamicArray<int> sorted = rawYears;
    std::sort(sorted.begin(), sorted.end());

    DynamicArray<int> uniqueYears;
    for (size_t i = 0; i < sorted.size(); ++i) {
        if (uniqueYears.empty() || uniqueYears.back() != sorted[i])
            uniqueYears.push_back(sorted[i]);
    }
    return uniqueYears;
}

int findYearIndex(const DynamicArray<ReportService::YearSummary>& list, int year) {
    int l = 0, r = static_cast<int>(list.size());
    while (l < r) {
        int mid = (l + r) >> 1;
        if (list[static_cast<size_t>(mid)].year < year) l = mid + 1;
        else if (list[static_cast<size_t>(mid)].year > year) r = mid;
        else return mid;
    }
    return -1;
}
} // namespace

namespace ReportService {

DateRangeReport buildDateRangeReport(const Date& from, const Date& to, int walletFilterId) {
    DateRangeReport report;
    report.from = from;
    report.to = to;
    report.walletFilterId = walletFilterId;
    report.byWallet.resize(Data::wallet.size(), Totals{});

    for (const auto& entry : Data::income) {
        const Income& tx = entry.Data;
        if (tx.TransDate() < from || tx.TransDate() > to) continue;
        if (walletFilterId != NULL_ID && tx.WalletId() != walletFilterId) continue;

        ++report.matchCount;
        report.total.income += tx.Amount();
        int idx = findIndexById(Data::wallet, tx.WalletId());
        if (idx >= 0) report.byWallet[static_cast<size_t>(idx)].income += tx.Amount();
    }

    for (const auto& entry : Data::expense) {
        const Expense& tx = entry.Data;
        if (tx.TransDate() < from || tx.TransDate() > to) continue;
        if (walletFilterId != NULL_ID && tx.WalletId() != walletFilterId) continue;

        ++report.matchCount;
        report.total.expense += tx.Amount();
        int idx = findIndexById(Data::wallet, tx.WalletId());
        if (idx >= 0) report.byWallet[static_cast<size_t>(idx)].expense += tx.Amount();
    }

    return report;
}

AnnualReport buildAnnualReport(const DynamicArray<int>& rawYears) {
    AnnualReport report;
    DynamicArray<int> years = normalizeYears(rawYears);

    for (size_t i = 0; i < years.size(); ++i)
        report.byYear.push_back(YearSummary{years[i], Totals{}});

    for (const auto& entry : Data::incomeSource)
        report.bySource.push_back(BreakdownItem{entry.Id(), entry.Data.Name(), Currency(0)});

    for (const auto& entry : Data::expenseCategory)
        report.byCategory.push_back(BreakdownItem{entry.Id(), entry.Data.Name(), Currency(0)});

    for (const auto& entry : Data::income) {
        const Income& tx = entry.Data;
        int yIdx = findYearIndex(report.byYear, tx.TransDate().year());
        if (yIdx < 0) continue;

        report.total.income += tx.Amount();
        report.byYear[static_cast<size_t>(yIdx)].totals.income += tx.Amount();

        int sIdx = findIndexById(Data::incomeSource, tx.SourceId());
        if (sIdx >= 0) report.bySource[static_cast<size_t>(sIdx)].amount += tx.Amount();
    }

    for (const auto& entry : Data::expense) {
        const Expense& tx = entry.Data;
        int yIdx = findYearIndex(report.byYear, tx.TransDate().year());
        if (yIdx < 0) continue;

        report.total.expense += tx.Amount();
        report.byYear[static_cast<size_t>(yIdx)].totals.expense += tx.Amount();

        int cIdx = findIndexById(Data::expenseCategory, tx.SourceId());
        if (cIdx >= 0) report.byCategory[static_cast<size_t>(cIdx)].amount += tx.Amount();
    }

    return report;
}

} // namespace ReportService