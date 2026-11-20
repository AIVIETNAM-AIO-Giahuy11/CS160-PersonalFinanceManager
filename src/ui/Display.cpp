// src/ui/Display.cpp
#include "ui/Display.h"
#include "ui/Terminal.h"

namespace Display {

void title(const std::string& name) {
    Terminal::clear();
    std::cout << '\n'
              << Format::ink("  POCKET LEDGER  /  ", "accent")
              << Format::ink(name, "title") << '\n'
              << Format::ink("  Type /cancel to return to the dashboard.", "muted")
              << "\n\n";
}

void section(const std::string& name) {
    std::cout << '\n' << Format::ink("  " + name, "accent") << '\n';
}

void notice(const std::string& msg) {
    std::cout << "  " << msg << '\n';
}

void problem(const std::string& msg) {
    std::cout << Format::ink("  ! " + msg, "expense") << '\n';
}

} // namespace Display