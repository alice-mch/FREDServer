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

void PrintError(string message);
void PrintWarning(string message);
void PrintInfo(string message);

#endif