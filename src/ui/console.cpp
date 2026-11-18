// #include "../../include/ui/ui.h"
// #include <iostream>
// #ifdef _WIN32
// #include <windows.h>
// #else
// #include <unistd.h>
// #endif

// // Terminal setup lives here so ui.cpp can focus on printing and input.
// // Set USE_COLOR to false to practice with plain text.
// const bool USE_COLOR = true;
// bool colorEnabled = false;
// #ifdef _WIN32
// DWORD savedMode = 0;
// #endif

// void startConsole() {
// #ifdef _WIN32
//     HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
//     if (USE_COLOR && GetConsoleMode(output, &savedMode)) {
//         // Enable ANSI color sequences on Windows.
//         colorEnabled = SetConsoleMode(output, savedMode | 0x0004) != 0;
//     }
// #else
//     colorEnabled = USE_COLOR && isatty(STDOUT_FILENO);
// #endif
// }

// std::string ink(const std::string& text, const std::string& role) {
//     if (!colorEnabled) return text;
//     std::string color = "\x1b[38;2;227;235;244m";
//     if (role == "accent") color = "\x1b[38;2;81;223;199m";
//     if (role == "income") color = "\x1b[38;2;131;233;165m";
//     if (role == "expense") color = "\x1b[38;2;255;149;161m";
//     if (role == "muted") color = "\x1b[38;2;155;175;193m";
//     return "\x1b[48;2;16;26;39m" + color + text + "\x1b[0m";
// }

// void finishConsole() {
//     if (colorEnabled) {
//         std::cout << "\x1b[0m";
// #ifdef _WIN32
//         SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), savedMode);
// #endif
//     }
// }
