#include "Parser/parser.h"
#include "Alfred/print.h"
#include "Parser/utility.h"
#include "Fred/Config/location.h"
#include "Fred/Config/instructions.h"
#include "Fred/Config/mapping.h"
#include "Fred/Config/groups.h"
#include <dirent.h>
#include <fstream>
#include <algorithm>

Parser::Parser(string sectionsPath)
{
    this->sectionsPath = sectionsPath;
    this->badFiles = false;
}

vector<string> Parser::findFiles(string directory)
{
    DIR* dir = opendir(directory.c_str());
    if (!dir)
    {
        Print::PrintError("Cannot open directory " + directory + "!");
        return vector<string>();
    }

    vector<string> files;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] != '.' && strstr(entry->d_name, ".section") != NULL)
        {
            files.push_back(entry->d_name);
        }
    }

    closedir(dir);

    return files;
}

vector<Section> Parser::parseSections()
{
    vector<Section> sections;

    vector<string> files = findFiles(this->sectionsPath);
    for (size_t i = 0; i < files.size(); i++)
    {
        vector<string> lines = readFile(files[i], this->sectionsPath);

        if(!lines.empty())
        {
            string name;
            vector<string> rest;
            vector<string> subsection = getSubsection(lines, "{}", name, rest); //without outer wrapper

            Section section(name);

            rest = subsection;

            vector<string> instructionsLines, mappingLines, groupsLines, maskingLines;

            while (rest.size()) //rest is shrinking each loop
            {
                vector<string> temp;
                subsection = getSubsection(rest, "{}", name, temp);
                rest = temp;

                if (name == "INSTRUCTIONS")
                {
                    instructionsLines = subsection;
                }
                else if (name == "MAPPING")
                {
                    mappingLines = subsection;
                }
                else if (name == "GROUPS")
                {
                    groupsLines = subsection;
                }
                else if (name == "MASK")
                {
                    maskingLines = subsection;
                }
                else
                {
                    Print::PrintError(files[i] + " has invalid name of paragraph: " + name + "!");
                    this->badFiles = true;
                }
            }

            if(!instructionsLines.size()) //section INSTRUCTIONS is mandatory
            {
                Print::PrintError("INSTRUCTIONS section in " + files[i] + " is missing!");
                this->badFiles = true;

            }
            if (!mappingLines.size()) //section MAPPING is mandatory
            {
                Print::PrintError("MAPPING section in " + files[i] + " is missing!");
                this->badFiles = true;
            }

            if(!this->badFiles)
            {
                try
                {
                    section.instructions = Instructions(instructionsLines, this->sectionsPath);
                    section.mapping = Mapping(mappingLines);
                    section.groups = Groups(groupsLines);

                    // next line will segfault if Mapping section is bad
                    //  e.g. wasn't processed due to bad section name
                    section.groups.calculateIds(section.mapping, maskingLines);

                    checkGroup(section); //check if group topics are existing
                }
                catch (exception& e)
                {
                    this->badFiles = true;
                } 
            }
            sections.push_back(section);  
        }
        else
        {
            badFiles = true;
        }
    }
    return sections;
}

void Parser::checkGroup(Section section)
{
    vector<string> topics = section.instructions.getTopics();
    vector<Groups::Group> groups = section.groups.getGroups();
    vector<Mapping::Unit> units = section.mapping.getUnits();

    for (auto i = groups.begin(); i != groups.end(); i++)
    {   
        if (!(find(topics.begin(), topics.end(), i->topicName) != topics.end()))
        {
            Print::PrintError("Topic " + i->topicName + " from group " + i->name + " in section "
                + section.getName() + " is not an existing topic!");
            throw runtime_error("Non existing group topic");
        }

        if(i->unitIds.size() == 0)
        {
            Print::PrintError("Group " + i->name + " in section " + section.getName() + " has no units!");
            throw runtime_error("Group with no units");
        }

        for (size_t j = 0; j < i->unitIds.size(); j++)
        {
            bool found = false;
            for (size_t k = 0; k < units.size(); k++)
            {    
                vector<int>::iterator it = find(units[k].unitIds.begin(), units[k].unitIds.end(), i->unitIds[j]); 
                if (it != units[k].unitIds.end()) 
                { 
                    found = true;
                }
            }

            if (!found)
            {
                Print::PrintError("Unit " + to_string(i->unitIds[j]) + " from group " + i->name + " in section "
                    + section.getName() + " is not an existing unit!");
                throw runtime_error("Non existing unit");
            }
        }
    }
}

vector<string> Parser::readFile(string fileName, string directory)
{
    if (directory[directory.size() - 1] == '/') directory.erase(directory.size() - 1);

    ifstream inFile(directory + "/" + fileName);
    if (!inFile.is_open())
    {
        Print::PrintError("Cannot open file " + fileName + " in directory '" + directory + "'!");
    }

    vector<string> lines;

    string line;
    while (getline(inFile, line))
    {
        Utility::removeWhiteSpaces(line);
        Utility::removeComment(line);
        if (line.length() > 0)
        {
            lines.push_back(line);
        }
    }

    inFile.close();

    if (!balancedBraces(lines, fileName))
    {
        vector<string> empty;
        return empty;
    }
    else
    {
        return lines;
    }
}

vector<string> Parser::getSubsection(vector<string> full, string bracets, string& name, vector<string>& rest)
{
    try
    {
        size_t firstLine = -1, lastLine = -1, counter = -1;

        for (size_t i = 0; i < full.size(); i++)
        {
            if (firstLine == -1)
            {
                if (full[i].find(bracets[0]) != string::npos)
                {
                    firstLine = i;
                    counter = 1;
                    name = full[i].substr(0, full[i].find("="));
                }
            }
            else
            {
                if (full[i].find(bracets[0]) != string::npos) counter++;
                else if (full[i].find(bracets[1]) != string::npos) counter--;

                if (counter == 0)
                {
                    lastLine = i;
                    break;
                }
            }
        }

        vector<string> subsection(full.begin() + firstLine + 1, full.begin() + lastLine);

        if (full.begin() < full.begin() + firstLine)
        {
            vector<string> before(full.begin(), full.begin() + firstLine);
            rest.insert(rest.end(), before.begin(), before.end());
        }
        if (full.begin() + lastLine + 1 < full.end())
        {
            vector<string> after(full.begin() + lastLine + 1, full.end());
            rest.insert(rest.end(), after.begin(), after.end());
        }
        return subsection;
    }
    catch (exception& error) 
    {
        Print::PrintError("Exception in getSubsection while parsing " + name);
        return vector<string>();
    }
}

bool Parser::balancedBraces(vector<string> lines, string name)
{
    int c = 0; 
  
    for (int i = 0; i < lines.size(); i++) 
    {
        for(int j = 0; j < lines[i].length(); j++)
        {     
            if (lines[i][j]=='('||lines[i][j]=='['||lines[i][j]=='{') c++;
            
            if (lines[i][j]==')'||lines[i][j]==']'||lines[i][j]=='}') c--;
        }
    }

    if(c != 0)
    {
        Print::PrintError(name + " has mismatched braces!");
    }

    return c == 0 ? 1 : 0;
}
