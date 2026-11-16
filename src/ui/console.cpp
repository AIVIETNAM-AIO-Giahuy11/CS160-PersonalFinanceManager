#include "ui.h"
#include <iostream>
#include <string>

// Terminal setup lives here so ui.cpp can focus on printing and input.
// Set USE_COLOR to false to practice with plain text.
const bool USE_COLOR = true;
bool colorEnabled = false;

void startConsole() {
    colorEnabled = true;
}

// ink is a function to write text depent on role
std :: string ink(const std::string& text, const std::string& role) {
    if (!colorEnabled) return text;
    std::string color = "\x1b[38;2;227;235;244m";
    if (role == "accent") color = "\x1b[38;2;81;223;199m";
    if (role == "income") color = "\x1b[38;2;131;233;165m";
    if (role == "expense") color = "\x1b[38;2;255;149;161m";
    if (role == "muted") color = "\x1b[38;2;155;175;193m";
    return "\x1b[48;2;16;26;39m" + color + text + "\x1b[0m";
}

void finishConsole() {
    if (colorEnabled) {
        std::cout << "\x1b[0m";
    }
}
