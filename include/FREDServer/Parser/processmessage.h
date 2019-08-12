#ifndef PROCESSMESSAGE_H
#define PROCESSMESSAGE_H

#include <string>
#include <vector>
#include "Fred/Config/instructions.h"
#include "Fred/Mapi/mapi.h"

using namespace std;

class ChainTopic;
class GroupCommand;

class ProcessMessage
{
private:
    bool correct;
    vector<vector<uint32_t> > input;
    vector<string> outputPattern;
    static const string SUCCESS, FAILURE;
    string fullMessage;
    GroupCommand* groupCommand;
    Mapi* mapi;

    bool checkMessage(string& message);
    void checkForOutput(string& line);
    void parseInputVariables(string& line, vector<string>& inVars, int32_t iteration);

public:
    ProcessMessage(string message, int32_t placeId);
    ProcessMessage(map<string, vector<uint32_t> > inVars, int32_t placeId, GroupCommand* groupCommand);
    ProcessMessage(Mapi* mapi, string input);
    int32_t getMultiplicity();
    bool isCorrect();

    string &generateFullMessage(Instructions::Instruction& instructions);

    vector<vector<uint32_t> > readbackValues(const string &message, Instructions::Instruction& instructions);
    vector<double> calculateReadbackResult(vector<vector<uint32_t> >& result, Instructions::Instruction& instructions);
    void evaluateMessage(string message, ChainTopic& chainTopic, bool ignoreStatus = false);

    string generateMapiMessage();
    void evaluateMapiMessage(string message, ChainTopic& chainTopic);
};

#endif // PROCESSMESSAGE_H
