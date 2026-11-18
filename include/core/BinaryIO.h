/*
Binary read/write helpers for the project's save file.
All values use fixed-width types so the file does not depend on the compiler.

# Supported functions:
    Write<T>(out, value)   : write one plain value (int32_t, int64_t, ...)
    Read<T>(in)            : read one plain value, throws on failure
    WriteString(out, str)  : write length (uint32_t) then characters
    ReadString(in)         : read a string written by WriteString, throws on failure

Field order in every write() must match the order in read().
*/

#pragma once
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

namespace BinaryIO
{
    inline constexpr uint32_t MAX_LEN = 65536;

    template<typename T>
    void Write(std::ostream &out, const T &value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
        if (!out) throw std::runtime_error("Cannot write data");
    }

    template<typename T>
    T Read(std::istream &in) {
        T value{};
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
        if (!in) throw std::runtime_error("Cannot read data");
        return value;
    }

    inline void WriteString(std::ostream &out, const std::string &value) {
        uint32_t len = static_cast<uint32_t>(value.size());
        if (len > MAX_LEN) throw std::runtime_error("Exceed string length limits");
        Write<uint32_t>(out, len);
        if (len > 0) {
            out.write(value.data(), len);
            if (!out) throw std::runtime_error("Cannot write string data");
        }
    }

    inline std::string ReadString(std::istream &in) {
        // TODO
    }
}