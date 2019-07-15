#include "Fred/fredtopics.h"
#include "Fred/fred.h"
#include "Fred/queue.h"
#include "Fred/alfinfo.h"
#include "Alfred/print.h"
#include <exception>

FredTopics::FredTopics(Fred* fred)
{
    this->fred = fred;
}

void FredTopics::registerUnit(string section, Mapping::Unit& unit, Instructions &instructions)
{
    for (size_t uId = 0; uId < unit.unitIds.size(); uId++)
    {
        if (unit.unitIds[uId] == -1) continue;

        for (auto topicName = instructions.getTopics().begin(); topicName != instructions.getTopics().end(); topicName++)
        {
            string fullName = this->fred->Name() + "/" + section + "/" + unit.unitName + to_string(unit.unitIds[uId]) + "/" + *topicName;

            topics[fullName] = ChainTopic();
            topics[fullName].name = fullName;

            topics[fullName].instruction = &(instructions.getInstructions()[*topicName]);
            topics[fullName].unit = &unit;
            topics[fullName].alfLink = this->fred->getAlfClients().getAlfNode(unit.alfId, unit.serialId, unit.linkId, topics[fullName].instruction->type);

            topics[fullName].alfQueue = this->fred->getAlfClients().getAlfQueue(unit.alfId);
            //topics[fullName].alfDns = this->fred->getAlfClients().getAlfDns(unit.alfId);

            topics[fullName].interval = 0.0;
            topics[fullName].mapi = NULL;

            if (topics[fullName].instruction->subscribe)
            {
                topics[fullName].command = new SubscribeCommand(fullName + "_REQ", this->fred, &topics[fullName], uId);
                topics[fullName].alfInfo = new AlfInfo(this->fred->getAlfClients().getAlfSubscribeTopic(unit.alfId, unit.serialId, unit.linkId, topics[fullName].instruction->type, fullName), this->fred);
            }
            else
            {
                topics[fullName].command =  new MappedCommand(fullName + "_REQ", this->fred, &topics[fullName], uId);
                topics[fullName].alfInfo = NULL;
            }

            this->fred->RegisterCommand(topics[fullName].command);

            topics[fullName].placeId = uId;

            topics[fullName].service = new ServiceString(fullName + "_ANS", this->fred);
            this->fred->RegisterService(topics[fullName].service);

            topics[fullName].error = new ServiceString(fullName + "_ERR", this->fred);
            this->fred->RegisterService(topics[fullName].error);
        }
    }
}

void FredTopics::registerGroup(string section, Groups::Group& group)
{
    string fullName = this->fred->Name() + "/" + section + "/" + group.name;

    groupTopics[fullName] = GroupTopic();
    groupTopics[fullName].name = fullName;

    for (size_t i = 0; i < group.unitIds.size(); i++)
    {
        ChainTopic* chainTopic = &topics[this->fred->Name() + "/" + section + "/" + group.unitName + to_string(group.unitIds[i]) + "/" + group.topicName];
        groupTopics[fullName].chainTopics.push_back(chainTopic);
    }

    groupTopics[fullName].command = new GroupCommand(fullName + "_REQ", this->fred, &groupTopics[fullName]);
    this->fred->RegisterCommand(groupTopics[fullName].command);

    groupTopics[fullName].service = new ServiceString(fullName + "_ANS", this->fred);
    this->fred->RegisterService(groupTopics[fullName].service);

    groupTopics[fullName].error = new ServiceString(fullName + "_ERR", this->fred);
    this->fred->RegisterService(groupTopics[fullName].error);

    groupTopics[fullName].instruction = groupTopics[fullName].chainTopics[0]->instruction;

    groupTopics[fullName].inVars = group.inVars;
    groupTopics[fullName].unitIds = group.unitIds;
}

void FredTopics::registerMapiObject(string topic, MapiInterface* mapi)
{
    try
    {
        topics[topic].mapi = mapi;
        PrintVerbose("Mapi object registered to topic " + topic);
    }
    catch (const exception& e)
    {
        PrintError("Invalid topic requested in MAPI!");
    }
}
