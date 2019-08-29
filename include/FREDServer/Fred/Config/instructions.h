#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class Instructions
{
public:
    Instructions(vector<string> data = vector<string>(), string currentPath = ".");

    enum Type
    {
        SCA,
        SWT,
        IC
    };

    struct Instruction
    {
        string name;
        Type type;
        string equation;
        vector<string> inVar, outVar, message;
        bool subscribe;
    };

    map<string, Instruction>& getInstructions();
    vector<string>& getTopics();

private:
    string path;
    vector<string> topics;
    map<string, Instruction> instructions;

    vector<string> processConfigFile(string file);
    vector<string> processSequenceFile(string file);
};

#endif // INSTRUCTIONS_H
