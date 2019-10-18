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
#include "Fred/Protocols/SCA.h"

void SCA::SCApad(string& line)
{
    stringstream ss;

    size_t comma = line.find(',');
    if (comma != string::npos)
    {
        unsigned long command, data;
        command = stol(line.substr(0, comma), 0, 16);
        data = stol(line.substr(comma + 1), 0, 16);

        if (command > 0xffffffff || data > 0xffffffff) throw runtime_error("SCA 16 bits exceeded!");

        ss << "0x" << setw(8) << setfill('0') << hex << command << "," << "0x" << setw(8) << setfill('0') << hex << data;
        line = ss.str();
    }
    else throw runtime_error("SCA comma is missing!");
}

vector<string> SCA::generateMessage(Instructions::Instruction& instructions, vector<string>& outputPattern, vector<string>& pollPattern, ProcessMessage* processMessage)
{
    string message;
    bool parseInVar = instructions.inVar.size() > 0;

    int32_t multiplicity = processMessage->getMultiplicity();
    size_t messageSize = instructions.message.size();

    vector<string> result;

    for (int32_t m = 0; m < multiplicity; m++)
    {
        for (size_t i = 0; i < messageSize; i++)
        {
            string outVar;
            string line = instructions.message[i];

            if (parseInVar) processMessage->parseInputVariables(line, instructions.inVar, m); //parse invariables

            size_t dolPos = line.find('$'); //user poll
            if (dolPos != string::npos)
            {
                string pollEqn = line.substr(dolPos + 1);

                line.erase(dolPos); //remove $eqn

                SCApad(line);

                if (!message.empty())
                {
                    result.push_back(message.erase(message.size() - 1));
                    pollPattern.push_back("");
                    message = "";
                }

                result.push_back(line);
                pollPattern.push_back(pollEqn);

                continue;
            }

            size_t atPos = line.find('@');
            if (atPos != string::npos)
            {
                outVar = line.substr(atPos + 1);
                line.erase(atPos); //remove @OUT_VAR
            }

            SCApad(line);

            outputPattern.push_back(outVar); //push_back outvar name, empty string if not present
            message += line + "\n";
        }
    }

    if (!message.empty())
    {
        result.push_back(message.erase(message.size() - 1));
        pollPattern.push_back("");
    }

    return result;
}

void SCA::checkIntegrity(const string& request, const string& response)
{
    for (size_t i = 0; i < response.size(); i++)
    {
        if (!(isxdigit(response[i]) || response[i] == '\n' || response[i] == ',' || response[i] == 'x'))
        {
            throw runtime_error("SCA: Invalid character received in RPC data:\n" + response + "\n");
        }
    }

    vector<string> reqVec = Utility::splitString(request, "\n");
    vector<string> resVec = Utility::splitString(response, "\n");

    if (reqVec.size() != resVec.size())
    {
        throw runtime_error("SCA: Invalid number of lines received!"); 
    }

    for (size_t i = 0; i < resVec.size(); i++)
    {
        transform(reqVec[i].begin(), reqVec[i].end(), reqVec[i].begin(), ::tolower);
        transform(resVec[i].begin(), resVec[i].end(), resVec[i].begin(), ::tolower);

        if (resVec[i].length() != 21 || (resVec[i].substr(0, resVec[i].find(",")) != reqVec[i].substr(0, reqVec[i].find(","))))
        {
            throw runtime_error("SCA: Integrity check of received message failed!");
        }
    }
}

vector<vector<unsigned long> > SCA::readbackValues(const string& message, vector<string> outputPattern, Instructions::Instruction& instructions)
{
    vector<string>& outVars = instructions.outVar;
    vector<string> splitted = Utility::splitString(message, "\n");
    vector<unsigned long> values;

    for (size_t i = 0; i < splitted.size(); i++)
    {
        size_t pos = splitted[i].find(",");
        if (pos != string::npos)
        {
            values.push_back(stoul(splitted[i].substr(pos + 1), NULL, 16)); //all 32 bits payloads
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

string SCA::valuesToString(vector<vector<unsigned long> > values, int32_t multiplicity)
{
    stringstream result;

    for (int32_t m = 0; m < multiplicity; m++)
    {
        for (size_t v = 0; v < values.size(); v++)
        {
            result << "0x" << setw(8) << setfill('0') << hex << values[v][m];
            if (v < values.size() - 1) result << ",";
        }
        if (m < multiplicity - 1) result << "\n";
    }

    return result.str();
}
