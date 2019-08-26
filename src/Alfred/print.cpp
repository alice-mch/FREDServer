#include "Alfred/print.h"
#include "Fred/fredMode.h"
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <fstream>
#include <iomanip>

extern bool logToFile;
extern string logFilePath;
extern int fredMode;

void Print(const string type, const string message)
{
	/*time_t t;
	time(&t);

	char *normTime = ctime(&t);
	(*strchr(normTime, '\n')) = '\0';*/
	struct timeval tv;
	gettimeofday(&tv, NULL);

    /*int millis = lrint(tv.tv_usec / 1000.0);
	if (millis >= 1000)
	{
		millis -= 1000;
		tv.tv_sec++;
    }*/

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
			PrintError("Unable to write to " + logFilePath);
			logToFile = true;
		}
	}
	else cout << '[' << type << "] [" << buffer << "." << setw(6) << setfill('0') << micros  << "] " << message << '\n';
}

void PrintError(string message)
{
	Print(string(ANSI_COLOR_RED) + "ERROR" + ANSI_COLOR_RESET, message);
}

void PrintWarning(string message)
{
	Print(string(ANSI_COLOR_YELLOW) + "WARNING" + ANSI_COLOR_RESET, message);
}

void PrintInfo(string message)
{
	Print(string(ANSI_COLOR_GREEN) + "INFO" + ANSI_COLOR_RESET, message);
}

void PrintVerbose(string message)
{
	if (fredMode == VERBOSE)
	{
		Print(string(ANSI_COLOR_BLUE) + "VERBOSE" + ANSI_COLOR_RESET, message);
	}
}

/*
 * Print functions with topic name before message, useful for debugging overlapping execution of topics 
 */

void PrintError(string topic, string message)
{
	string suffix = topic.substr(topic.size() - 4, topic.size());
	if (suffix == "_ERR" || suffix == "_ANS" || suffix == "_REQ") topic = topic.substr(0, topic.size() - 4);
	PrintError("[" + topic + "] " + message);
}

void PrintWarning(string topic, string message)
{
	string suffix = topic.substr(topic.size() - 4, topic.size());
	if (suffix == "_ERR" || suffix == "_ANS" || suffix == "_REQ") topic = topic.substr(0, topic.size() - 4);
	PrintWarning("[" + topic + "] " + message);
}

void PrintInfo(string topic, string message)
{
	string suffix = topic.substr(topic.size() - 4, topic.size());
	if (suffix == "_ERR" || suffix == "_ANS" || suffix == "_REQ") topic = topic.substr(0, topic.size() - 4);
	PrintInfo("[" + topic + "] " + message);
}

void PrintVerbose(string topic, string message)
{
	string suffix = topic.substr(topic.size() - 4, topic.size());
	if (suffix == "_ERR" || suffix == "_ANS" || suffix == "_REQ") topic = topic.substr(0, topic.size() - 4);
	PrintVerbose("[" + topic + "] " + message);
}
