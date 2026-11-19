// include/ui/Display.h
#pragma once
#ifndef Display_h
#define Display_h
#include "core/DataContainer.h"
#include "ui/Format.h"
#include <iostream>
#include <string>

namespace Display {
    void title(const std::string& name);
    void section(const std::string& name);
    void notice(const std::string& msg);
    void problem(const std::string& msg);

    template <typename T>
    void itemList(const DataContainer<T>& list) {
        if (list.empty()) {
            notice("No items yet.");
            return;
        }
        for (size_t i = 0; i < list.size(); ++i) {
            std::cout << "  " << Format::ink(std::to_string(i + 1), "accent")
                      << "  " << list.atIndex(i).Data.Name() << '\n';
        }
    }
}
#endif