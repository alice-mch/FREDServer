#include "Parser/parser.h"
#include "Alfred/print.h"
#include "Parser/utility.h"
#include "Fred/Config/location.h"
#include "Fred/Config/instructions.h"
#include "Fred/Config/mapping.h"
#include "Fred/Config/groups.h"
#include <dirent.h>
#include <fstream>
#include <iostream>

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
        PrintError("Cannot open directory " + directory + "!");
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
    Section emptySection("empty");
    vector<Section> emptySections;

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
            for (size_t j = 0; j < 4; j++) //section must have 4 parts
            {
                vector<string> temp;
                subsection = getSubsection(rest, "{}", name, temp);
                rest = temp;
  
                if (name == "ALFS")
                {
                    try
                    {
                        section.location = Location(subsection);
                    }
                    catch (exception& e)
                    {
                        this->badFiles = true;
                    } 
                }
                else if (name == "INSTRUCTIONS")
                {
                    try
                    {
                        section.instructions = Instructions(subsection, this->sectionsPath);
                    }
                    catch (exception& e)
                    {
                        this->badFiles = true;
                    } 
                }
                else if (name == "MAPPING")
                {
                    try
                    {
                        section.mapping = Mapping(subsection);
                    }
                    catch (exception& e)
                    {
                        this->badFiles = true;
                    } 
                }
                else if (name == "GROUPS")
                {
                    try
                    {
                        section.groups = Groups(subsection);
                        // next line will segfault if Mapping section is bad
                        //  e.g. wasn't processed due to bad section name
                        section.groups.calculateIds(section.mapping);
                    }
                    catch (exception& e)
                    {
                        this->badFiles = true;
                    } 
                }
                else
                {
                    PrintError(files[i] + " has invalid name of paragraph: " + name + "!");
                    this->badFiles = true;
                    //section = emptySection;
                }
            } // for
            sections.push_back(section);  
        }
        else{
            badFiles = true;
        }
    }
    return sections;
}

vector<string> Parser::readFile(string fileName, string directory)
{
    if (directory[directory.size() - 1] == '/') directory.erase(directory.size() - 1);

    ifstream inFile(directory + "/" + fileName);
    if (!inFile.is_open())
    {
        PrintError("Cannot open file " + fileName + " in directory '" + directory + "'!");
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
        PrintError("Exception in getSubsection while parsing " + name);
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
        PrintError(name + " has mismatched braces!");
    }

    return c == 0 ? 1 : 0;
}
