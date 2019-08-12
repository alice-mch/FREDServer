#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include "Mapi/testgroup.h"
#include "Alfred/print.h"
#include "Parser/utility.h"

Testgroup::Testgroup()
{

}

string Testgroup::processInputMessage(string input)
{
	vector< pair <string, string> > requests;

	if (input == "go")
	{
		requests.push_back(make_pair("FREDTEST/ITERATIVEBOARDS/ITERATIVEBOARD0/INTERNAL/AVERAGE", "10"));
		requests.push_back(make_pair("FREDTEST/ITERATIVEBOARDS/ITERATIVEBOARD0/INTERNAL/AVERAGE", "100"));
		requests.push_back(make_pair("FREDTEST/ITERATIVEBOARDS/ITERATIVEBOARD0/INTERNAL/AVERAGE", "1000"));
		
		newMapiGroupRequest(requests);
	}
	
	requests.empty();
	
	noRpcRequest = true;

    return "";
}

string Testgroup::processOutputMessage(string output)
{
	return output;
}
