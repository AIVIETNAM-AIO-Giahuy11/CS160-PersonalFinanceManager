// include/service/ReportService.h
#pragma once
#include "core/Currency.h"
#include "core/Date.h"
#include "core/DynamicArray.h"
#include "data/data.h"
#include <string>

namespace ReportService {

struct Totals {
    Currency income;
    Currency expense;
    Currency net() const { return income - expense; }
};

struct BreakdownItem {
    int id = NULL_ID;
    std::string name;
    Currency amount;
};

struct DateRangeReport {
    Date from;
    Date to;
    int walletFilterId = NULL_ID;       // NULL_ID = tất cả các ví
    int matchCount = 0;
    Totals total;
    DynamicArray<Totals> byWallet;      // khớp chỉ số với Data::wallet
};

struct YearSummary {
    int year = 0;
    Totals totals;
};

struct AnnualReport {
    DynamicArray<YearSummary> byYear;
    Totals total;
    DynamicArray<BreakdownItem> bySource;
    DynamicArray<BreakdownItem> byCategory;
};

DateRangeReport buildDateRangeReport(const Date& from, const Date& to, int walletFilterId = NULL_ID);
AnnualReport buildAnnualReport(const DynamicArray<int>& rawYears);

} // namespace ReportService