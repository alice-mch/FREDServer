#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Fred/section.h"

using namespace std;

class Parser
{
private:
    string sectionsPath, boardsPath;

    vector<string> findFiles(string directory);

public:
	bool badFiles;
    Parser(string sectionsPath);
    vector<Section> parseSections();
    static vector<string> getSubsection(vector<string> full, string bracets, string& name, vector<string>& rest);
    static vector<string> readFile(string fileName, string directory);
    static bool balancedBraces(vector<string> lines, string name);
    static void checkGroup(Section section);
};

#endif // PARSER_H
