// src/ui/Input.cpp
#include "ui/Input.h"
#include "ui/Format.h"
#include "ui/Terminal.h"
#include <climits>
#include <exception>
#include <iostream>

namespace {
bool isCancelCommand(const std::string& text) {
    return text == "/cancel" || text == "/huy";
}

bool tryParseInt(const std::string& raw, int& result) {
    std::string text = Format::trim(raw);
    if (text.empty()) return false;

    size_t pos = 0;
    bool negative = (text[0] == '-');
    if (negative || text[0] == '+') ++pos;
    if (pos >= text.size()) return false;

    long long value = 0;
    long long limit = negative ? -static_cast<long long>(INT_MIN) : INT_MAX;
    for (; pos < text.size(); ++pos) {
        if (text[pos] < '0' || text[pos] > '9') return false;
        value = value * 10 + (text[pos] - '0');
        if (value > limit) return false;
    }
    result = static_cast<int>(negative ? -value : value);
    return true;
}

std::string buildDatePrompt(const std::string& label, bool optional, const Date& fallback) {
    std::string hint = optional
        ? " [leave blank for no end date]"
        : " [" + fallback.toPaddedString() + "]";
    return label + " (dd-mm-yyyy)" + hint;
}

bool tryParseDate(const std::string& text, Date& out) {
    try {
        out = Date(text);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool tryParseAmount(const std::string& text, Currency& out) {
    try {
        out = Currency(text);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
} // namespace

namespace Input {

std::string line(const std::string& label) {
    std::cout << "  " << label << Format::ink(" › ", "accent") << std::flush;
    std::string raw;
    if (!Terminal::readRawLine(raw)) throw EndInput{};
    std::string cleaned = Format::trim(raw);
    if (isCancelCommand(cleaned)) throw CancelInput{};
    return cleaned;
}

int integer(const std::string& label, int low, int high) {
    while (true) {
        int value = 0;
        if (tryParseInt(line(label), value) && value >= low && value <= high)
            return value;
        Display::problem("Enter a whole number from " + std::to_string(low) + " to " + std::to_string(high) + ".");
    }
}

Date date(const std::string& label, bool optional, const Date& fallback, const Date& earliest) {
    std::string prompt = buildDatePrompt(label, optional, fallback);
    while (true) {
        std::string text = line(prompt);
        Date parsed;

        if (text.empty()) {
            if (optional) return Date(31, 12, Date::MAX_YEAR);
            parsed = fallback;
        } else if (!tryParseDate(text, parsed)) {
            Display::problem("Enter a valid date in dd-mm-yyyy format (e.g. 19-11-2026).");
            continue;
        }

        if (parsed < earliest) {
            Display::problem("Date must be on or after " + earliest.toPaddedString() + ".");
            continue;
        }
        return parsed;
    }
}

Currency amount(const std::string& label) {
    while (true) {
        Currency value;
        if (!tryParseAmount(line(label), value)) {
            Display::problem("Invalid amount. Examples: 50000, 500.000, 50k, 2m.");
            continue;
        }
        if (value <= Currency(0)) {
            Display::problem("Amount must be greater than 0 VND.");
            continue;
        }
        return value;
    }
}

std::string description(const std::string& label) {
    while (true) {
        std::string text = line(label);
        if (text.size() <= DESCRIPTION_LIMIT && Format::isPrintable(text))
            return text;
        Display::problem("Use one printable line up to " + std::to_string(DESCRIPTION_LIMIT) + " bytes.");
    }
}

void pause(const std::string& label) {
    line(label);
}

} // namespace Input