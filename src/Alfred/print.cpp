#include "Alfred/print.h"
#include "Fred/fredMode.h"
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <iomanip>

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

    cout << '[' << type << "] [" << buffer << "." << setw(6) << setfill('0') << micros  << "] " << message << '\n';
}

void Print(const string type, string topic, const string message)
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

	string suffix = topic.substr(topic.size() - 4, topic.size());

	if (suffix == "_ERR" || suffix == "_ANS" || suffix == "_REQ") topic = topic.substr(0, topic.size() - 4);

    cout << '[' << type << "] [" << buffer << "." << setw(6) << setfill('0') << micros  << "] [" << topic << "] " << message << '\n';
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
	extern int fredMode;
	if (fredMode == VERBOSE)
	{
		Print(string(ANSI_COLOR_BLUE) + "VERBOSE" + ANSI_COLOR_RESET, message);
	}
}

/*
 * Print functions with topic name between timestamp and message, useful for debugging overlapping execution of topics 
 */

void PrintError(string topic, string message)
{
	Print(string(ANSI_COLOR_RED) + "ERROR" + ANSI_COLOR_RESET, topic, message);
}

void PrintWarning(string topic, string message)
{
	Print(string(ANSI_COLOR_YELLOW) + "WARNING" + ANSI_COLOR_RESET, topic, message);
}

void PrintInfo(string topic, string message)
{
	Print(string(ANSI_COLOR_GREEN) + "INFO" + ANSI_COLOR_RESET, topic, message);
}

void PrintVerbose(string topic, string message)
{
	extern int fredMode;
	if (fredMode == VERBOSE)
	{
		Print(string(ANSI_COLOR_BLUE) + "VERBOSE" + ANSI_COLOR_RESET, topic, message);
	}
}
