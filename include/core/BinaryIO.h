/*
Binary read/write helpers for the project's save file.
All values use fixed-width types so the file does not depend on the compiler.

# Supported functions:
    Write<T>(out, value)   : write one plain value (int32_t, int64_t, ...)
    Read<T>(in)            : read one plain value, throws on failure
    WriteString(out, str)  : write a string, throws if longer than MAX_LEN
    ReadString(in)         : read a string written by WriteString, throws on failure

# Byte layout:
    Plain value : sizeof(T) raw bytes
    String      : [length: uint32_t][characters: length bytes], length <= MAX_LEN
*/

#pragma once
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

namespace BinaryIO {
    inline constexpr uint32_t MAX_LEN = 65536;

    template<typename T>
    void Write(std::ostream &out, const T &value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
        if (!out) throw std::runtime_error("Cannot write data");
    }

    inline void WriteString(std::ostream &out, const std::string &value) {
        uint32_t len = static_cast<uint32_t>(value.size());
        if (len > MAX_LEN) throw std::runtime_error("Exceed string length limits");
        Write<uint32_t>(out, len);
        if (len > 0) {
            out.write(&value[0], len);
            if (!out) throw std::runtime_error("Cannot write string data");
        }
    }

    template<typename T>
    T Read(std::istream &in) {
        T value{};
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
        if (!in) throw std::runtime_error("Cannot read data");
        return value;
    }

    inline std::string ReadString(std::istream &in) {
        uint32_t len = Read<uint32_t>(in);
        if (len > MAX_LEN) throw std::runtime_error("Exceed string length limits");
        std::string s(len, '\0');
        in.read(&s[0], len);
        if (!in) throw std::runtime_error("Cannot read string data");
        return s;
    }
}