/*
Uncomment commented lines and recompile to use MAPI example
*/

#include <stdio.h>
#include "Fred/fred.h"
#include "Alfred/print.h"
#include "Fred/fredMode.h"
#include <boost/program_options.hpp>

//#include "Mapi/mapiexample.h"

int fredMode = DEFAULT;

int main(int argc, char** argv)
{
	namespace po = boost::program_options;
	po::options_description description("FRED options");
	description.add_options()
    ("help, h", "Print help message")
    ("verbose, v", "Verbose output")
    ("parser, p", "Parse config files then exit");

    po::variables_map vm;

    try
    {
    	po::store(po::parse_command_line(argc, argv, description), vm);
    	if (vm.count("parser"))
    	{
     		fredMode = PARSER;
     		PrintWarning("Parse only mode!");
    	}
     	else if (vm.count("help"))
     	{
        	cout << description << endl;
        	return 0;
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

    try
    {
        //MapiExample mapiExample;
        //fred.registerMapiObject("FRED1/MAPI_EXAMPLE/LOOP0/TEST", &mapiExample);
    }
    catch (exception& e)
    {
        exit(-1);
    }

    fred.Start();
    return 0;
}
