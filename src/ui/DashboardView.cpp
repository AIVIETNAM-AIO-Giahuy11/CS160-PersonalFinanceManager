// src/ui/DashboardView.cpp
#include "ui/DashboardView.h"
#include "core/Date.h"
#include "core/DynamicArray.h"
#include "data/data.h"
#include "service/FinanceService.h"
#include "ui/Console.h"
#include <algorithm>
#include <iostream>
#ifdef _WIN32
#include <conio.h>
#endif

using std::cout;
using std::string;

namespace {
constexpr int PAGE_SIZE = 5;
const char* MENU_ITEMS[8] = {
    "Add Transaction", "Transactions", "Recurring", "Wallets",
    "Income Sources", "Expense Categories", "Reports", "Exit"
};

string formatMonthYear(const Date& d) {
    return (d.month() < 10 ? "0" : "") + std::to_string(d.month()) + "-" + std::to_string(d.year());
}

void buildRightColumn(const FinanceService::DashboardSummary& sum,
                      const Date& today, int page, int rightWidth, bool wide,
                      DynamicArray<string>& rows) {
    string hero = Format::ansi("\x1b[48;2;23;53;61m");
    string base = Format::ansi("\x1b[48;2;16;26;39m");

    rows.push_back(hero + Format::ink(Format::padRight("  TOTAL BALANCE", rightWidth), "muted") + base);
    rows.push_back(hero + Format::ink(Format::padRight("  " + Format::money(sum.totalBalance), rightWidth),
                   sum.totalBalance < Currency(0) ? "expense" : "accent") + base);
    rows.push_back("");

    rows.push_back(Format::ink("CASH FLOW  /  " + formatMonthYear(today), "muted"));
    rows.push_back(Format::ink("Income    " + Format::money(sum.monthIncome, sum.monthIncome > Currency(0)), "income"));
    rows.push_back(Format::ink("Expenses  " + string(sum.monthExpense > Currency(0) ? "-" : "")
                   + Format::money(sum.monthExpense), "expense"));
    rows.push_back("");

    int totalWallets = static_cast<int>(Data::wallet.size());
    int pages = std::max(1, (totalWallets + PAGE_SIZE - 1) / PAGE_SIZE);
    int first = page * PAGE_SIZE;
    int shown = std::min(PAGE_SIZE, totalWallets - first);

    string caption = "WALLET BALANCES";
    if (pages > 1) caption += "  " + std::to_string(page + 1) + "/" + std::to_string(pages);
    rows.push_back(Format::ink(caption, "muted"));

    if (shown <= 0) {
        rows.push_back("No wallets yet.");
    } else {
        for (int i = 0; i < shown; ++i) {
            size_t idx = static_cast<size_t>(first + i);
            const Currency& bal = sum.walletBalances[idx];
            rows.push_back(Format::amountRow(
                Data::wallet.atIndex(idx).Data.Name(),
                Format::money(bal),
                rightWidth,
                bal < Currency(0) ? "expense" : "title"
            ));
        }
    }

    if (!sum.recent.empty() && wide) {
        rows.push_back("");
        rows.push_back(Format::ink("RECENT TRANSACTIONS", "muted"));
        for (size_t i = 0; i < sum.recent.size(); ++i) {
            const auto& tx = sum.recent[i];
            string amt = (tx.isIncome ? "+" : "-") + Format::money(tx.amount);
            string cat = tx.isIncome
                ? FinanceService::incomeSourceName(tx.sourceId)
                : FinanceService::expenseCategoryName(tx.sourceId);
            string label = tx.date.toPaddedString() + "  " + cat;
            rows.push_back(Format::amountRow(label, amt, rightWidth, tx.isIncome ? "income" : "expense"));
        }
    }
}

void buildNavColumn(int selected, int menuWidth, DynamicArray<string>& nav) {
    string hero = Format::ansi("\x1b[48;2;23;53;61m");
    string base = Format::ansi("\x1b[48;2;16;26;39m");

    nav.push_back(Format::ink(Format::padRight("MENU", menuWidth), "muted"));
    for (int i = 0; i < 8; ++i) {
        char key = (i == 7) ? '0' : static_cast<char>('1' + i);
        string name = string(i == selected ? "> " : "  ") + key + "  " + MENU_ITEMS[i];
        name = Format::padRight(name, menuWidth);
        nav.push_back(i == selected ? hero + Format::ink(name, "accent") + base
                                    : Format::ink(name, "title"));
    }
}

void render(const FinanceService::DashboardSummary& sum,
            const Date& today, const string& message, int selected, int page) {
    Terminal::clear();
    int w = Terminal::width();
    bool wide = w >= 74;
    int leftWidth = wide ? 28 : 0;
    int menuWidth = wide ? leftWidth - 3 : 0;
    int rightWidth = w - leftWidth - 4;

    DynamicArray<string> rows;
    DynamicArray<string> nav;
    buildRightColumn(sum, today, page, rightWidth, wide, rows);
    buildNavColumn(selected, menuWidth, nav);

    string bg = Format::ansi("\x1b[48;2;16;26;39m");
    string brand = wide ? "POCKET LEDGER  /  PERSONAL FINANCE MANAGER" : "POCKET LEDGER";
    string dateLabel = w >= 29 ? today.toPaddedString() : "";

    string frame = bg + "  "
                 + Format::ink(Format::padRight(brand, w - 4 - static_cast<int>(dateLabel.size())), "accent")
                 + Format::ink(dateLabel, "muted") + "  \n";
    frame += bg + Format::ink("  Dashboard", "title") + "\n\n";

    size_t lineCount = wide ? std::max<size_t>(nav.size(), rows.size()) : rows.size();
    for (size_t i = 0; i < lineCount; ++i) {
        frame += bg + "  ";
        if (wide) {
            frame += (i < nav.size() ? nav[i] : string(static_cast<size_t>(menuWidth), ' '))
                   + Format::ink("|  ", "muted");
        }
        if (i < rows.size()) frame += rows[i];
        frame += '\n';
    }
    if (!wide) {
        frame += '\n';
        for (size_t i = 0; i < nav.size(); ++i) frame += bg + "  " + nav[i] + '\n';
    }

    frame += '\n';
    if (!message.empty()) frame += Format::ink("  " + message, "accent") + '\n';

    int totalWallets = static_cast<int>(Data::wallet.size());
    int pages = std::max(1, (totalWallets + PAGE_SIZE - 1) / PAGE_SIZE);
    if (pages > 1) frame += Format::ink("  [ Previous wallets    ] Next wallets", "muted") + '\n';

    cout << frame << std::flush;
}
} // namespace

namespace DashboardView {

int show(const string& message) {
    Date today = Date::today();
    FinanceService::DashboardSummary summary = FinanceService::getDashboardSummary(today);

    int totalWallets = static_cast<int>(Data::wallet.size());
    int pages = std::max(1, (totalWallets + PAGE_SIZE - 1) / PAGE_SIZE);
    int selected = 0, page = 0;

    while (true) {
        render(summary, today, message, selected, page);
        string choice;
#ifdef _WIN32
        if (Terminal::isInteractive()) {
            cout << Format::ink("  ↑ ↓ Move    Enter Open    0-7 Select", "muted") << std::flush;
            int key = _getch();
            if (key == 0 || key == 224) {
                int arrow = _getch();
                if (arrow == 72) selected = (selected + 7) % 8;
                if (arrow == 80) selected = (selected + 1) % 8;
                continue;
            }
            if (key == 13) return selected == 7 ? 0 : selected + 1;
            if (key == 3 || key == 26) throw EndInput{};
            choice.assign(1, static_cast<char>(key));
        } else
#endif
        choice = Input::line("Select a menu option (0-7)");

        if (choice == "[") page = (page + pages - 1) % pages;
        else if (choice == "]") page = (page + 1) % pages;
        else if (choice.size() == 1 && choice[0] >= '0' && choice[0] <= '7') return choice[0] - '0';
        else Display::problem("Select a menu option from 0 to 7.");
    }
}

} // namespace DashboardView