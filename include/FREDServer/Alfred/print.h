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

class Print
{
private:
    static bool verbose;
    static bool logToFile;
    static string logFilePath;

    static void basicPrint(const string& type, const string& message);

    Print() {};

public:
    static void PrintError(const string& message);
    static void PrintWarning(const string& message);
    static void PrintInfo(const string& message);
    static void PrintVerbose(const string& message);

    static void PrintError(string topic, const string& message);
    static void PrintWarning(string topic, const string& message);
    static void PrintInfo(string topic, const string& message);
    static void PrintVerbose(string topic, const string& message);

    static void setVerbose(bool verbose);
    static void setLogFile(const string& filePath);
};

#endif
