/*
Uncomment commented lines and recompile to use MAPI example
*/

#include <stdio.h>
#include <fstream>
#include "Fred/fred.h"
#include "Alfred/print.h"
#include "Fred/fredMode.h"
#include <boost/program_options.hpp>
#include "Mapi/sampa_config.h"
//#include "Mapi/mapiexample.h"

int fredMode = DEFAULT;
bool logToFile = false;
string logFilePath;

int main(int argc, char** argv)
{
	namespace po = boost::program_options;
	po::options_description description("FRED options");
	description.add_options()
	("help, h", "Print help message")
	("verbose, v", "Verbose output")
	("parser, p", "Parse config files then exit")
	("log, l", po::value<string>(),"Log to file <file>");

	po::variables_map vm;

	try
	{
		po::store(po::parse_command_line(argc, argv, description), vm);
		if (vm.count("help"))
		{
			cout << description << endl;
			return 0;
		}
		if (vm.count("log"))
		{
			logFilePath = vm["log"].as<string>();
			ofstream logFile;

			logFile.open(logFilePath, ios_base::app);
			if (logFile)
			{
				PrintInfo("FRED launched, logging to " + logFilePath); //inform user
				logToFile = true;
				PrintInfo("FRED launched, logging to " + logFilePath); //inform via log file
			}
			else
			{
				logToFile = false;
				PrintError("FRED launched, log file " + logFilePath + " is not writable, falling back to standard output!");
			}
			logFile.close();
		}
		else
		{
			PrintInfo("FRED launched!");
		}
		if (vm.count("parser"))
		{
			fredMode = PARSER;
			PrintWarning("Parse only mode!");
		}
		else if (vm.count("verbose"))
		{
			fredMode = VERBOSE;
			PrintWarning("FRED is verbose!");
		}
		po::notify(vm);
	}
	catch (po::error& e)
	{
		PrintError(e.what());
		cerr << description << endl;
		return 0;
	}

	pair<string, string> config = Fred::readConfigFile();
	Fred fred(config.first, config.second, "./sections");

	//MapiExample mapiExample;
	SampaConfig sampaConfig;

	try
	{
	  fred.registerMapiObject("FRED1/SAMPA_CONFIG/SOLAR1/SAMPA_REG", &sampaConfig);
	}
	catch (exception& e)
	{
		exit(-1);
	}

	fred.Start();
	return 0;
}
