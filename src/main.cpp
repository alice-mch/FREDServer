/*
 * Uncomment commented lines and recompile to use MAPI example
 */

#include <stdio.h>
#include <fstream>
#include "Fred/fred.h"
#include "Fred/global.h"
#include "Alfred/print.h"
//#include "Mapi/mapiexample.h"

int main(int argc, char** argv)
{
	parseOnly = Fred::commandLineArguments(argc, argv);
	pair<string, string> config = Fred::readConfigFile();
	Fred fred(parseOnly, config.first, config.second, "./sections");

	//MapiExample mapiExample;
	
	try
	{
		//fred.registerMapiObject("FRED1/MAPI_EXAMPLE/LOOP0/TEST", &mapiExample);
	}
	catch (exception& e)
	{
		exit(EXIT_FAILURE);
	}

	fred.Start();
	return 0;
}
