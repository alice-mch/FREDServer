#include <cstdio>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <climits>
#include <cmath>
#include "Parser/utility.h"
#include "Parser/calculator.h"
#include "Alfred/print.h"
#include "Parser/processmessage.h"
#include "Fred/Protocols/SCA.h"
#include "Fred/Protocols/SWT.h"
#include "Fred/Protocols/IC.h"

vector<double> Utility::splitString2Num(const string &text, string by)
{
    vector<double> result;

    vector<string> nums = splitString(text, by);

    for (size_t j = 0; j < nums.size(); j++)
    {
        if (nums[j].find('x') != string::npos)
        {
            result.push_back(double(stoi(nums[j].substr(nums[j].find('x') + 1), NULL, 16)));
        }
        else
        {
            result.push_back(stof(nums[j]));
        }
    }

    return result;
}

vector<string> Utility::splitString(const string& text, string by)
{
    vector<string> result;
    string temp = text;
    
    while(temp.size())
    {
        size_t index = temp.find(by);
        if(index != string::npos)
        {
            result.push_back(temp.substr(0, index));
            temp = temp.substr(index + 1);
        }
        else
        {
            result.push_back(temp);
            temp = "";
            break;
        }
    }

    return result;
}

vector<vector<string> > Utility::splitMessage(const string& text)
{
    vector<vector<string> > result;

    vector<string> lines = splitString(text, "\n");

    for (size_t i = 0; i < lines.size(); i++)
    {
        result.push_back(splitString(lines[i], ","));
    }

    return result;
}

vector<vector<double> > Utility::splitMessage2Num(const string &text)
{
    vector<vector<double> > result;
    vector<string> lines = splitString(text, "\n");

    for (size_t i = 0; i < lines.size(); i++)
    {
        result.push_back(splitString2Num(lines[i], ","));
    }

    return result;
}

void Utility::removeWhiteSpaces(string& text)
{
    text.erase(remove_if(text.begin(), text.end(), [](unsigned char c){return isspace(c);}), text.end());
    text.erase(remove_if(text.begin(), text.end(), [](unsigned char c){return iscntrl(c);}), text.end());
}

void Utility::removeComment(string& text)
{
    size_t pos = text.find("#");
    if (pos != string::npos)
    {
        text.erase(pos);
    }
}

void Utility::printVector(const vector<string> &data)
{
    for (int j = 0; j < data.size(); j++) cout << data[j] << "\n";
}

void Utility::printVector(const vector<int32_t> &data)
{
    for (int j = 0; j < data.size(); j++) cout << data[j] << "\n";
}

double Utility::calculateEquation(string& equation, vector<string>& variables, vector<double>& values)
{
    map<string, int> varMap;

    for (size_t i = 0; i < values.size(); i++)
    {
        if (trunc(values[i]) == values[i])
        {
            varMap[variables[i]] = int(values[i]);
        }
        else
        {
            size_t pos = -1;
            while ((pos = equation.find(variables[i], pos + 1) != string::npos))
            {
                equation.insert(pos + variables[i].size(), "/1000");
            }
            varMap[variables[i]] = int(values[i] * 1000);
        }
    }

    try
    {
        return (calculator::eval(equation, varMap));
    }
    catch (const calculator::error& err)
    {
        Print::PrintError("Cannot parse equation!");
    }

    return 0;
}

string Utility::readbackToString(vector<double> data)
{
    string result;

    for (size_t v = 0; v < data.size(); v++)
    {
        result += to_string(data[v]);
        if (v < data.size() - 1) result += "\n";
    }

    return result;
}

vector<unsigned long> Utility::splitAlfResponse(const string& message, Instructions::Type type)
{
    vector<string> splitted = splitString(message, "\n");
    vector<unsigned long> result;

    for (size_t i = 0; i < splitted.size(); i++)
    {
        if (type == Instructions::Type::SCA)
        {
            size_t pos = splitted[i].find(",");
            if (pos != string::npos)
            {
                result.push_back(stoul(splitted[i].substr(pos + 1), NULL, 16)); //return the 32 bits payload
            }
        }
        else if (type == Instructions::Type::SWT)
        {
            result.push_back(stoul(splitted[i].size() > 4 ? splitted[i].substr(splitted[i].size() - 4) : splitted[i], NULL, 16)); //return last 16 bits
        }
        else if (type == Instructions::Type::IC)
        {
            size_t pos = splitted[i].find(",");
            if (pos != string::npos)
            {
                result.push_back(stoul(splitted[i].substr(pos + 1), NULL, 16)); //return the 32 bits payload
            }
        }
    }

    return result;
}

void Utility::checkMessageIntegrity(const string& request, const string& response, Instructions::Type type)
{
    try
    {
        switch (type)
        {
            case Instructions::Type::SWT: SWT::checkIntegrity(request, response);
                break;
            case Instructions::Type::SCA: SCA::checkIntegrity(request, response);
                break;
            case Instructions::Type::IC: IC::checkIntegrity(request, response);
                break;
        }
    }
    catch (exception& e)
    {
        throw runtime_error(e.what());
    }
}

Utility::Utility()
{

}
