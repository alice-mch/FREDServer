#ifndef FREDTOPICS_H
#define FREDTOPICS_H

#include <string>
#include <map>
#include "Fred/mappedcommand.h"
#include "Fred/subscribecommand.h"
#include "Fred/groupcommand.h"
#include "Fred/Config/mapping.h"
#include "Fred/Config/instructions.h"
#include "Fred/Config/groups.h"
#include "Alfred/service.h"
#include "Fred/mapiinterface.h"

class Fred;
class AlfRpcInfo;
class AlfInfo;
class Queue;

struct ChainTopic
{
    string name;
    CommandString* command;
    ServiceString* service;
    Instructions::Instruction* instruction;
    AlfRpcInfo* alfLink;
    Queue* alfQueue;
    Mapping::Unit* unit;
    string alfDns;
    int32_t placeId;

    AlfInfo* alfInfo;
    float interval;

    MapiInterface* mapi;
};

struct GroupTopic
{
    string name;
    GroupCommand* command;
    ServiceString* service;
    Instructions::Instruction* instruction;
    vector<ChainTopic*> chainTopics;
    map<string, vector<uint32_t> > inVars;
    vector<int32_t> unitIds;
};

class FredTopics
{
public:
    FredTopics(Fred* fred);
    void registerUnit(string section, Mapping::Unit& unit, Instructions& instructions);
    void registerGroup(string section, Groups::Group& group);
    void registerMapiObject(string topic, MapiInterface* mapi);

private:
    Fred* fred;
    map<string, ChainTopic> topics;
    map<string, GroupTopic> groupTopics;
};

#endif // FREDTOPICS_H
