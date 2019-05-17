#include <cstdio>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "Parser/utility.h"
#include "Parser/calculator.h"
#include "Alfred/print.h"

vector<uint32_t> Utility::splitString2Num(const string &text, string by)
{
    vector<uint32_t> result;

    vector<string> nums = splitString(text, by);

    for (size_t j = 0; j < nums.size(); j++)
    {
        if (nums[j].find('x') != string::npos)
        {
            result.push_back(stoi(nums[j].substr(nums[j].find('x') + 1), NULL, 16));
        }
        else
        {
            result.push_back(stoi(nums[j], NULL, 10));
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
            /*if(temp.size() == 0)
            {
                result.push_back(temp);
            }*/
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

vector<vector<uint32_t> > Utility::splitMessage2Num(const string &text)
{
    vector<vector<uint32_t> > result;
    vector<string> lines = splitString(text, "\n");

    for (size_t i = 0; i < lines.size(); i++)
    {
        result.push_back(splitString2Num(lines[i], ","));
    }

    return result;
}

void Utility::removeWhiteSpaces(string& text)
{
    text.erase(remove(text.begin(), text.end(), ' '), text.end());
    text.erase(remove(text.begin(), text.end(), '\t'), text.end());
    text.erase(remove(text.begin(), text.end(), '\n'), text.end());
    text.erase(remove(text.begin(), text.end(), '\r'), text.end());
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

double Utility::calculateEquation(string& equation, vector<string>& variables, vector<uint32_t>& values)
{
    map<string, int> varMap;

    for (size_t i = 0; i < values.size(); i++)
    {
        varMap[variables[i]] = (int)(values[i]);
    }

    try
    {
        return (calculator::eval(equation, varMap));
    }
    catch (const calculator::error& err)
    {
        PrintError("Cannot parse equation!");
    }

    return 0;
}

string Utility::readbackToString(vector<vector<uint32_t> > data, int32_t multiplicity)
{
    stringstream result;

    for (int32_t m = 0; m < multiplicity; m++)
    {
        for (size_t v = 0; v < data.size(); v++)
        {
            result << "0x" << hex << data[v][m];
            if (v < data.size() - 1) result << ",";
        }

        if (m < multiplicity - 1) result << "\n";
    }

    return result.str();
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

vector<uint32_t> Utility::splitAlfResponse(const string& message, Instructions::Type type)
{
    vector<string> splitted = splitString(message, "\n");
    vector<uint32_t> result;

    for (size_t i = 0; i < splitted.size(); i++)
    {
        if (type == Instructions::Type::SCA)
        {
            size_t pos = splitted[i].find(",");
            if (pos != string::npos)
            {
                result.push_back(stoul(splitted[i].substr(pos + 1), NULL, 16));
            }
        }
        else if (type == Instructions::Type::SWT)
        {
            result.push_back(stoul(splitted[i].size() > 4 ? splitted[i].substr(splitted[i].size() - 4) : splitted[i], NULL, 16));
        }
    }

    return result;
}

bool Utility::checkMessageIntegrity(const string& request, const string& response, Instructions::Type type)
{
    vector<string> reqVec = splitString(request, "\n");
    vector<string> resVec = splitString(response, "\n");

    if (reqVec.size() != resVec.size())
    {
        PrintError("Invalid number of lines received!");
        return false;
    }

    try
    {
        for (size_t i = 0; i < reqVec.size(); i++)
        {
            transform(reqVec[i].begin(), reqVec[i].end(), reqVec[i].begin(), ::tolower);
            transform(resVec[i].begin(), resVec[i].end(), resVec[i].begin(), ::tolower);

            if (type == Instructions::Type::SCA)
            {
                size_t nonZeroPos = reqVec[i].find_first_not_of('0');
                size_t commaPos = reqVec[i].find(",");
                if (reqVec[i].substr(nonZeroPos, commaPos-nonZeroPos) != resVec[i].substr(0, resVec[i].find(","))) return false;
            }
            else if (type == Instructions::Type::SWT)
            {
                if (resVec[i] == "0") continue;
                if (reqVec[i].substr(reqVec[i].size() - 8, 4) != resVec[i].substr(resVec[i].size() - 8, 4)) return false;
            }
        }
    }
    catch (exception& e)
    {
        PrintError("Integrity check of received message failed!");
        return false;
    }

    return true;
}

Utility::Utility()
{

}
