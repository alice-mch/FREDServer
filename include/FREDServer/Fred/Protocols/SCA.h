#ifndef SCA_H
#define SCA_H

#include <vector>
#include <string>
#include "Fred/Config/instructions.h"

using namespace std;

class SCA //: public Protocol
{
public:
    static vector<string> generateMessage(Instructions::Instruction& instructions, vector<string>& outputPattern, vector<string>& pollPattern, ProcessMessage* processMessage);
    static void checkIntegrity(const string& request, const string& response);
    static vector<vector<unsigned long> > readbackValues(const string& message, vector<string> outputPattern, Instructions::Instruction& instructions);
    static string valuesToString(vector<vector<unsigned long> > values, int32_t multiplicity);
    static void SCApad(string& line);
};

#endif // SCA_H
