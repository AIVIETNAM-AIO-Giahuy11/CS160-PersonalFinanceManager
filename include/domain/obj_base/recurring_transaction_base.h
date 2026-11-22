/*
Base object for Recurring Transaction data (Income/Expense)
Supported frequencies are provided by enum class Frequency
Constructor order: frequency - dateStart - dateEnd - amount - sourceid - walletid - desc

# Supported functions:
## Getter:
    Freq()
    SouceId()
    WalletId()
    DateStr()
    DateEnd()
    Amount()
    Desc()
## Setter:
    setFreq()
    setSourceId()
    setWalletId()
    setDateStr()
    setDateEnd()
    setAmount()
    setDesc()
*/

#pragma once
#include <string>
#include "core/DataContainer.h"
#include "core/Currency.h"
#include "core/Date.h"

enum class Frequency{
    DAILY,
    WEEKLY,
    MONTHLY,
    YEARLY
};

class _recurring_transaction_base{
private:
    Frequency freq = Frequency::DAILY;
    Date dateStr = Date();
    Date dateEnd = Date();
    Currency amount = Currency();
    int sourceId = NULL_ID;
    int walletId = NULL_ID;
    std::string description = "";
public:
    _recurring_transaction_base() {}
    _recurring_transaction_base(Frequency _freq, Date &_date, const Date &_date1, Currency _amount, int _sourceId, int _walletId, const std::string &_desc)
    {
        freq = _freq;
        dateStr = _date;
        dateEnd = _date1;
        amount = _amount;
        sourceId = _sourceId;
        walletId = _walletId;
        description = _desc;
    }

    Frequency Freq() const { return freq; }
    int SourceId() const { return sourceId; }
    int WalletId() const { return walletId; }
    const Date &DateStr() const { return dateStr; }
    const Date &DateEnd() const { return dateEnd; }
    Currency Amount() const { return amount; }
    const std::string &Desc() const { return description; }

    void setFreq(Frequency _freq) { freq = _freq; }
    void setSourceId(int _sourceId) { sourceId = _sourceId; }
    void setWalletId(int _walletId) { walletId = _walletId; }
    void setAmount(Currency _amount) { amount = _amount; }
    void setDesc(const std::string &_desc) { description = _desc; }
    void setDateStr(const Date &_date) { dateStr = _date; }
    void setDateEnd(const Date &_date) { dateEnd = _date; }
};