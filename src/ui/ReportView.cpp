// src/ui/ReportView.cpp
#include "ui/ReportView.h"
#include "core/Date.h"
#include "core/DynamicArray.h"
#include "data/data.h"
#include "service/FinanceService.h"
#include "service/ReportService.h"
#include "ui/Console.h"
#include <iostream>
#include <string>

using std::cout;
using std::string;

namespace {
void showTotals(const ReportService::Totals& totals) {
    Currency net = totals.net();
    cout << "  Income  " << Format::ink(Format::money(totals.income), "income")
         << "    Expenses  " << Format::ink(Format::money(totals.expense), "expense")
         << "    Net  " << Format::ink(Format::money(net), net < Currency(0) ? "expense" : "accent")
         << '\n';
}

void showBreakdown(const DynamicArray<ReportService::BreakdownItem>& items,
                   const Currency& total, const char* role) {
    if (items.empty()) {
        cout << "  No entries yet.\n";
        return;
    }
    for (size_t i = 0; i < items.size(); ++i) {
        int bars = 0;
        if (total.raw() > 0)
            bars = static_cast<int>(10.0 * items[i].amount.raw() / total.raw() + 0.5);

        string filled, empty;
        for (int n = 0; n < bars; ++n) filled += "■";
        for (int n = bars; n < 10; ++n) empty += "·";

        cout << "  " << Format::ink(filled, role) << Format::ink(empty, "muted") << "  "
             << items[i].name << "   " << Format::ink(Format::money(items[i].amount), role) << '\n';
    }
}

void runDateRangeReport() {
    Display::section("Date range");
    Date today = Date::today();
    Date firstOfMonth(1, today.month(), today.year());

    Date from = Input::date("Start date", false, firstOfMonth);
    Date defaultTo = (from > today) ? from : today;
    Date to = Input::date("End date (inclusive)", false, defaultTo, from);

    int walletFilterId = NULL_ID;
    if (!Data::wallet.empty()) {
        Display::section("Wallet filter");
        cout << "  0  All wallets\n";
        Display::itemList(Data::wallet);
        int choice = Input::integer("Wallet number", 0, static_cast<int>(Data::wallet.size()));
        if (choice > 0)
            walletFilterId = Data::wallet.atIndex(static_cast<size_t>(choice - 1)).Id();
    }

    ReportService::DateRangeReport report = ReportService::buildDateRangeReport(from, to, walletFilterId);

    Display::title("Date range report");
    cout << "  " << report.from.toPaddedString() << "  ->  " << report.to.toPaddedString() << '\n';
    Display::section(walletFilterId == NULL_ID ? "All wallets" : FinanceService::walletName(walletFilterId));
    showTotals(report.total);

    if (report.matchCount == 0)
        Display::notice("No transactions in this date range.");

    if (walletFilterId == NULL_ID && !Data::wallet.empty()) {
        Display::section("Breakdown by wallet");
        for (size_t i = 0; i < Data::wallet.size(); ++i) {
            cout << "  " << Format::ink(Data::wallet.atIndex(i).Data.Name(), "title") << '\n';
            showTotals(report.byWallet[i]);
        }
    }
}

bool parseYears(const string& line, DynamicArray<int>& years) {
    years.clear();
    const char* sep = ", \t\r\n\f\v";
    size_t start = line.find_first_not_of(sep);

    while (start != string::npos) {
        size_t end = line.find_first_of(sep, start);
        string token = line.substr(start, end - start);
        if (token.size() != 4) return false;

        int y = 0;
        for (char c : token) {
            if (c < '0' || c > '9') return false;
            y = y * 10 + (c - '0');
        }
        if (y < Date::MIN_YEAR || y > Date::MAX_YEAR) return false;

        years.push_back(y);
        start = line.find_first_not_of(sep, end);
    }
    return !years.empty();
}

void runAnnualReport() {
    string defaultYear = std::to_string(Date::today().year());
    DynamicArray<int> selectedYears;

    while (true) {
        string text = Input::line("Years, separated by commas [" + defaultYear + "]");
        if (text.empty()) text = defaultYear;
        if (parseYears(text, selectedYears)) break;
        Display::problem("Enter valid 4-digit years between "
            + std::to_string(Date::MIN_YEAR) + " and " + std::to_string(Date::MAX_YEAR)
            + " (e.g. 2025, 2026).");
    }

    ReportService::AnnualReport report = ReportService::buildAnnualReport(selectedYears);

    Display::title("Annual report");
    Display::section("Yearly totals");
    for (size_t i = 0; i < report.byYear.size(); ++i) {
        cout << "  " << Format::ink("Year " + std::to_string(report.byYear[i].year), "title") << '\n';
        showTotals(report.byYear[i].totals);
    }

    if (report.byYear.size() > 1) {
        Display::section("Total for selected years");
        showTotals(report.total);
    }
    if (report.total.income == Currency(0) && report.total.expense == Currency(0))
        Display::notice("No transactions in the selected years.");

    Display::section("Income by source  ·  " + Format::money(report.total.income));
    showBreakdown(report.bySource, report.total.income, "income");

    Display::section("Expenses by category  ·  " + Format::money(report.total.expense));
    showBreakdown(report.byCategory, report.total.expense, "expense");
}
} // namespace

namespace ReportView {

void show() {
    Display::title("Reports");
    cout << "  1  Income and expenses by date range and wallet\n"
         << "  2  Annual totals by income source and expense category\n"
         << "  0  Back to dashboard\n";

    int choice = Input::integer("Report number", 0, 2);
    if (choice == 0) return;

    if (choice == 1) runDateRangeReport();
    else runAnnualReport();

    Input::pause();
}

} // namespace ReportView