#include <cctype>
#include <math.h>
#include <algorithm>
#include <string>
#include <iomanip>
#include <sstream>
#include "Parser/processmessage.h"
#include "Alfred/print.h"
#include "Parser/utility.h"
#include "Fred/fredtopics.h"
#include "Fred/groupcommand.h"
#include "Fred/Config/instructions.h"
#include "Fred/Protocols/SCA.h"
#include "Fred/Protocols/SWT.h"
#include "Fred/Protocols/IC.h"

const string ProcessMessage::SUCCESS = "success";
const string ProcessMessage::FAILURE = "failure";

/*
 * ProcessMessage constructor for regular topics 
 */
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
    else
    {
        correct = false;
        return;
    }
}

/*
 * ProcessMessage constructor for group topics 
 */
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

/*
 * ProcessMessage constructor for MAPI topics 
 */
ProcessMessage::ProcessMessage(Mapi* mapi, string input)
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

string ProcessMessage::generateFullMessage(Instructions::Instruction& instructions)
{
    outputPattern.clear(); //clear outvar name vector
    try
    {
        switch (instructions.type)
        {
            case Instructions::Type::SWT: fullMessage = SWT::generateMessage(instructions, outputPattern, this);
                break;
            case Instructions::Type::SCA: fullMessage = SCA::generateMessage(instructions, outputPattern, this);
                break;
            case Instructions::Type::IC: fullMessage = IC::generateMessage(instructions, outputPattern, this);
                break;
        }
    }
    catch (exception& e)
    {
        throw runtime_error(e.what());
    }

    return fullMessage;
}

void ProcessMessage::parseInputVariables(string& line, vector<string>& inVars, int32_t iteration)
{
    if (input[iteration].size() != inVars.size())
    {
        throw runtime_error("Number of arguments doesn't correspond with the config file!");
    }

    size_t left, right;
    while ((left = line.find("[")) != string::npos && (right = line.find("]")) != string::npos)
    {
        string equation = line.substr(left + 1, right - left - 1);
        int32_t value = int32_t(roundf(Utility::calculateEquation(equation, inVars, input[iteration]))) & 0xFF;

        char number[3];
        sprintf(number, "%02X", value); //two hexadecimal characters format

        line.replace(left, right - left + 1, string(number));
    }
}

vector<vector<unsigned long> > ProcessMessage::readbackValues(const string& message, Instructions::Instruction& instructions)
{
    vector<vector<unsigned long> > results;
 
    try
    {
        switch (instructions.type)
        {
            case Instructions::Type::SWT: results = SWT::readbackValues(message, outputPattern, instructions);
                break;
            case Instructions::Type::SCA: results = SCA::readbackValues(message, outputPattern, instructions);
                break;
            case Instructions::Type::IC: results = IC::readbackValues(message, outputPattern, instructions);
                break;
        }
    }
    catch (exception& e)
    {
        throw runtime_error(e.what());
    }

    return results;
}

vector<double> ProcessMessage::calculateReadbackResult(vector<vector<unsigned long> >& result, Instructions::Instruction& instructions)
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

string ProcessMessage::valuesToString(vector<vector<unsigned long> > values, int32_t multiplicity, Instructions::Type type)
{
    string response;
 
    try
    {
        switch (type)
        {
            case Instructions::Type::SWT: response = SWT::valuesToString(values, multiplicity);
                break;
            case Instructions::Type::SCA: response = SCA::valuesToString(values, multiplicity);
                break;
            case Instructions::Type::IC: response = IC::valuesToString(values, multiplicity);
                break;
        }
    }
    catch (exception& e)
    {
        throw runtime_error(e.what());
    }

    return response;
}

/*
 * Update _ANS or _ERR response channel
 */
void ProcessMessage::updateResponse(ChainTopic& chainTopic, string response, bool error)
{
    if (error)
    {
        if (groupCommand == NULL)
        {
            chainTopic.error->Update(response.c_str());
            PrintError(chainTopic.name, "Updating error service!");
        }
        else groupCommand->receivedResponse(&chainTopic, response, true);
    }
    else
    {
        if (groupCommand == NULL)
        {
            chainTopic.service->Update(response.c_str());
            PrintVerbose(chainTopic.name, "Updating service");
        }
        else groupCommand->receivedResponse(&chainTopic, response, false);        
    }
}

void ProcessMessage::evaluateMessage(string message, ChainTopic &chainTopic, bool ignoreStatus)
{
    try
    {
        //if (!ignoreStatus) //ignoreStatus is true only for alfinfo (not the usual alfrpcinfo), so for SUBSCRIBE only
        //{
            string response;
            if (message == "NO RPC LINK!")
            {
                response = "ALF_" + chainTopic.unit->alfId + " is not responding!";
                PrintError(chainTopic.name, response);
                
                updateResponse(chainTopic, response, true);
                return;
            }
            else if (message.find(SUCCESS) != string::npos) //SUCCESS
            {
                vector<vector<unsigned long> > values;
                try
                {
                    Utility::checkMessageIntegrity(this->fullMessage, message.substr(SUCCESS.length() + 1), chainTopic.instruction->type); //check message integrity
                    values = readbackValues(message.substr(SUCCESS.length() + 1), *chainTopic.instruction); //extract eventual outvars
                }
                catch (exception& e)
                {
                    PrintError(chainTopic.name, e.what());
                    response = e.what();
                    replace(message.begin(), message.end(), '\n', ';');
                    response += ";" + message;

                    updateResponse(chainTopic, response, true);
                    return;
                }
                if (values.empty())
                {
                    response = "OK"; //FRED ACK response

                    updateResponse(chainTopic, response, false);
                    return;
                }

                if (chainTopic.instruction->equation != "")
                {
                    vector<double> realValues = calculateReadbackResult(values, *chainTopic.instruction);
                    response = Utility::readbackToString(realValues);
                }
                else
                {
                    response = valuesToString(values, getMultiplicity(), chainTopic.instruction->type);
                }

                updateResponse(chainTopic, response, false);
                return;
            }
            else if (!(message.find(SUCCESS) != string::npos)) //not a SUCCESS
            {
                string response;

                if (message.empty()) //empty message
                {
                    response = "Empty response received!";
                    PrintError(chainTopic.name, response);
                }
                else if (message.find(FAILURE) != string::npos) //FAILURE message
                {
                    PrintError(chainTopic.name, "Error message received:\n" + message + "\n");
                    replace(message.begin(), message.end(), '\n', ';');
                    response = message;
                }
                else //unknown message
                {
                    PrintError(chainTopic.name, "Unknown message received:\n" + message + "\n");
                    replace(message.begin(), message.end(), '\n', ';');
                    response = message;
                }
                
                updateResponse(chainTopic, response, true);
                return;
            }
        //}
        //else //ignoreStatus is true only for alfinfo (not the usual alfrpcinfo), so for SUBSCRIBE only 
        //{
        //    string response;
        //    vector<vector<unsigned long> > values;
        //    try
        //    {
        //        values = readbackValues(message.substr(SUCCESS.length() + 1), *chainTopic.instruction);
        //    }
        //    catch (exception& e)
        //    {
        //        PrintError(chainTopic.name, e.what());
        //        response = e.what();
        //        replace(message.begin(), message.end(), '\n', ';');
        //        response += ";" + message;

        //        updateResponse(chainTopic, response, true);
        //        return;
        //    }

        //    if (values.empty())
        //    {
        //        response = "OK"; //FRED ACK response  
        //        chainTopic.service->Update(response.c_str());

        //        return;
        //    }

        //    if (chainTopic.instruction->equation != "")
        //    {
        //        vector<double> realValues = calculateReadbackResult(values, *chainTopic.instruction);
        //        response = Utility::readbackToString(realValues);
        //    }
        //    else
        //    {
        //        response = valuesToString(values, getMultiplicity(), chainTopic.instruction->type);
        //    }

        //    chainTopic.service->Update(response.c_str());
        //    return;
        //}
    }
    catch (exception& e)
    {
        string response = "Error in message evaluation! Incorrect data received!";
        PrintError(chainTopic.name, response);
        updateResponse(chainTopic, response, true);
    }
}

string ProcessMessage::generateMapiMessage()
{
    return mapi->processInputMessage(this->fullMessage);
}

void ProcessMessage::evaluateMapiMessage(string message, ChainTopic& chainTopic)
{
    string response = mapi->processOutputMessage(message);

    if (!mapi->noReturn)
    {
        if (mapi->returnError)
        {
            chainTopic.error->Update(response.c_str());
            PrintError(chainTopic.name, "Updating MAPI error service!");
            mapi->returnError = false; //reset returnError
        }
        else
        {
            chainTopic.service->Update(response.c_str());
            PrintVerbose(chainTopic.name, "Updating MAPI service");
        }
    }
    else
    {
        PrintVerbose(chainTopic.name, "Mapi is noReturn");
        mapi->noReturn = false; //reset noReturn
    }
}
