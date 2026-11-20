#pragma once

#include <string>
#include "core/Currency.h"
#include "core/DataContainer.h"
#include "domain/Wallet.h"

// All operations use the shared Data::wallet container.
class WalletManager {
public:
    // Returns false if the name is empty or contains only whitespace.
    static bool addWallet(const std::string &name);

    // Returns a copy of the name; throws std::invalid_argument for an unknown ID.
    static std::string getWalletNameFromId(int id);
    static DynamicArray<IdData<Wallet>> getWallets();

    // Returns false for an unknown ID or invalid name.
    static bool renameWallet(int id, const std::string &name);
    // Returns false if the wallet does not exist or is referenced by a transaction/rule.
    static bool deleteWallet(int id);

    // Income minus expense; throws std::invalid_argument for an unknown ID.
    static Currency getWalletBalance(int id);

private:
    static bool isValidName(const std::string &name);
    static bool isWalletInUse(int id);
};
