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
#include "Fred/Protocols/SWT.h"

string SWT::generateMessage(Instructions::Instruction& instructions, vector<string>& outputPattern, ProcessMessage* processMessage)
{
    bool parseInVar = instructions.inVar.size() > 0;

    int32_t multiplicity = processMessage->getMultiplicity();
    size_t messageSize = instructions.message.size();

    string message = "reset\n";

    for (int32_t m = 0; m < multiplicity; m++)
    {
        for (size_t i = 0; i < messageSize; i++)
        {
            string outVar;
            stringstream ss;
            string line = instructions.message[i]; //add raw user line

            if (parseInVar) processMessage->parseInputVariables(line, instructions.inVar, m); //parse invariables

            if (line.find("0x") != string::npos) line = line.substr(2); //remove eventual "0x"

            size_t atPos = line.find('@');

            if (atPos != string::npos) //user read @OUT_VAR
            {
                outVar = line.substr(atPos + 1);

                line.erase(atPos); //remove @OUT_VAR

                if (line.length() > SWT_WIDTH) throw runtime_error("SWT " + to_string(SWT_WIDTH * 4) + " bits exceeded!"); //check SWT max width

                ss << "0x" << setw(SWT_WIDTH) << setfill('0') << hex << line << ",write\nread";
                line = ss.str();
                outputPattern.push_back("");
                outputPattern.push_back(outVar);

            }
            else //user write
            {
                outputPattern.push_back("");
                if (line.length() > SWT_WIDTH) throw runtime_error("SWT " + to_string(SWT_WIDTH * 4) + " bits exceeded!"); //check SWT max width

                ss << "0x" << setw(SWT_WIDTH) << setfill('0') << hex << line  << ",write";
                line = ss.str();
            }

            //outputPattern.push_back(outVar); //push_back outvar name, empty string if not present
            message += line + "\n";
        }
    }

    message.erase(message.size() - 1);
    return message;
}

void SWT::checkIntegrity(const string& request, const string& response)
{
    for (size_t i = 0; i < response.size(); i++)
    {
        if (!(isxdigit(response[i]) || response[i] == '\n' || response[i] == 'x'))
        {
            throw runtime_error("SWT: Invalid character received in RPC data:\n" + response + "\n");
        }
    }

    vector<string> reqVec = Utility::splitString(request, "\n");
    vector<string> resVec = Utility::splitString(response, "\n");

    reqVec.erase(reqVec.begin()); //remove first line "reset" 

    if (reqVec.size() != resVec.size())
    {
        throw runtime_error("SWT: Invalid number of lines received!");
    }

    for (size_t i = 0; i < resVec.size(); i++)
    {
        transform(reqVec[i].begin(), reqVec[i].end(), reqVec[i].begin(), ::tolower);
        transform(resVec[i].begin(), resVec[i].end(), resVec[i].begin(), ::tolower);

        if (resVec[i].length() != (SWT_WIDTH + strlen("0x")) && (resVec[i].length() != strlen("0")))
        {
            throw runtime_error("SWT: Invalid width message received!");
        }

        if ( (reqVec[i].find("write") != string::npos && resVec[i] != "0") || (reqVec[i].find("read") != string::npos && resVec[i] == "0"))
        {
            throw runtime_error("SWT: Integrity check of received message failed!");
        }
    }
}

vector<vector<unsigned long> > SWT::readbackValues(const string& message, vector<string> outputPattern, Instructions::Instruction& instructions)
{
    vector<string>& outVars = instructions.outVar;
    vector<string> splitted = Utility::splitString(message, "\n");
    vector<unsigned long> values;

    for (size_t i = 0; i < splitted.size(); i++)
    {
        {
            values.push_back(stoul(splitted[i].size() > 4 ? splitted[i].substr(splitted[i].size() - 4) : splitted[i], NULL, 16)); //last 4 or 0
        }
    }

    vector<vector<unsigned long> > results(outVars.size(), vector<unsigned long>());


    for (size_t i = 0; i < values.size(); i++) //for each line of the response
    {
        if (outputPattern[i] != "") //if there is an outvar in the request line
        {
            size_t id = distance(outVars.begin(), find(outVars.begin(), outVars.end(), outputPattern[i]));
            results[id].push_back(values[i]); //push into results
        }
    }

    return results;
}

string SWT::valuesToString(vector<vector<unsigned long> > values, int32_t multiplicity)
{
    stringstream result;

    for (int32_t m = 0; m < multiplicity; m++)
    {
        for (size_t v = 0; v < values.size(); v++)
        {
            result << "0x" << setw(4) << setfill('0') << hex << values[v][m];
            if (v < values.size() - 1) result << ",";
        }
        if (m < multiplicity - 1) result << "\n";
    }

    return result.str();
}
