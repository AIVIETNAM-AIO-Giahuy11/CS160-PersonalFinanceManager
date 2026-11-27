#include "manager/IncomeSourceManager.h"
#include "data/data.h"

bool IncomeSourceManager::isValidName(const std::string &name)
{
    return name.find_first_not_of(" \t\n\r\f\v") != std::string::npos;
}

bool IncomeSourceManager::addIncomeSource(const std::string &name)
{
    if (!isValidName(name)) return false;
    Data::incomeSource.addEntry(IncomeSource(name));
    Data::saveIfAuto();
    return true;
}

bool IncomeSourceManager::renameIncomeSource(int id, const std::string &name)
{
    if (!Data::incomeSource.hasEntry(id) || !isValidName(name)) return false;
    Data::incomeSource.writeEntry(id, IncomeSource(name));
    Data::saveIfAuto();
    return true;
}

bool IncomeSourceManager::isIncomeSourceInUse(int id)
{
    for (const auto &entry : Data::income)
        if (entry.Data.SourceId() == id) return true;
    for (const auto &entry : Data::recurringIncome)
        if (entry.Data.SourceId() == id) return true;
    return false;
}

bool IncomeSourceManager::deleteIncomeSource(int id)
{
    if (!Data::incomeSource.hasEntry(id) || isIncomeSourceInUse(id)) return false;
    Data::incomeSource.eraseEntry(id);
    Data::saveIfAuto();
    return true;
}

std::string IncomeSourceManager::getIncomeSourceNameFromId(int id)
{
    return Data::incomeSource.getEntry(id).Name();
}

DynamicArray<IdData<IncomeSource>> IncomeSourceManager::getIncomeSources()
{
    DynamicArray<IdData<IncomeSource>> sources;
    sources << Data::incomeSource;
    return sources;
}
