#include "Fred/fredtopics.h"
#include "Fred/fred.h"
#include "Fred/queue.h"
#include "Fred/alfinfo.h"
#include "Alfred/print.h"
#include "Fred/selectcommand.h"
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

        vector<MappedCommand*> unitCommands;

        for (auto topicName = instructions.getTopics().begin(); topicName != instructions.getTopics().end(); topicName++)
        {
            string fullName = this->fred->Name() + "/" + section + "/" + unit.unitName + to_string(unit.unitIds[uId]) + "/" + *topicName;

            topics[fullName] = ChainTopic();
            topics[fullName].name = fullName;

            topics[fullName].instruction = &(instructions.getInstructions()[*topicName]);
            topics[fullName].unit = &unit;

            topics[fullName].alfLink.first = !unit.alfs.first.alfId.empty() ? this->fred->getAlfClients().getAlfNode(unit.alfs.first.alfId, unit.alfs.first.serialId, unit.alfs.first.linkId, topics[fullName].instruction->type) : NULL;
            topics[fullName].alfLink.second = !unit.alfs.second.alfId.empty() && topics[fullName].instruction->type == Instructions::Type::SWT ? this->fred->getAlfClients().getAlfNode(unit.alfs.second.alfId, unit.alfs.second.serialId, unit.alfs.second.linkId, topics[fullName].instruction->type) : NULL; //for backup CAN bus

            topics[fullName].alfQueue.first = topics[fullName].alfLink.first ? this->fred->getAlfClients().getAlfQueue(unit.alfs.first.alfId) : NULL;
            topics[fullName].alfQueue.second = topics[fullName].alfLink.second ? this->fred->getAlfClients().getAlfQueue(unit.alfs.second.alfId) : NULL;

            topics[fullName].placeId = uId;
            topics[fullName].mapi = NULL;

            topics[fullName].command =  new MappedCommand(fullName + "_REQ", this->fred, &topics[fullName], uId);
            this->fred->RegisterCommand(topics[fullName].command);

            topics[fullName].service = new ServiceString(fullName + "_ANS", this->fred);
            this->fred->RegisterService(topics[fullName].service);

            topics[fullName].error = new ServiceString(fullName + "_ERR", this->fred);
            this->fred->RegisterService(topics[fullName].error);

            if (!unit.alfs.second.alfId.empty() && topics[fullName].instruction->type == Instructions::Type::SWT)
            {
                unitCommands.push_back((MappedCommand*)topics[fullName].command);
            }
        }

        if (unitCommands.size())
        {
            ServiceString* service = new ServiceString(this->fred->Name() + "/" + section + "/" + unit.unitName + to_string(unit.unitIds[uId]) + "/SELECT_ALF_ANS", this->fred);
            service->Update("ALF");
            this->fred->RegisterService(service);
            this->fred->RegisterCommand(new SelectCommand(this->fred->Name() + "/" + section + "/" + unit.unitName + to_string(unit.unitIds[uId]) + "/SELECT_ALF_REQ", this->fred, unitCommands, service));
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

void FredTopics::registerMapiObject(string topic, Mapi* mapi)
{
    auto it = topics.find(topic);
    if (it == topics.end())
    {
        Print::PrintError("Requested MAPI topic " + topic + " is not a registered topic!");
        throw runtime_error("Requested MAPI topic " + topic + " is not a registered topic!");
    }
    else
    {
        topics[topic].mapi = mapi;
        Print::PrintVerbose("Mapi object registered to " + topic);
    }
}

/*
 * To give MAPI access to the list (map) of topics
 */
map<string, ChainTopic>& FredTopics::getTopicsMap()
{
    return topics;
}
