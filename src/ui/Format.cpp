// src/ui/Format.cpp
#include "ui/Format.h"
#include "ui/Terminal.h"
#include <algorithm>
#include <cstring>

namespace {
const char* roleToAnsi(const char* role) {
    if (std::strcmp(role, "accent") == 0)  return "\x1b[1;38;2;81;223;199m";
    if (std::strcmp(role, "income") == 0)  return "\x1b[38;2;131;233;165m";
    if (std::strcmp(role, "expense") == 0) return "\x1b[38;2;255;149;161m";
    if (std::strcmp(role, "muted") == 0)   return "\x1b[38;2;155;175;193m";
    return "\x1b[38;2;227;235;244m";
}
} // namespace

namespace Format {

std::string trim(const std::string& text) {
    const char* ws = " \t\r\n\f\v";
    size_t start = text.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = text.find_last_not_of(ws);
    return text.substr(start, end - start + 1);
}

bool isPrintable(const std::string& text) {
    for (unsigned char c : text) {
        if (c < 32 || c == 127) return false;
    }
    return true;
}

std::string ansi(const char* sequence) {
    return Terminal::hasColor() ? sequence : "";
}

std::string ink(const std::string& text, const char* role) {
    if (!Terminal::hasColor()) return text;
    return std::string(roleToAnsi(role)) + text + "\x1b[22;38;2;227;235;244m";
}

std::string padRight(const std::string& text, int width) {
    int missing = width - static_cast<int>(text.size());
    return missing <= 0 ? text : text + std::string(missing, ' ');
}

std::string truncate(const std::string& text, int maxWidth) {
    if (static_cast<int>(text.size()) <= maxWidth) return text;
    int cut = std::max(0, maxWidth - 3);
    while (cut > 0 && static_cast<unsigned char>(text[cut]) >= 128) --cut;
    return text.substr(0, cut) + std::string(std::min(3, maxWidth), '.');
}

std::string amountRow(const std::string& label, const std::string& amount, int width, const char* role) {
    int labelWidth = std::max(1, width - static_cast<int>(amount.size()) - 1);
    std::string fitted = truncate(label, labelWidth);
    return padRight(fitted, labelWidth) + " " + ink(amount, role);
}

std::string money(const Currency& amount, bool showPlus) {
    return amount.str(showPlus);
}

} // namespace Format