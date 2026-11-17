/*
Project's data container using an internal monotonic Id system and binary search for Id lookup
Object condition:
    Private id field, always default constructed with NULL_ID
    Private setId(int id) function
    Public Id() function
    Include DataContainer as friend class

# Supported functions:
    
*/

#pragma once
#include "core/DynamicArray.h"
#include <stdexcept>

static constexpr int NULL_ID = -1;

template<typename _D>
struct IdData{
    int Id;
    _D Data;
    IdData(int _id = -1, const _D &_d = _D()): Id(_id), Data(_d) {}
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
            if (container[mid].Id < target_id)
                l = mid+1;
            else if (container[mid].Id > target_id)
                r = mid;
            else
                return mid;
        }
        return -1;
    }
    IdData<Data> &operator[](size_t pos)
    {
        return container[pos];
    }
public:
    bool hasEntry(int id) const { return findIndex(id)!=-1; }

    void addEntry(const Data &A)
    {
        container.push_back(IdData<Data>(idHead++, A));
    }

    const IdData<Data> &getEntry(int id) const // ensure id is valid first
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

    friend void operator<<(DynamicArray<IdData<Data>> &LHS, const DataContainer<Data> &RHS)
    {
        LHS = RHS.container;
    }
};