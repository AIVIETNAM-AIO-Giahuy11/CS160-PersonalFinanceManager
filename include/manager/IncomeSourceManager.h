#pragma once

#include <string>
#include "core/DataContainer.h"
#include "domain/IncomeSource.h"

class IncomeSourceManager {
public:
    static bool addIncomeSource(const std::string &name);
    static bool renameIncomeSource(int id, const std::string &name);
    static bool deleteIncomeSource(int id);

    // Throws std::invalid_argument if the ID does not exist.
    static std::string getIncomeSourceNameFromId(int id);
    static DynamicArray<IdData<IncomeSource>> getIncomeSources();
    static bool isIncomeSourceInUse(int id);

private:
    static bool isValidName(const std::string &name);
};
