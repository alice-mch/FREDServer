#ifndef __PRINT
#define __PRINT

#include <iostream>
#include <string>
#include <cstring>
#include <time.h>

using namespace std;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BLUE    "\x1b[38;5;27m"

void PrintError(string message);
void PrintWarning(string message);
void PrintInfo(string message);
void PrintVerbose(string message);

void PrintError(string topic, string message);
void PrintWarning(string topic, string message);
void PrintInfo(string topic, string message);
void PrintVerbose(string topic, string message);

#endif