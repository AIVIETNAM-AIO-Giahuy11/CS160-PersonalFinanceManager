/*
Base object for (id-name) data, such as wallet, expenseCategory, incomeSource

# Supported functions:
## Getter:
    Name()
## Setter:
    setName()
*/

#pragma once
#include <string>
#include "core/DataContainer.h"

class _category_base{
private:
    std::string name = "";
public:
    _category_base() {}
    _category_base(const std::string &_name): name(_name) {}
    
    const std::string &Name() const { return name; }

    void setName(const std::string &s) { name = s; }
};