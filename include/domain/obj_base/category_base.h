#pragma once
#include <string>

class _category_base{
private:
    int id;
    std::string name;
public:
    _category_base(int _id, const std::string &_name): id(_id), name(_name) {}
    int Id() const { return id; }
    const std::string &Name() const { return name; }

    void setName(const std::string &s) { name = s; }
};