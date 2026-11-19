// src/ui/Terminal.cpp
#include "ui/Terminal.h"
#include <algorithm>
#include <iostream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace {
bool gColor = false;
bool gInteractive = false;

#ifdef _WIN32
DWORD gOriginalMode = 0;
UINT gOldInputCP = 0;
UINT gOldOutputCP = 0;

void enableWindowsUtf8(bool consoleOut, bool consoleIn) {
    gOldInputCP = GetConsoleCP();
    gOldOutputCP = GetConsoleOutputCP();
    if (consoleOut) SetConsoleOutputCP(CP_UTF8);
    if (consoleIn)  SetConsoleCP(CP_UTF8);
}

bool enableWindowsAnsi(HANDLE out) {
    return SetConsoleMode(out, gOriginalMode | 0x0004) != 0;
}

std::string wideToUtf8(const std::wstring& wide) {
    if (wide.empty()) return "";
    int bytes = WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), NULL, 0, NULL, NULL);
    std::string out(bytes, '\0');
    if (bytes > 0)
        WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), &out[0], bytes, NULL, NULL);
    return out;
}

bool readWindowsConsoleLine(HANDLE in, std::string& out) {
    std::wstring line;
    wchar_t buf[256];
    DWORD count = 0;
    do {
        if (!ReadConsoleW(in, buf, 256, &count, NULL) || count == 0) return false;
        line.append(buf, count);
    } while (line.find(L'\n') == std::wstring::npos);

    if (!line.empty() && line[0] == 26) return false; // Ctrl+Z
    out = wideToUtf8(line);
    return true;
}
#endif
} // namespace

namespace Terminal {

void start() {
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE in  = GetStdHandle(STD_INPUT_HANDLE);
    DWORD inMode = 0;
    bool consoleOut = GetConsoleMode(out, &gOriginalMode) != 0;
    bool consoleIn  = GetConsoleMode(in, &inMode) != 0;

    gInteractive = consoleOut && consoleIn;
    enableWindowsUtf8(consoleOut, gInteractive);
    gColor = consoleOut && enableWindowsAnsi(out);
#else
    gInteractive = isatty(STDOUT_FILENO) && isatty(STDIN_FILENO);
    gColor = isatty(STDOUT_FILENO);
#endif
}

void finish() {
    if (gColor) std::cout << "\x1b[0m" << std::flush;
#ifdef _WIN32
    if (gColor) SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), gOriginalMode);
    if (gOldInputCP)  SetConsoleCP(gOldInputCP);
    if (gOldOutputCP) SetConsoleOutputCP(gOldOutputCP);
#endif
}

bool isInteractive() { return gInteractive; }
bool hasColor()      { return gColor; }

int width() {
    int cols = 88;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        cols = info.srWindow.Right - info.srWindow.Left + 1;
#else
    winsize sz{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &sz) == 0 && sz.ws_col > 0)
        cols = sz.ws_col;
#endif
    return std::max(20, std::min(96, cols - 1));
}

void clear() {
    if (gInteractive && gColor)
        std::cout << "\x1b[48;2;16;26;39m\x1b[38;2;227;235;244m\x1b[2J\x1b[H";
}

bool readRawLine(std::string& out) {
#ifdef _WIN32
    DWORD mode = 0;
    HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    if (GetConsoleMode(in, &mode))
        return readWindowsConsoleLine(in, out);
#endif
    return static_cast<bool>(std::getline(std::cin, out));
}

} // namespace Terminal