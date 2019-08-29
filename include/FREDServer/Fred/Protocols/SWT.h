#ifndef SWT_H
#define SWT_H

#include <vector>
#include <string>
#include "Fred/Config/instructions.h"

using namespace std;

const int SWT_WIDTH = 76 / 4; //SWT hexadecimal characters width

class SWT
{
private:

public:
    static string generateMessage(Instructions::Instruction& instructions, vector<string>& outputPattern, ProcessMessage* processMessage);
    static void checkIntegrity(const string& request, const string& response);
    static vector<vector<unsigned long> > readbackValues(const string& message, vector<string> outputPattern, Instructions::Instruction& instructions);
    static string valuesToString(vector<vector<unsigned long> > values, int32_t multiplicity);
};

#endif // SWT_H
