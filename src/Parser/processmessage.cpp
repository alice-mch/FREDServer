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
ProcessMessage::ProcessMessage(string message, int32_t placeId, bool useCru)
{
    groupCommand = NULL;
    mapi = NULL;
    this->useCru = useCru;
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
            for (size_t i = 0; i < inSize; i++) input[i].insert(input[i].begin(), double(placeId));
        }
        else
        {
            input.push_back(vector<double>());
            input[0].push_back(double(placeId));
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
ProcessMessage::ProcessMessage(map<string, vector<double> > inVars, int32_t placeId, GroupCommand* groupCommand, bool useCru)
{
    this->groupCommand = groupCommand;
    mapi = NULL;
    correct = true;
    this->useCru = useCru;

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
        for (size_t i = 0; i < inSize; i++) input[i].insert(input[i].begin(), double(placeId));
    }
    else
    {
        input.push_back(vector<double>());
        input[0].push_back(double(placeId));
    }
}

/*
 * ProcessMessage constructor for MAPI topics 
 */
ProcessMessage::ProcessMessage(Mapi* mapi, string input, bool useCru)
{
    this->mapi = mapi;
    this->fullMessage = input;
    this->useCru = useCru;
}

bool ProcessMessage::checkMessage(string& message)
{
    for (size_t i = 0; i < message.size(); i++)
    {
        if (!(isxdigit(message[i])  || message[i] == ',' || message[i] == '\n' || message[i] == 'x' || message[i] == '.'))
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

vector<string> ProcessMessage::generateFullMessage(Instructions::Instruction& instructions)
{
    outputPattern.clear(); //clear outvar name vector
    pollPattern.clear();

    vector<string> fullMessage;

    try
    {
        switch (instructions.type)
        {
            case Instructions::Type::SWT: fullMessage = SWT::generateMessage(instructions, outputPattern, pollPattern, this);
                break;
            case Instructions::Type::SCA: fullMessage = SCA::generateMessage(instructions, outputPattern, pollPattern, this);
                break;
            case Instructions::Type::IC: fullMessage = IC::generateMessage(instructions, outputPattern, pollPattern, this);
                break;
        }
    }
    catch (exception& e)
    {
        throw runtime_error(e.what());
    }

    this->fullMessage.clear();
    for (size_t i = 0; i < fullMessage.size(); i++)
    {
        if (pollPattern[i].empty())
        {
            this->fullMessage += fullMessage[i] + "\n";
        }
    }

    if (!this->fullMessage.empty())
    {
        this->fullMessage.erase(this->fullMessage.size() - 1);
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
        vector<double> received;
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
            Print::PrintError(chainTopic.name, "Updating error service!");
        }
        else groupCommand->receivedResponse(&chainTopic, response, true);
    }
    else
    {
        if (groupCommand == NULL)
        {
            chainTopic.service->Update(response.c_str());
            Print::PrintVerbose(chainTopic.name, "Updating service");
        }
        else groupCommand->receivedResponse(&chainTopic, response, false);        
    }
}

bool ProcessMessage::checkLink(string message, ChainTopic& chainTopic)
{
    if (message == "NO RPC LINK!")
    {
        string response = "ALF_" + (this->getUseCru() ? chainTopic.unit->alfs.first.alfId : chainTopic.unit->alfs.second.alfId) + " is not responding!";
        Print::PrintError(chainTopic.name, response);

        updateResponse(chainTopic, response, true);
        return false;
    }

    return true;
}

void ProcessMessage::evaluateMessage(string message, ChainTopic &chainTopic, bool ignoreStatus)
{
    try
    {
        //if (!ignoreStatus) //ignoreStatus is true only for alfinfo (not the usual alfrpcinfo), so for SUBSCRIBE only
        //{
            string response;
            if (message.find(SUCCESS) != string::npos) //SUCCESS
            {
                vector<vector<unsigned long> > values;
                try
                {
                    Utility::checkMessageIntegrity(this->fullMessage, message.substr(SUCCESS.length() + 1), chainTopic.instruction->type); //check message integrity
                    values = readbackValues(message.substr(SUCCESS.length() + 1), *chainTopic.instruction); //extract eventual outvars
                }
                catch (exception& e)
                {
                    Print::PrintError(chainTopic.name, e.what());
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
                    Print::PrintError(chainTopic.name, response);
                }
                else if (message.find(FAILURE) != string::npos) //FAILURE message
                {
                    Print::PrintError(chainTopic.name, "Error message received:\n" + message + "\n");
                    replace(message.begin(), message.end(), '\n', ';');
                    response = message;
                }
                else //unknown message
                {
                    Print::PrintError(chainTopic.name, "Unknown message received:\n" + message + "\n");
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
        //        Print::PrintError(chainTopic.name, e.what());
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
        Print::PrintError(chainTopic.name, response);
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
            Print::PrintError(chainTopic.name, "Updating MAPI error service!");
            mapi->returnError = false; //reset returnError
        }
        else
        {
            chainTopic.service->Update(response.c_str());
            Print::PrintVerbose(chainTopic.name, "Updating MAPI service");
        }
    }
    else
    {
        Print::PrintVerbose(chainTopic.name, "Mapi is noReturn");
        mapi->noReturn = false; //reset noReturn
    }
}

vector<string>* ProcessMessage::getPollPattern()
{
    return &this->pollPattern;
}

bool ProcessMessage::getUseCru()
{
    return useCru;
}
