#ifndef SWT_H
#define SWT_H

#include <vector>
#include <string>
#include "Fred/Config/instructions.h"

using namespace std;

const int SWT_LOW_WIDTH = 32 / 4; //SWT low hexadecimal width
const int SWT_MIDDLE_WIDTH = 64 / 4; //SWT middle hexadecimal width
const int SWT_HIGH_WIDTH = 76 / 4; //SWT low hexadecimal width

class SWT
{
public:
    static vector<string> generateMessage(Instructions::Instruction& instructions, vector<string>& outputPattern, vector<string>& pollPattern, ProcessMessage* processMessage);
    static void checkIntegrity(const string& request, const string& response);
    static vector<vector<unsigned long> > readbackValues(const string& message, vector<string> outputPattern, Instructions::Instruction& instructions);
    static string valuesToString(vector<vector<unsigned long> > values, int32_t multiplicity);
    static void SWTpad(string& line);
};

#endif // SWT_H
