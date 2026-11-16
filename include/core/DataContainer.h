#pragma once
#include "core/DynamicArray.h"

template<typename Data>
class DataContainer{
private:
    int idHead = 0;
    DynamicArray<Data> container;
public:
    bool empty() const { return container.empty(); }
    size_t size()  const { return container.size(); }
    Data* begin() const { return container.begin(); }
    Data* end() const { return container.end(); }
    

    void addEntry(const Data &A)
    {
        container.push_back(A);
        container.back().setId(idHead++);
    }
};