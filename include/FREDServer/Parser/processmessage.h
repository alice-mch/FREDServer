#ifndef PROCESSMESSAGE_H
#define PROCESSMESSAGE_H

#include <string>
#include <vector>
#include "Fred/Config/instructions.h"
#include "Fred/Mapi/mapi.h"
#include "Fred/Mapi/iterativemapi.h"
#include "Fred/Mapi/mapigroup.h"

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

public:
    void parseInputVariables(string& line, vector<string>& inVars, int32_t iteration);

    ProcessMessage(string message, int32_t placeId);
    ProcessMessage(map<string, vector<uint32_t> > inVars, int32_t placeId, GroupCommand* groupCommand);
    ProcessMessage(Mapi* mapi, string input);
    int32_t getMultiplicity();
    bool isCorrect();
    vector<vector<unsigned long> > readbackValues(const string &message, Instructions::Instruction& instructions);
    vector<double> calculateReadbackResult(vector<vector<unsigned long> >& result, Instructions::Instruction& instructions);
    void evaluateMessage(string message, ChainTopic& chainTopic, bool ignoreStatus = false);
    string generateMapiMessage();
    void evaluateMapiMessage(string message, ChainTopic& chainTopic);
    string generateFullMessage(Instructions::Instruction& instructions);
    string valuesToString(vector<vector<unsigned long> > values, int32_t multiplicity, Instructions::Type type);
};

#endif // PROCESSMESSAGE_H
