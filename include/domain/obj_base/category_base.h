/*
Base object for (id-name) data, such as wallet, expenseCategory, incomeSource

# Supported functions:
## Getter:
    Id()
    Name()
## Setter:
    setName()

- Note: Id is strictly set by DataContainer<> object's addEntry()
*/

#pragma once
#include <string>
#include "core/DataContainer.h"

class _category_base{
private:
    int id = NULL_ID;
    std::string name;

    void setId(int _id) { id = _id; }
public:
    _category_base(const std::string &_name = "Empty"): name(_name) {}
    int Id() const { return id; }
    const std::string &Name() const { return name; }

    void setName(const std::string &s) { name = s; }

    template <typename T> friend class DataContainer;
};