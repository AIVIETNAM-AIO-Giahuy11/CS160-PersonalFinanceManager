// include/ui/Format.h

#pragma once
#ifndef Format_h
#define Format_h
#include "core/Currency.h"
#include <string>

namespace Format {
    std::string trim(const std::string& text);
    bool isPrintable(const std::string& text);

    std::string ansi(const char* sequence);
    std::string ink(const std::string& text, const char* role);

    std::string padRight(const std::string& text, int width);
    std::string truncate(const std::string& text, int maxWidth);
    std::string amountRow(const std::string& label, const std::string& amount, int width, const char* role);
    std::string money(const Currency& amount, bool showPlus = false);
}
#endif