#include <cctype>
#include <math.h>
#include <algorithm>
#include <string>
#include <iomanip>
#include <sstream>
#include "Parser/processmessage.h"
#include "Alfred/print.h"
#include "Parser/utility.h"
#include "Fred/Config/instructions.h"
#include "Fred/Protocols/IC.h"

vector<string> IC::generateMessage(Instructions::Instruction& instructions, vector<string>& outputPattern, vector<string>& pollPattern, ProcessMessage* processMessage)
{
	string message;
	for (size_t i = 0; i < instructions.message.size(); i++)
    {
		message += instructions.message[i]; //send raw sequence, parsing to be defined
        message += "\n";

    }
    message.erase(message.size() - 1);
    return vector<string>({ message });
}

void IC::checkIntegrity(const string& request, const string& response)
{
    //to be defined
}

vector<vector<unsigned long> > IC::readbackValues(const string& message, vector<string> outputPattern, Instructions::Instruction& instructions)
{
    vector<vector<unsigned long> > results;
    //to be defined
    return results;
}

string IC::valuesToString(vector<vector<unsigned long> > values, int32_t multiplicity)
{
    stringstream result;
    //to be defined
    return result.str();
}
