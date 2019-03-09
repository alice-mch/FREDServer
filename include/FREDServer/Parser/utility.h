#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <string>
#include "Fred/Config/instructions.h"

using namespace std;

class Utility
{
public:
    static vector<uint32_t> splitString2Num(const string& text, string by);
    static vector<string> splitString(const string& text, string by);
    static vector<vector<string> > splitMessage(const string& text);
    static vector<vector<uint32_t> > splitMessage2Num(const string& text);
    static void removeWhiteSpaces(string& text);
    static void removeComment(string& text);
    static void printVector(const vector<string>& data);
    static void printVector(const vector<int32_t>& data);
    static double calculateEquation(string& equation, vector<string>& variables, vector<uint32_t>& values);
    static string readbackToString(vector<vector<uint32_t> > data, int32_t multiplicity);
    static string readbackToString(vector<double> data);
    static vector<uint32_t> splitAlfResponse(const string &message, Instructions::Type type);
    static bool checkMessageIntegrity(const string& request, const string& response, Instructions::Type type);

private:
    Utility();
};

#endif // UTILITY_H
