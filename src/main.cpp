/*
 * Uncomment commented lines and recompile to use MAPI example
 */

#include <stdio.h>
#include <fstream>
#include "Fred/fred.h"
#include "Fred/global.h"
#include "Alfred/print.h"
//#include "Mapi/mapiexample.h"
#include "Mapi/sampa_config.h"

int main(int argc, char** argv)
{
	parseOnly = Fred::commandLineArguments(argc, argv);
	pair<string, string> config = Fred::readConfigFile();
	Fred fred(parseOnly, config.first, config.second, "./sections");

	//MapiExample mapiExample;
	SampaConfig sampaConfig;

	try
	{
    fred.registerMapiObject("FRED1/SAMPA_CONFIG/SOLAR1/SAMPA_REG", &sampaConfig);
	}
	catch (exception& e)
	{
		exit(EXIT_FAILURE);
	}

	fred.Start();
	return 0;
}
