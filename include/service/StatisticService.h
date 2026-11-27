#pragma once

#include "service/ReportService.h"
#include <optional>

namespace StatisticService {

struct AmountComparison {
    Currency current;
    Currency previous;
    Currency difference;

    // Không có giá trị nếu tháng trước bằng 0.
    std::optional<double> percentChange;
};

struct MonthlyComparison {
    int month = 0;
    int year = 0;

    int previousMonth = 0;
    int previousYear = 0;

    int walletFilterId = NULL_ID;

    AmountComparison income;
    AmountComparison expense;

    Currency currentNet;
    Currency previousNet;
    Currency netDifference;
};

MonthlyComparison compareWithPreviousMonth(
    int month,
    int year,
    int walletFilterId = NULL_ID
);

}