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
    vector<vector<double> > input;
    vector<string> outputPattern;
    vector<string> pollPattern;
    static const string SUCCESS, FAILURE;
    string fullMessage;
    GroupCommand* groupCommand;
    Mapi* mapi;
    bool useCru;

    bool checkMessage(string& message);

public:
    void parseInputVariables(string& line, vector<string>& inVars, int32_t iteration);

    ProcessMessage(string message, int32_t placeId, bool useCru);
    ProcessMessage(map<string, vector<double> > inVars, int32_t placeId, GroupCommand* groupCommand, bool useCru);
    ProcessMessage(Mapi* mapi, string input, bool useCru);
    int32_t getMultiplicity();
    bool isCorrect();
    vector<vector<unsigned long> > readbackValues(const string &message, Instructions::Instruction& instructions);
    vector<double> calculateReadbackResult(vector<vector<unsigned long> >& result, Instructions::Instruction& instructions);
    bool checkLink(string message, ChainTopic& chainTopic);
    void evaluateMessage(string message, ChainTopic& chainTopic, bool ignoreStatus = false);
    string generateMapiMessage();
    void evaluateMapiMessage(string message, ChainTopic& chainTopic);
    vector<string> generateFullMessage(Instructions::Instruction& instructions);
    string valuesToString(vector<vector<unsigned long> > values, int32_t multiplicity, Instructions::Type type);
    void updateResponse(ChainTopic& chainTopic, string response, bool error);
    vector<string>* getPollPattern();
    bool getUseCru();
};

#endif // PROCESSMESSAGE_H
