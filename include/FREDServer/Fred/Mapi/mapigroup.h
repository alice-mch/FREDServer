#ifndef MAPIGROUP_H
#define MAPIGROUP_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

class Fred;

using namespace std;

class Mapigroup: public Mapi
{
public:
    void newMapiGroupRequest(vector<pair<string, string> > requests);
    void newTopicGroupRequest(vector<pair<string, string> > requests);

    void publishAnswer(string message);
    void publishError(string error);
};

#endif // MAPIGROUP_H
