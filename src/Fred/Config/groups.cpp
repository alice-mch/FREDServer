#include <algorithm>
#include "Fred/Config/groups.h"
#include "Alfred/print.h"
#include "Parser/utility.h"

Groups::Groups(vector<string> data)
{
    for (size_t i = 0; i < data.size(); i++)
    {
        if (data[i].find("=") != string::npos)
        {
            string left = data[i].substr(0, data[i].find("="));
            string right = data[i].substr(data[i].find("=") + 1);

            vector<string> inVars;
            if (i + 1 < data.size() - 1 && data[i + 1].find("{") != string::npos)
            {
                i+=2;
                while (data[i].find("}") == string::npos)
                {
                    inVars.push_back(data[i]);
                    i++;
                }
            }
            processGroup(left, right, inVars);
        }
    }
}

void Groups::processGroup(string& left, string& right, vector<string> &inVars)
{
    Group group;

    group.name = left;

    string unitName = right.substr(0, right.find("["));
    string topicName = right.substr(right.find("]/") + 2);
    string inside = right.substr(right.find("[") + 1, right.find("]") - right.find("[") - 1);

    group.unitName = unitName;
    group.topicName = topicName;
    group.range = inside;

    for (size_t i = 0; i < inVars.size(); i++)
    {
        string left = inVars[i].substr(0, inVars[i].find("="));
        string right = inVars[i].substr(inVars[i].find("=") + 1);
        group.inVars[left] = processChannels(right);
    }

    fillInVars(group);

    groups.push_back(group);
}

void Groups::calculateIds(Mapping& mapping, vector<string> masking)
{
    vector<int32_t> mask, allIds;
    string fed;

    if (masking.size())
    {
        string maskString = masking[0];
        fed = maskString.substr(0, maskString.find("["));
        maskString = maskString.substr(maskString.find("[") + 1, maskString.find("]") - maskString.find("[") - 1);
        Utility::removeWhiteSpaces(maskString);
        vector<string> textMask = Utility::splitString(maskString, ",");

        for (size_t i = 0; i < textMask.size(); i++)
        {
            mask.push_back(stoi(textMask[i]));
        }
    }

    for (size_t g = 0; g < groups.size(); g++)
    {
        vector<int32_t> all;
        for (size_t i = 0; i < mapping.getUnits().size(); i++)
        {
            if (mapping.getUnits()[i].unitName == groups[g].unitName)
            {
                all.insert(all.end(), mapping.getUnits()[i].unitIds.begin(), mapping.getUnits()[i].unitIds.end());
            }
            else
            {
                Print::PrintError(groups[g].unitName + " is not an existing FED!");
                throw runtime_error("Unexisting FED");
            }
        }

        all.erase(remove(all.begin(), all.end(), -1), all.end());
        sort(all.begin(), all.end());

        string range = groups[g].range;
        
        size_t pos;
        while ((pos = range.find("..")) != string::npos)
        {
            size_t first = 0, last = all.size() - 1;
            if (pos > 0)
            {
                size_t begin = pos;
                do { begin--; } while (begin > 0 && range[begin] != ',');
                if (begin > 0) begin++;
                first = distance(all.begin(), find(all.begin(), all.end(), stoi(range.substr(begin, pos - begin))));
                range.replace(begin, pos - begin, pos - begin, ' ');
            }
            if (pos + 2 < range.size())
            {
                size_t end = pos + 2;
                do { end++; } while (end < range.size() && range[end] != ',');
                if (end < range.size()) end--;
                last = distance(all.begin(), find(all.begin(), all.end(), stoi(range.substr(pos + 2, end - pos - 2))));
                range.replace(pos + 2, end - pos - 2, end - pos - 2, ' ');
            }

            string replacement;
            for (size_t i = first; i <= last; i++)
            {
                replacement += to_string(all[i]);
                if (i < last) replacement += ',';
            }

            range.replace(pos, 2, replacement);
            Utility::removeWhiteSpaces(range);
        }

        vector<string> textIds = Utility::splitString(range, ",");
        vector<int32_t> ids;

        for (size_t i = 0; i < textIds.size(); i++)
        {
            ids.push_back(stoi(textIds[i]));
            allIds.push_back(stoi(textIds[i]));
        }

        for (auto it = ids.begin(); it != ids.end(); it++)
        {
            if (find(mask.begin(), mask.end(), *it) != mask.end())
            {
                ids.erase(it--);
            }
        }
        
        groups[g].unitIds = ids;         
    }

    for (auto it = mask.begin(); it != mask.end(); it++)
    {
        Print::PrintWarning(fed + "[" + to_string(*it) + "] is masked!");

        if (find(allIds.begin(), allIds.end(), *it) != allIds.end())
        {
            mask.erase(it--);
        }
    }

    for (size_t i = 0; i < mask.size(); i++)
    {
        Print::PrintWarning(fed + " " + to_string(mask[i]) + " is masked but it is not in a group!");
    }
}

vector<double> Groups::processChannels(string line)
{
    line = line.substr(line.find("[") + 1, line.find("]") - line.find("[") - 1);

    size_t pos;
    while ((pos = line.find("..")) != string::npos)
    {
        size_t leftPos = pos - 1;
        while (leftPos - 1 >= 0 && isdigit(line[leftPos - 1])) leftPos--;
        uint32_t leftNum = stoul(line.substr(leftPos, pos - leftPos));

        size_t rightPos = pos + 2;
        while (rightPos + 1 < line.size() && isdigit(line[rightPos + 1])) rightPos++;
        uint32_t rightNum = stoul(line.substr(pos + 2, rightPos - pos - 1));

        string replace;
        for (uint32_t i = leftNum + 1; i <= rightNum - 1; i++)
        {
            replace += "," + to_string(i);
            if (i == rightNum - 1) replace += ",";
        }

        line.replace(pos, 2, replace);
    }

    return Utility::splitString2Num(line, ",");
}

vector<Groups::Group>& Groups::getGroups()
{
    return groups;
}

void Groups::fillInVars(Group& group)
{
    size_t max = 0;

    for (auto it = group.inVars.begin(); it != group.inVars.end(); it++)
    {
        if (it->second.size() > max) max = it->second.size();
    }

    for (auto it = group.inVars.begin(); it != group.inVars.end(); it++)
    {
        if (it->second.size() < max)
        {
            for (size_t i = it->second.size(); i < max; i++) it->second.push_back(it->second[it->second.size() - 1]);
        }
    }
}
