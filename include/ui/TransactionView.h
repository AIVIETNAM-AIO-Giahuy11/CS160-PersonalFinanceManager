// include/ui/TransactionView.h
#pragma once
#include <string>

namespace TransactionView {
    bool recordTransaction(std::string& message);
    bool showHistory(std::string& message);
}