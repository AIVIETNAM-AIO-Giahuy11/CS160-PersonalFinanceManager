#include "data/data.h"
#include "core/BinaryIO.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace {
    constexpr uint32_t FILE_MAGIC = 0x50464D31; // "PFM1"
    constexpr uint32_t FILE_VERSION = 1;
}
