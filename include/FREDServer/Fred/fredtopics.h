#ifndef FREDTOPICS_H
#define FREDTOPICS_H

#include <string>
#include <map>
#include <utility>
#include "Fred/mappedcommand.h"
#include "Fred/subscribecommand.h"
#include "Fred/groupcommand.h"
#include "Fred/Config/mapping.h"
#include "Fred/Config/instructions.h"
#include "Fred/Config/groups.h"
#include "Alfred/service.h"
#include "Fred/Mapi/mapi.h"
#include "Fred/Mapi/iterativemapi.h"
#include "Fred/Mapi/mapigroup.h"

class Fred;
class AlfRpcInfo;
class AlfInfo;
class Queue;

struct ChainTopic
{
    string name;

    CommandString* command;
    ServiceString* service;
    ServiceString* error;

    pair<AlfRpcInfo*, AlfRpcInfo*> alfLink;
    pair<Queue*, Queue*> alfQueue;

    Instructions::Instruction* instruction;
    Mapping::Unit* unit;
    int32_t placeId;

    Mapi* mapi;
};

struct GroupTopic
{
    string name;

    GroupCommand* command;
    ServiceString* service;
    ServiceString* error;

    Instructions::Instruction* instruction;
    vector<ChainTopic*> chainTopics;

    map<string, vector<double> > inVars;
    vector<int32_t> unitIds;
};

class FredTopics
{
public:
    FredTopics(Fred* fred);
    void registerUnit(string section, Mapping::Unit& unit, Instructions& instructions);
    void registerGroup(string section, Groups::Group& group);
    void registerMapiObject(string topic, Mapi* mapi);
    map<string, ChainTopic> &getTopicsMap();

private:
    Fred* fred;
    map<string, ChainTopic> topics;
    map<string, GroupTopic> groupTopics;
};

#endif // FREDTOPICS_H
