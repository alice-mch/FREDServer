#include "Fred/Config/instructions.h"
#include "Parser/parser.h"
#include "Parser/utility.h"
#include "Alfred/print.h"

Instructions::Instructions(vector<string> data, string currentPath)
{
    if (currentPath[currentPath.size() - 1] == '/') currentPath.erase(currentPath.size() - 1);

    size_t idx = 0;
    while (idx < data.size())
    {
        if (data[idx].find("PATH") != string::npos)
        {
            processConfigFile(currentPath + "/" + data[idx].substr(data[idx].find("=") + 1));
        }
        else if (data[idx].find("TOPICS") != string::npos)
        {
            do
            {
                idx++;

                string topic = data[idx];
                if (topic[topic.size() - 1] == ',') topic.erase(topic.size() - 1);
                topics.push_back(topic);
            }
            while (data[idx + 1].find("]") == string::npos);
        }

        idx++;
    }
}

void Instructions::processConfigFile(string file)
{
    this->path = file;

    vector<string> lines = Parser::readFile(this->path, ".");
    if (!lines.empty())
    {
        vector<string> rest = lines;
        while (!rest.empty())
        {
            string name;
            vector<string> temp;
            vector<string> subsection = Parser::getSubsection(rest, "{}", name, temp);
            rest = temp;

            Instruction instruction;
            instruction.subscribe = false;
            instruction.name = name;

            for (size_t i = 0; i < subsection.size(); i++)
            {
                string left = subsection[i].substr(0, subsection[i].find("="));
                string right = subsection[i].substr(subsection[i].find("=") + 1);

                if (left == "TYPE")
                {
                    if (right == "SWT")
                    {
                        instruction.type = Type::SWT;
                    }
                    else if (right == "SCA")
                    {
                        instruction.type = Type::SCA;
                    }
                    else
                    {
                        PrintError(this->path + " has invalid type name " + right + " in topic " + name);
                        throw runtime_error("TYPE");
                    }
                }
                else if (left == "SUBSCRIBE")
                {
                    instruction.subscribe = right == "TRUE" ? true : false;
                }
                else if (left == "EQUATION")
                {
                    instruction.equation = right;
                }
                else if (left == "IN_VAR")
                {
                    instruction.inVar = Utility::splitString(right, ",");
                }
                else if (left == "OUT_VAR")
                {
                    instruction.outVar = Utility::splitString(right, ",");
                }
                else if (left == "MESSAGE")
                {
                    instruction.message.push_back(right);
                }
                else if (left == "FILE")
                {
                    instruction.message = processSequenceFile(this->path.substr(0, this->path.find_last_of("/")) + "/" + right);
                    if (instruction.message.empty())
                    {
                        PrintError("Sequence File in " + name + " topic doesn't exist or is empty");
                        throw runtime_error("Sequence File doesn't exist or is empty");
                    }
                }
                else
                {
                    PrintError(this->path + " has invalid instruction name " + left + " in topic " + name);
                    throw runtime_error("invalid instruction name");
                }
            }

            instruction.inVar.insert(instruction.inVar.begin(), "_ID_");    //number determined by location of element in the mapping
            instructions[instruction.name] = instruction;
        }
    }
    else{
        throw runtime_error("instructions");
    }
}

vector<string> Instructions::processSequenceFile(string file)
{
    return Parser::readFile(file, ".");
}

map<string, Instructions::Instruction>& Instructions::getInstructions()
{
    return instructions;
}

vector<string>& Instructions::getTopics()
{
    return topics;
}
