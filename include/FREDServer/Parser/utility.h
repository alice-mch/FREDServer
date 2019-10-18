#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <string>
#include "Fred/Config/instructions.h"

using namespace std;

class Utility
{
public:
    static vector<double> splitString2Num(const string& text, string by);
    static vector<string> splitString(const string& text, string by);
    static vector<vector<string> > splitMessage(const string& text);
    static vector<vector<double> > splitMessage2Num(const string& text);
    static void removeWhiteSpaces(string& text);
    static void removeComment(string& text);
    static void printVector(const vector<string>& data);
    static void printVector(const vector<int32_t>& data);
    static double calculateEquation(string& equation, vector<string>& variables, vector<double>& values);
    static string readbackToString(vector<double> data);
    static vector<unsigned long> splitAlfResponse(const string &message, Instructions::Type type);
    static void checkMessageIntegrity(const string& request, const string& response, Instructions::Type type);

private:
    Utility();
};

#endif // UTILITY_H
