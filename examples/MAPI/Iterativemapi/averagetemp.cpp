#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include "Mapi/averagetemp.h"
#include "Alfred/print.h"
#include "Parser/utility.h"

Averagetemp::Averagetemp()
{
	counter = 0;
}

string Averagetemp::processInputMessage(string input)
{
	PrintInfo(input + " remaining queries");
	counter = stoi(input);	

	if (input == "test")
	{
		string message = "message";
		publishAnswer(message);
		
		string error = "error";
		publishError(error);
	}

	sequence = "0x25030000"; //internal temperature read
    return sequence;
}

string Averagetemp::processOutputMessage(string output)
{
	int temp;
	string value;

	try
	{
		output.erase(remove(output.begin(), output.end(), '\n'), output.end());
		value = output.substr(output.size() - 4, output.size());
		temp = stoi(value, nullptr, 16);

		PrintInfo("Temperature: " + to_string(temp));
	}
	catch (exception& e)
	{
		PrintError("ERROR!");
		returnError = true;
		return "ERROR!";
	}

	if (counter)
	{
		counter--;
		temperatures.push_back(temp);

		newRequest(to_string(counter));

		noReturn = true;
		return "";
	}
	else
	{
		float average = accumulate( temperatures.begin(), temperatures.end(), 0.0)/temperatures.size();
		temperatures.clear();
		return to_string(average);
	}
}
