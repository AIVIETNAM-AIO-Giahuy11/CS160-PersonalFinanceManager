#ifndef FINANCE_UI_H
#define FINANCE_UI_H

#include "finance.h"
#include <string>


void startConsole();
void finishConsole();
int input(const std::string& label, std :: string &text);
int number(const std::string& label, int low, int high, int& value);
void title(const std::string& name);
void section(const std::string& name);
void notice(const std::string& msg);
void problem(const std::string& msg);
int pauseScreen();
std::string money(long long amount);
std::string ink(const std::string& value, const std::string& role);
int dashboard(const Finance& db, const std::string& message, int& choice);
int reports(const Finance& db);

#endif
