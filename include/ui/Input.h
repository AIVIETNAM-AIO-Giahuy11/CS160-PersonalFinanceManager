// include/ui/Input.h
#pragma once
#ifndef Input_h
#define Input_h
#include "core/Currency.h"
#include "core/DataContainer.h"
#include "core/Date.h"
#include "ui/Display.h"
#include <string>

struct CancelInput {};
struct EndInput {};

namespace Input {
    constexpr size_t NAME_LIMIT = 80;
    constexpr size_t DESCRIPTION_LIMIT = 240;

    std::string line(const std::string& label);
    int integer(const std::string& label, int low, int high);
    Date date(const std::string& label, bool optional, const Date& fallback, const Date& earliest = Date());
    Currency amount(const std::string& label = "Amount in VND (e.g. 50000, 50k, 1m)");
    std::string description(const std::string& label = "Description");
    void pause(const std::string& label = "Press Enter to return to the dashboard");

    template <typename T>
    int chooseItem(const DataContainer<T>& list, const std::string& label) {
        Display::section(label);
        Display::itemList(list);
        int choice = integer("Item number", 1, static_cast<int>(list.size()));
        return list.atIndex(static_cast<size_t>(choice - 1)).Id();
    }
}
#endif 