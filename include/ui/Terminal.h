#ifndef Terminal_h
#define Terminal_h
#pragma once
#include <string>

namespace Terminal {
    void start();
    void finish();

    bool isInteractive();
    bool hasColor();
    int width();

    void clear();
    bool readRawLine(std::string& out); // trả về false nếu EOF (Ctrl+Z / Ctrl+D)
}
#endif