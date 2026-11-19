// src/main.cpp
#include "core/Date.h"
#include "data/data.h"
#include "service/FinanceService.h"
#include "service/RecurringService.h"
#include "service/StorageService.h"
#include "ui/Console.h"
#include "ui/DashboardView.h"
#include "ui/MasterDataView.h"
#include "ui/RecurringView.h"
#include "ui/ReportView.h"
#include "ui/TransactionView.h"
#include <iostream>
#include <new>
#include <string>

using std::string;

namespace {
const char* DATA_PATH = "windowns-version.dat";

bool saveChanges() {
    string error;
    while (!StorageService::serializeData(DATA_PATH, error)) {
        Display::problem(error);
        Display::notice("Your latest changes are still in memory. Check the folder and try again.");
        try {
            if (Input::integer("[1] Retry save    [0] Exit and discard unsaved changes", 0, 1) == 0)
                return false;
        } catch (const CancelInput&) {
            // Giữ nguyên vòng lặp cho tới khi lưu được hoặc xác nhận thoát
        } catch (const EndInput&) {
            return false;
        }
    }
    return true;
}

int runApp() {
    string message, error;
    if (!StorageService::lockData(DATA_PATH, error)) {
        Display::problem(error);
        return 1;
    }

    auto status = StorageService::deserializeData(DATA_PATH, error);
    if (status == StorageService::LoadStatus::LoadError) {
        Display::problem(error);
        return 1;
    }

    if (status == StorageService::LoadStatus::Missing) {
        FinanceService::seedDefaultMasterData();
        message = "Press 1 to record your first transaction.";
    }

    int posted = RecurringService::syncForCurrentMonth(Date::today());
    if (posted < 0) {
        Display::problem("Could not process this month's recurring transactions.");
        return 1;
    }
    if (posted > 0) {
        message = "Recurring transactions posted this month: " + std::to_string(posted) + ".";
    }
    if ((status == StorageService::LoadStatus::Missing || posted > 0) && !saveChanges()) {
        return 1;
    }

    while (true) {
        try {
            int choice = DashboardView::show(message);
            message.clear();
            bool changed = false;

            switch (choice) {
            case 0: return 0;
            case 1: changed = TransactionView::recordTransaction(message); break;
            case 2: changed = TransactionView::showHistory(message); break;
            case 3: changed = RecurringView::show(message); break;
            case 4: changed = MasterDataView::show(MasterDataView::Kind::Wallet, message); break;
            case 5: changed = MasterDataView::show(MasterDataView::Kind::IncomeSource, message); break;
            case 6: changed = MasterDataView::show(MasterDataView::Kind::ExpenseCategory, message); break;
            case 7: ReportView::show(); break;
            }

            if (changed && !saveChanges()) return 1;
        } catch (const CancelInput&) {
            message = "Back on the dashboard.";
        }
    }
}
} // namespace

int main() {
    Terminal::start();
    int exitCode = 0;
    try {
        exitCode = runApp();
    } catch (const EndInput&) {
        // Dữ liệu sau mỗi thao tác thành công đã được lưu xuống đĩa
    } catch (const std::bad_alloc&) {
        Display::problem("There is not enough memory to continue.");
        exitCode = 1;
    }
    Data::clearAll();
    StorageService::unlockData();
    std::cout << '\n' << std::flush;
    Terminal::finish();
    return exitCode;
}