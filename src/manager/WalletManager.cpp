#include "manager/WalletManager.h"

#include "data/data.h"
#include <stdexcept>

//Check the name of the Wallet.""	false
bool WalletManager::isValidName(const std::string &name)
{
    return name.find_first_not_of(" \t\n\r\f\v") != std::string::npos;
}

bool WalletManager::addWallet(const std::string &name)
{
    if (!isValidName(name)) return false;
    Data::wallet.addEntry(Wallet(name));
    return true;
}

std::string WalletManager::getWalletNameFromId(int id)
{
    return Data::wallet.getEntry(id).Name();
}

DynamicArray<IdData<Wallet>> WalletManager::getWallets()
{
    DynamicArray<IdData<Wallet>> wallets;
    wallets << Data::wallet;
    return wallets;
}

bool WalletManager::renameWallet(int id, const std::string &name)
{
    if (!Data::wallet.hasEntry(id) || !isValidName(name)) return false;
    Data::wallet.writeEntry(id, Wallet(name));
    return true;
}

bool WalletManager::isWalletInUse(int id)
{
    for (const auto &entry : Data::income)
        if (entry.Data.WalletId() == id) return true;
    for (const auto &entry : Data::expense)
        if (entry.Data.WalletId() == id) return true;
    for (const auto &entry : Data::recurringIncome)
        if (entry.Data.WalletId() == id) return true;
    for (const auto &entry : Data::recurringExpense)
        if (entry.Data.WalletId() == id) return true;
    return false;
}

bool WalletManager::deleteWallet(int id)
{
    if (!Data::wallet.hasEntry(id) || isWalletInUse(id)) return false;
    Data::wallet.eraseEntry(id);
    return true;
}

Currency WalletManager::getWalletBalance(int id)
{
    if (!Data::wallet.hasEntry(id))
        throw std::invalid_argument("Wallet ID not found");

    Currency balance;
    for (const auto &entry : Data::income)
        if (entry.Data.WalletId() == id) balance += entry.Data.Amount();
    for (const auto &entry : Data::expense)
        if (entry.Data.WalletId() == id) balance -= entry.Data.Amount();
    return balance;
}
