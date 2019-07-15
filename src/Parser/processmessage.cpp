#include <cctype>
#include <math.h>
#include <algorithm>
#include <string>
#include "Parser/processmessage.h"
#include "Alfred/print.h"
#include "Parser/utility.h"
#include "Fred/fredtopics.h"
#include "Fred/groupcommand.h"

const string ProcessMessage::SUCCESS = "success";
const string ProcessMessage::FAILURE = "failure";

ProcessMessage::ProcessMessage(string message, int32_t placeId)
{
    groupCommand = NULL;
    mapi = NULL;
    correct = checkMessage(message);
    if (correct)
    {
        try
        {
            input = Utility::splitMessage2Num(message);
        }
        catch (exception& e)
        {
            PrintError("Invalid input received!");
            correct = false;
            return;
        }

        size_t inSize = input.size();
        if (inSize)
        {
            for (size_t i = 0; i < inSize; i++) input[i].insert(input[i].begin(), uint32_t(placeId));
        }
        else
        {
            input.push_back(vector<uint32_t>());
            input[0].push_back(uint32_t(placeId));
        }
    }
}

ProcessMessage::ProcessMessage(map<string, vector<uint32_t> > inVars, int32_t placeId, GroupCommand* groupCommand)
{
    this->groupCommand = groupCommand;
    mapi = NULL;
    correct = true;

    size_t varSize = inVars.size();
    if (varSize > 0)
    {
        size_t multiplicity = inVars.begin()->second.size();

        input.resize(multiplicity);
        for (size_t i = 0; i < multiplicity; i++) input[i].resize(varSize);

        size_t m = 0;
        for (auto it = inVars.begin(); it != inVars.end(); it++)
        {
            for (size_t i = 0; i < it->second.size(); i++)
            {
                input[i][m] = it->second[i];
            }

            m++;
        }
    }

    size_t inSize = input.size();
    if (inSize)
    {
        for (size_t i = 0; i < inSize; i++) input[i].insert(input[i].begin(), uint32_t(placeId));
    }
    else
    {
        input.push_back(vector<uint32_t>());
        input[0].push_back(uint32_t(placeId));
    }
}

ProcessMessage::ProcessMessage(MapiInterface* mapi, string input)
{
    this->mapi = mapi;
    this->fullMessage = input;
}

bool ProcessMessage::checkMessage(string& message)
{
    for (size_t i = 0; i < message.size(); i++)
    {
        if (!(isxdigit(message[i])  || message[i] == ',' || message[i] == '\n' || message[i] == 'x'))
        {
            PrintError("Request contains forbidden characters!");
            return false;
        }
    }

    return true;
}

int32_t ProcessMessage::getMultiplicity()
{
    return input.size() == 0 ? 1 : input.size();
}

bool ProcessMessage::isCorrect()
{
    return correct;
}

string& ProcessMessage::generateFullMessage(Instructions::Instruction& instructions)
{
    outputPattern.clear();

    string line;
    bool parseInVar = instructions.inVar.size() > 0;

    int32_t multiplicity = getMultiplicity();
    size_t messageSize = instructions.message.size();

    for (int32_t m = 0; m < multiplicity; m++)
    {
        for (size_t i = 0; i < messageSize; i++)
        {
            line = instructions.message[i];

            checkForOutput(line);

            if (parseInVar)
            {
                parseInputVariables(line, instructions.inVar, m);
            }

            fullMessage += line + "\n";
        }
    }

    fullMessage.erase(fullMessage.size() - 1);

    return fullMessage;
}

void ProcessMessage::checkForOutput(string &line)
{
    string outVar;

    size_t atPos = line.find('@');
    if (atPos != string::npos)
    {
        outVar = line.substr(atPos + 1);
        line.erase(atPos);
    }

    outputPattern.push_back(outVar);
}

void ProcessMessage::parseInputVariables(string& line, vector<string>& inVars, int32_t iteration)
{
    if (input[iteration].size() < inVars.size())
    {
        PrintWarning("Number of arguments doesn't correspond with the config file!");
        input[iteration].resize(inVars.size(), 0);
    }

    size_t left, right;
    while ((left = line.find("[")) != string::npos && (right = line.find("]")) != string::npos)
    {
        string equation = line.substr(left + 1, right - left - 1);
        int32_t value = int32_t(roundf(Utility::calculateEquation(equation, inVars, input[iteration]))) & 0xFF;

        char number[3];
        sprintf(number, "%02X", value);

        line.replace(left, right - left + 1, string(number));
    }
}

vector<vector<uint32_t> > ProcessMessage::readbackValues(const string& message, Instructions::Instruction& instructions)
{
    for (size_t i = 0; i < message.size(); i++)
    {
        if (!(isxdigit(message[i]) || message[i] == '\n' || message[i] == ','))
        {
            PrintError("Invalid character received in RPC data:\n" + message + "\n");
            throw runtime_error("Invalid character received in RPC data!");
        }
    }

    bool checkMessage;
    try
    {
        checkMessage = Utility::checkMessageIntegrity(this->fullMessage, message, instructions.type);
    }
    catch (exception& ex)
    {
        throw runtime_error(ex.what());
    }

    if (!checkMessage)
    {
        return vector<vector<uint32_t> >();
    }

    vector<string>& outVars = instructions.outVar;
    vector<uint32_t> values = Utility::splitAlfResponse(message, instructions.type);

    vector<vector<uint32_t> > results(outVars.size(), vector<uint32_t>());

    for (size_t i = 0; i < values.size(); i++)
    {
        if (outputPattern[i] != "")
        {
            size_t id = distance(outVars.begin(), find(outVars.begin(), outVars.end(), outputPattern[i]));
            results[id].push_back(values[i]);
        }
    }

    return results;
}

vector<double> ProcessMessage::calculateReadbackResult(vector<vector<uint32_t> >& result, Instructions::Instruction& instructions)
{
    vector<double> resultValues;
    vector<string>& outVars = instructions.outVar;

    for (size_t m = 0; m < getMultiplicity(); m++)
    {
        vector<uint32_t> received;
        for (size_t v = 0; v < outVars.size(); v++) received.push_back(result[v][m]);

        resultValues.push_back(Utility::calculateEquation(instructions.equation, outVars, received));
    }

    return resultValues;
}

void ProcessMessage::evaluateMessage(string message, ChainTopic &chainTopic, bool ignoreStatus)
{
    try
    {
        if (!ignoreStatus) //ignoreStatus is true only for alfinfo (not the usual alfrpcinfo), so for SUBSCRIBE only
        {
            if (!(message.find(SUCCESS) != string::npos)) //not a SUCCESS
            {
                string response;

                if (message.empty()) //empty message
                {
                    response = "Empty response received!";
                    PrintError(response);
                }
                else if (message.find(FAILURE) != string::npos) //FAILURE message
                {
                    PrintError("Error message received:\n" + message + "\n");
                    replace(message.begin(), message.end(), '\n', ';');
                    response = message;
                }
                else //unknown message
                {
                    PrintError("Unknown message received:\n" + message + "\n");
                    replace(message.begin(), message.end(), '\n', ';');
                    response = message;
                }
                
                if (groupCommand == NULL)
                {
                    chainTopic.error->Update(response.c_str());
                    PrintError("Updating error service!");
                }
                else groupCommand->receivedResponse(&chainTopic, response, true);

                return;
            }
            else if (message.find(SUCCESS) != string::npos) //SUCCESS
            {
                string response;
                vector<vector<uint32_t> > values;
                try
                {
                    values = readbackValues(message.substr(SUCCESS.length() + 1), *chainTopic.instruction);
                }
                catch (exception& e)
                {
                    response = e.what();
                    replace(message.begin(), message.end(), '\n', ';');
                    response += ";" + message;
                    if (groupCommand == NULL)
                    {
                        chainTopic.error->Update(response.c_str());
                        PrintError("Updating error service!");
                    }
                    else groupCommand->receivedResponse(&chainTopic, response, true);
                    
                    return;
                }

                if (values.empty())
                {
                    response = "OK"; //FRED ACK response

                    if (groupCommand == NULL)
                    {
                        PrintVerbose("Updating service");
                        chainTopic.service->Update(response.c_str());
                    }
                    else groupCommand->receivedResponse(&chainTopic, response, false);

                    return;
                }

                if (chainTopic.instruction->equation != "")
                {
                    vector<double> realValues = calculateReadbackResult(values, *chainTopic.instruction);
                    response = Utility::readbackToString(realValues);
                }
                else
                {
                    response = Utility::readbackToString(values, getMultiplicity());
                }

                if (groupCommand == NULL)
                {
                    PrintVerbose("Updating service");
                    chainTopic.service->Update(response.c_str());
                }
                else groupCommand->receivedResponse(&chainTopic, response, false);

                return;
            }
        }
        else //ignoreStatus is true only for alfinfo (not the usual alfrpcinfo), so for SUBSCRIBE only 
        {
            string response;
            vector<vector<uint32_t> > values;
            try
            {
                values = readbackValues(message, *chainTopic.instruction);
            }
            catch (exception& e)
            {
                response = e.what();
                replace(message.begin(), message.end(), '\n', ';');
                response += ";" + message;
                if (groupCommand == NULL)
                {
                    chainTopic.error->Update(response.c_str());
                    PrintError("Updating error service!");
                }
                else groupCommand->receivedResponse(&chainTopic, response, true);
                
                return;
            }

            if (values.empty())
            {
                response = "OK"; //FRED ACK response  
                chainTopic.service->Update(response.c_str());

                return;
            }

            if (chainTopic.instruction->equation != "")
            {
                vector<double> realValues = calculateReadbackResult(values, *chainTopic.instruction);
                response = Utility::readbackToString(realValues);
            }
            else
            {
                response = Utility::readbackToString(values, getMultiplicity());
            }

            chainTopic.service->Update(response.c_str());
            
            return;
        }
    }
    catch (exception& e)
    {
        string response = "Error in message evaluation! Incorrect data received!";
        PrintError(response);

        if (groupCommand == NULL)
        {
            chainTopic.error->Update(response.c_str());
            PrintError("Updating error service!");
        }
        else groupCommand->receivedResponse(&chainTopic, response, true);
    }
}

string ProcessMessage::generateMapiMessage()
{
    return mapi->processInputMessage(this->fullMessage);
}

void ProcessMessage::evaluateMapiMessage(string message, ChainTopic& chainTopic)
{
    string response = mapi->processOutputMessage(message);
    chainTopic.service->Update(response.c_str());
}
