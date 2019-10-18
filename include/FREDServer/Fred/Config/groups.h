#ifndef GROUPS_H
#define GROUPS_H

#include <string>
#include <vector>
#include <map>
#include "Fred/Config/mapping.h"

using namespace std;

class Groups
{
public:

    struct Group
    {
        string name;
        string unitName, topicName;
        string range;
        vector<int32_t> unitIds;
        map<string, vector<double> > inVars;
    };

    Groups(vector<string> data = vector<string>());
    void calculateIds(Mapping& mapping, vector<string> masking);
    vector<Group>& getGroups();

private:
    vector<Group> groups;

    void processGroup(string& left, string& right, vector<string>& inVars);
    vector<double> processChannels(string line);

    void fillInVars(Group& group);
};

#endif // GROUPS_H
