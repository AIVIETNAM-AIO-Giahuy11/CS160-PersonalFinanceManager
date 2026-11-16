/*
Project's data container using an internal monotonic Id system and binary search for Id lookup
Object condition:
    Private id field, always default constructed with NULL_ID
    Private setId(int id) function
    Include DataContainer as friend class

# Supported functions:
    findIndex(int id) : find index in internal array from Id
    addEntry(Object) : insert Object
    hasEntry(int id) : check if container has Id (useful for potentially deleted objects)
    operator[](int pos) : get object from internal array's index
    getEntry(int id) : get object from Id, throwing on invalid Id
    eraseEntry(int id) : erase object from Id, throwing on invalid Id
    operator<<(DynamicArray LHS, DataContainer RHS) : extract internal container to a DynamicArray (useful for gui)
*/
#pragma once
#include "core/DynamicArray.h"
#include <stdexcept>

static constexpr int NULL_ID = -1;

template<typename Data>
class DataContainer{
private:
    int idHead = 0;
    DynamicArray<Data> container{};

public:
    bool empty() const { return container.empty(); }
    size_t size()  const { return container.size(); }
    Data* begin() const { return container.begin(); }
    Data* end() const { return container.end(); }

    int findIndex(int target_id) const
    {
        if (empty()) return -1;
        int l = 0, r = (int)size();
        while (l<r)
        {
            int mid = (l+r)>>1;
            if (container[mid].Id() < target_id)
                l = mid+1;
            else if (container[mid].Id() > target_id)
                r = mid;
            else
                return mid;
        }
        return -1;
    }
    bool hasEntry(int id) const { return findIndex(id)!=-1; }

    void addEntry(const Data &A)
    {
        container.push_back(A);
        container.back().setId(idHead++);
    }

    Data &operator[](size_t pos)
    {
        return container[pos];
    }

    Data &getEntry(int id) // ensure id is valid first
    {
        int t = findIndex(id);
        if (t==-1)
            throw std::invalid_argument("Id not found, ensure Object is in DataContainer before getEntry()");
        return this->operator[](t);
    }

    void eraseEntry(int id) // ensure id is valid first
    {
        int t = findIndex(id);
        if (t==-1)
            throw std::invalid_argument("Id not found, ensure Object is in DataContainer before eraseEntry()");
        container.erase(t);
    }

    friend void operator<<(DynamicArray<Data> &LHS, const DataContainer<Data> &RHS)
    {
        LHS = RHS.container;
    }
};