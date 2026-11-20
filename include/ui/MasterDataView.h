// include/ui/MasterDataView.h
#pragma once
#include <string>

namespace MasterDataView {
    enum class Kind {
        Wallet,
        IncomeSource,
        ExpenseCategory
    };

    bool show(Kind kind, std::string& message);
}