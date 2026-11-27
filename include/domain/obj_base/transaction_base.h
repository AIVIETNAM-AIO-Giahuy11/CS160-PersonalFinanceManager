/*
Base object for Transaction data (Income/Expense)
Constructor order: date - amount - sourceid - walletid - desc

# Supported functions:
## Getter:
    SouceId()
    WalletId()
    TransDate()
    Amount()
    Desc()
## Setter:
    setSourceId()
    setWalletId()
    setDate()
    setAmount()
    setDesc()
*/

#pragma once
#include <string>
#include "core/DataContainer.h"
#include "core/Currency.h"
#include "core/Date.h"
#include "core/BinaryIO.h"

class _transaction_base{
private:
    Date date = Date();
    Currency amount = Currency();
    int sourceId = NULL_ID;
    int walletId = NULL_ID;
    std::string description = "";
public:
    _transaction_base() {}
    _transaction_base(Date _date, Currency _amount, int _sourceId, int _walletId, const std::string &_desc)
    {
        date = _date;
        amount = _amount;
        sourceId = _sourceId;
        walletId = _walletId;
        description = _desc;
    }

    int SourceId() const { return sourceId; }
    int WalletId() const { return walletId; }
    Date TransDate() const { return date; }
    Currency Amount() const { return amount; }
    const std::string &Desc() const { return description; }

    void setSourceId(int _sourceId) { sourceId = _sourceId; }
    void setWalletId(int _walletId) { walletId = _walletId; }
    void setAmount(Currency _amount) { amount = _amount; }
    void setDesc(const std::string &_desc) { description = _desc; }
    void setDate(const Date &_date) { date = _date; }

    void write(std::ostream &out) const {
        date.write(out);
        amount.write(out);
        BinaryIO::Write<int32_t>(out, sourceId);
        BinaryIO::Write<int32_t>(out, walletId);
        BinaryIO::WriteString(out, description);
    }

    void read(std::istream &in) {
        Date d;
        Currency amt;
        d.read(in);
        amt.read(in);
        int32_t srcId = BinaryIO::Read<int32_t>(in);
        int32_t walId = BinaryIO::Read<int32_t>(in);
        std::string desc = BinaryIO::ReadString(in);
        int32_t rule = BinaryIO::Read<int32_t>(in);
        *this = _transaction_base(d, amt, srcId, walId, desc);
    }
};