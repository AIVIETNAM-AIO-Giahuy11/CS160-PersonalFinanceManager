#pragma once
#include "core/DynamicArray.h"
#include <stdexcept>

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