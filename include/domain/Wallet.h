#pragma once
#include "obj_base/category_base.h"

// Wallet names are stored here; IDs are managed by DataContainer<Wallet>.
// Name() and setName() are inherited from _category_base.
class Wallet : public _category_base {
public:
    Wallet();
    Wallet(const std::string &name);
};
