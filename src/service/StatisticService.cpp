#include "service/StatisticService.h"
#include <stdexcept>

namespace {

StatisticService::AmountComparison compareAmounts(
    const Currency& current,
    const Currency& previous
) {
    StatisticService::AmountComparison result;

    result.current = current;
    result.previous = previous;
    result.difference = current - previous;

    if (previous.raw() != 0) {
        result.percentChange =
            (static_cast<double>(current.raw())
             - static_cast<double>(previous.raw()))
            / static_cast<double>(previous.raw())
            * 100.0;
    }

    return result;
}

}

namespace StatisticService {

MonthlyComparison compareWithPreviousMonth(
    int month,
    int year,
    int walletFilterId
) {
    // Constructor Date kiểm tra tháng/năm hợp lệ.
    const Date currentFrom(1, month, year);

    // Date hiện chỉ hỗ trợ từ năm 1970.
    if (month == 1 && year == Date::MIN_YEAR) {
        throw std::out_of_range(
            "Previous month is outside supported date range"
        );
    }

    if (walletFilterId != NULL_ID
        && !Data::wallet.hasEntry(walletFilterId)) {
        throw std::invalid_argument("Wallet does not exist");
    }

    int previousMonth = month - 1;
    int previousYear = year;

    if (previousMonth == 0) {
        previousMonth = 12;
        --previousYear;
    }

    const Date currentTo(
        Date::monthLen(month, year),
        month,
        year
    );

    const Date previousFrom(1, previousMonth, previousYear);

    const Date previousTo(
        Date::monthLen(previousMonth, previousYear),
        previousMonth,
        previousYear
    );

    const auto current = ReportService::buildDateRangeReport(
        currentFrom, currentTo, walletFilterId
    );

    const auto previous = ReportService::buildDateRangeReport(
        previousFrom, previousTo, walletFilterId
    );

    MonthlyComparison result;
    result.month = month;
    result.year = year;
    result.previousMonth = previousMonth;
    result.previousYear = previousYear;
    result.walletFilterId = walletFilterId;

    result.income = compareAmounts(
        current.total.income,
        previous.total.income
    );

    result.expense = compareAmounts(
        current.total.expense,
        previous.total.expense
    );

    result.currentNet = current.total.net();
    result.previousNet = previous.total.net();
    result.netDifference = result.currentNet - result.previousNet;

    return result;
}

}