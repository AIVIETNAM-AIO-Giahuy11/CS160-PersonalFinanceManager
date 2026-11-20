// include/service/StorageService.h
#pragma once
#include <string>

namespace StorageService {

enum class LoadStatus {
    Loaded,
    Missing,
    LoadError
};

bool lockData(const std::string& path, std::string& error);
void unlockData();

LoadStatus deserializeData(const std::string& path, std::string& error);
bool serializeData(const std::string& path, std::string& error);

} // namespace StorageService