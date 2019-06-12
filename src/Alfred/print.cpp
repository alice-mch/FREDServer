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
