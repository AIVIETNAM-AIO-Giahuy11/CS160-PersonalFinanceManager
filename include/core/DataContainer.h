/*
Project's data container using an internal monotonic ID system and binary search for ID lookup
<Data> must be wrapped with IdData<> for Id managing 

# Supported functions:
    hasEntry(int id) : check if ID is in container
        ex: checking if a WalletId still exist
    
## Modify container: 
    addEntry(<Data>) : append new <Data> to container
    eraseEntry(int id) : erase entry by ID, must ensure valid ID
    writeEntry(int id, <Data>) : look up entry by ID and overwrite with new <Data>, must ensure valid ID

## Read container:
    getEntry(int id) : (READ ONLY) retrieve a const <Data> instance, must ensure valid ID
    begin(), end() : (READ ONLY) support range based for loop and iterator loop
        ex: 
            for (auto &i : container) <== looping as reference (&i) is recommended
                cout << i.Id() << ' ' << i.Data.[Data getter function] << '\n'; 
        ex2:
            for (auto i = begin(); i!=end(); ++i) 
                cout << (*i).Id() << ' ' << (*i).Data.[Data getter function] << '\n'; 

    operator<< : export container data to a DynamicArray of IdData<Data>
        ex: DynamicArray<IdData<Wallet>> _DynamicArray << _WalletContainer;

IdData Wrapper:
    .Id() : get internal Id     <== This prevent user from accidentally modifying id
        ex: Income x; IdData<Wallet> y; 
            x.setWalletId(y.Id());
    .Data : access Data field
        ex: IdData<Wallet> x; x.Data.setName("new name");
*/

#pragma once
#include "core/DynamicArray.h"
#include "core/BinaryIO.h"
#include <stdexcept>

static constexpr int NULL_ID = -1;

template<typename T> class DataContainer; // Forward declaration for IdData

template<typename _D>
class IdData{
private:
    int id; // Private id
public:
    _D Data; // Public data

    IdData(int _id = -1, const _D &_d = _D()): id(_id), Data(_d) {}
    int Id() const { return id; }

    friend class DataContainer<_D>; // Expose id access to DataContainer
};

template<typename Data>
class DataContainer{
private:
    int idHead = 0;
    DynamicArray<IdData<Data>> container{};

    int findIndex(int target_id) const
    {
        if (container.empty()) return -1;
        int l = 0, r = (int)container.size();
        while (l<r)
        {
            int mid = (l+r)>>1;
            if (container[mid].id < target_id)
                l = mid+1;
            else if (container[mid].id > target_id)
                r = mid;
            else
                return mid;
        }
        return -1;
    }

    bool loadEntry(int id, const Data &d) 
    {
        if (id < idHead) return false;
        container.push_back(IdData<Data>(id, d));
        idHead = id + 1;
        return true;
    }
    // const IdData<Data> &operator[](size_t pos) const
    // {
    //     return container[pos];
    // }
public:
    bool hasEntry(int id) const { return findIndex(id)!=-1; }

    /* Modify data */
    void addEntry(const Data &A)
    {
        container.push_back(IdData<Data>(idHead++, A));
    }

    void eraseEntry(int id) // ensure id is valid first
    {
        int t = findIndex(id);
        if (t==-1)
            throw std::invalid_argument("Id not found, ensure Object is in DataContainer before eraseEntry()");
        container.erase(t);
    }

    void writeEntry(int id, const Data &new_entry)
    {
        int t = findIndex(id);
        if (t==-1)
            throw std::invalid_argument("Id not found, ensure Object is in DataContainer before overwriteEntry()");
        container[t].Data = new_entry;
    }

    /* Access data */
    const Data &getEntry(int id) const // ensure id is valid first
    {
        int t = findIndex(id);
        if (t==-1)
            throw std::invalid_argument("Id not found, ensure Object is in DataContainer before getEntry()");
        return container[t].Data;
    }

    const IdData<Data> *begin() const { return container.begin(); }
    const IdData<Data> *end() const { return container.end(); }


    friend void operator<<(DynamicArray<IdData<Data>> &LHS, const DataContainer<Data> &RHS)
    {
        LHS = RHS.container;
    }

    /*Binary functions*/
    void write(std::ostream &out) const {
        BinaryIO::Write<int32_t>(out, idHead);
        BinaryIO::Write<uint32_t>(out, static_cast<uint32_t>(container.size()));
        for (const auto &e : *this) {
            BinaryIO::Write<int32_t>(out, e.Id());
            e.Data.write(out);
        }
    }

    void read(std::istream &in) {
        int32_t savedHead = BinaryIO::Read<int32_t>(in);
        uint32_t count = BinaryIO::Read<uint32_t>(in);
        DataContainer temp;
        for (uint32_t i = 0; i < count; ++i) {
            int32_t id = BinaryIO::Read<int32_t>(in);
            Data d;
            d.read(in);
            if (!temp.loadEntry(id, d))
                throw std::runtime_error("Invalid or duplicate id in data file");
        }

        if (savedHead < temp.idHead)
            throw std::runtime_error("Invalid next-id in data file");
        temp.idHead = savedHead;

        *this = temp;
    }
};
