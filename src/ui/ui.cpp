#include "ui.h"
#include <iostream>

using std::cin;
using std::cout;
using std::string;

// Start here: each function prints a small part of the screen.
void notice(const string& message) {
    cout << "  " << message << '\n';
}

void problem(const string& message) {
    cout << ink("  Error: " + message, "expense") << '\n';
}

void section(const string& name) {
    cout << '\n' << ink("  " + name, "accent") << '\n';
}

void title(const string& name) {
    cout << "\n======================================================\n";
    cout << ink("  PERSONAL FINANCE MANAGER", "accent") << '\n';
    cout << "  " << name << '\n';
    cout << "======================================================\n";
    cout << "  Type /cancel in any form to return to the dashboard.\n";
}

// All forms use getline, so names and descriptions can contain spaces.
int input(const string& label, std :: string& text) {
    cout << "  " << label << ": ";
    if (!std::getline(cin, text)) {
        return -1;
        // input failed
    }
    if (text == "/cancel") {
        return 0;
        // user input cancel
    }
    return 1;
    // input completed
}

// Store the valid number in value.
// Return 1 on success, 0 on cancellation, -1 when input ends.
int number(const string& label, int low, int high, int& value) {
    while (true) {
        string text;
        int status = input(label, text);

        // Return immediately if the user cancels or input ends.
        if (status != 1) {
            return status;
        }

        int candidate = 0;
        bool valid = !text.empty();

        for (size_t i = 0; i < text.size(); ++i) {
            if (text[i] < '0' || text[i] > '9') {
                valid = false;
                break;
            }

            int digit = text[i] - '0';

            // Check the limit before multiplying.
            if (candidate > high / 10 ||
                (candidate == high / 10 && digit > high % 10)) {
                valid = false;
                break;
            }

            candidate = candidate * 10 + digit;
        }

        if (valid && candidate >= low) {
            value = candidate;
            return 1;
        }

        problem(
            "Enter digits only, from " +
            std::to_string(low) + " to " +
            std::to_string(high) + "."
        );
    }
}
void pauseScreen() {
    string text;
    input("Press Enter to return to the dashboard", text);
}

// A simple format to start with. You can add thousands separators later.
string money(long long amount) {
    return std::to_string(amount) + " VND";
}

// Walk through the transaction linked list and sum one wallet's balance.
long long walletBalance(const Finance& db, int walletId) {
    long long balance = 0;
    for (const Node* p = db.head; p != nullptr; p = p->next) {
        if (p->value.walletId == walletId) {
            if (p->value.income) balance += p->value.amount;
            else balance -= p->value.amount;
        }
    }
    return balance;
}

void showSummary(const Finance& db) {
    long long total = 0;
    long long income = 0;
    long long expense = 0;
    string month = todayISO().substr(0, 7); // Year and Month, put chrono into there

    for (const Node* p = db.head; p != nullptr; p = p->next) {
        const Transaction& t = p->value;
        if (t.income) total += t.amount;
        else total -= t.amount;

        if (t.date.substr(0, 7) == month) {
            if (t.income) income += t.amount;
            else expense += t.amount;
        }
    }

    section("OVERVIEW");
    string role = "accent";
    if (total < 0) role = "expense";
    cout << "  Total balance: " << ink(money(total), role) << '\n';
    cout << "  Month:         " << month << '\n';
    cout << "  Income:        " << ink("+" + money(income), "income") << '\n';
    cout << "  Expenses:      " << ink("-" + money(expense), "expense") << '\n';
}

void showWallets(const Finance& db) {
    section("WALLET BALANCES");
    if (db.wallets.count == 0) notice("No wallets yet.");
    for (int i = 0; i < db.wallets.count; ++i) {
        const Item& wallet = db.wallets.data[i];
        long long balance = walletBalance(db, wallet.id);
        string role = "title";
        if (balance < 0) role = "expense";
        cout << "  " << wallet.name << ": " << ink(money(balance), role) << '\n';
    }
}

void showMenu() {
    section("MAIN MENU");
    cout << "  1. Add Transaction\n";
    cout << "  2. Transaction History\n";
    cout << "  3. Recurring Transactions\n";
    cout << "  4. Manage Wallets\n";
    cout << "  5. Manage Income Sources\n";
    cout << "  6. Manage Expense Categories\n";
    cout << "  7. Reports & Statistics\n";
    cout << "  0. Exit\n\n";
}

int dashboard(const Finance& db, const string& message) {
    title("Dashboard");
    showSummary(db);
    showWallets(db);
    if (!message.empty()) {
        cout << '\n' << ink("  " + message, "accent") << '\n';
    }
    showMenu();
    return number("Your choice", 0, 7);
}
