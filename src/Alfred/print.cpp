#include "Alfred/print.h"
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <fstream>
#include <iomanip>

bool Print::verbose = false;
bool Print::logToFile = false;
string Print::logFilePath;

void Print::basicPrint(const string& type, const string& message)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    int micros = tv.tv_usec;
    while (micros > 1000000)
    {
        micros -= 1000000;
        tv.tv_sec++;
    }

    struct tm* tm_info = localtime(&tv.tv_sec);
    char buffer[40];
    strftime(buffer, 40, "%Y:%m:%d %H:%M:%S", tm_info);

    if (logToFile)
    {
        ofstream logFile;
        logFile.open(logFilePath, ios_base::app);
        if (logFile)
        {
            logFile << '[' << type << "] [" << buffer << "." << setw(6) << setfill('0') << micros  << "] " << message << '\n';
        }
        else
        {
            logToFile = false;
            Print::PrintError("Unable to write to " + logFilePath);
            logToFile = true;
        }
    }
    else cout << '[' << type << "] [" << buffer << "." << setw(6) << setfill('0') << micros  << "] " << message << '\n';
}

void Print::PrintError(const string& message)
{
    Print::basicPrint(string(ANSI_COLOR_RED) + "ERROR" + ANSI_COLOR_RESET, message);
}

void Print::PrintWarning(const string& message)
{
    Print::basicPrint(string(ANSI_COLOR_YELLOW) + "WARNING" + ANSI_COLOR_RESET, message);
}

void Print::PrintInfo(const string& message)
{
    Print::basicPrint(string(ANSI_COLOR_GREEN) + "INFO" + ANSI_COLOR_RESET, message);
}

void Print::PrintVerbose(const string& message)
{
    if (verbose)
    {
        Print::basicPrint(string(ANSI_COLOR_BLUE) + "VERBOSE" + ANSI_COLOR_RESET, message);
    }
}

/*
 * Print functions with topic name before message, useful for debugging overlapping execution of topics
 */
void Print::PrintError(string topic, const string& message)
{
    string suffix = topic.substr(topic.size() - 4, topic.size());
    if (suffix == "_ERR" || suffix == "_ANS" || suffix == "_REQ") topic = topic.substr(0, topic.size() - 4);
    PrintError("[" + topic + "] " + message);
}

void Print::PrintWarning(string topic, const string& message)
{
    string suffix = topic.substr(topic.size() - 4, topic.size());
    if (suffix == "_ERR" || suffix == "_ANS" || suffix == "_REQ") topic = topic.substr(0, topic.size() - 4);
    PrintWarning("[" + topic + "] " + message);
}

void Print::PrintInfo(string topic, const string& message)
{
    string suffix = topic.substr(topic.size() - 4, topic.size());
    if (suffix == "_ERR" || suffix == "_ANS" || suffix == "_REQ") topic = topic.substr(0, topic.size() - 4);
    PrintInfo("[" + topic + "] " + message);
}

void Print::PrintVerbose(string topic, const string& message)
{
    string suffix = topic.substr(topic.size() - 4, topic.size());
    if (suffix == "_ERR" || suffix == "_ANS" || suffix == "_REQ") topic = topic.substr(0, topic.size() - 4);
    PrintVerbose("[" + topic + "] " + message);
}

void Print::setVerbose(bool verbose)
{
    Print::verbose = verbose;
}

void Print::setLogFile(const string& filePath)
{
    Print::logToFile = !filePath.empty();
    Print::logFilePath = filePath;
}
