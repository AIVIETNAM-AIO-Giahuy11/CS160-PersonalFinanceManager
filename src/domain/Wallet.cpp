#include "domain/Wallet.h"

Wallet::Wallet() = default;

Wallet::Wallet(const std::string &name)
    : _category_base(name)
{
}
