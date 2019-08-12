#include <algorithm>
#include "Fred/groupcommand.h"
#include "Alfred/print.h"
#include "Parser/processmessage.h"
#include "Fred/fredtopics.h"
#include "Fred/queue.h"

GroupCommand::GroupCommand(string name, Fred* fred, GroupTopic *topic): CommandString::CommandString(name, (ALFRED*)fred)
{
    this->topic = topic;
    this->isFinished = false;
    this->queueThread = new thread(processRequest, this);
    this->groupError = false;
}

GroupCommand::~GroupCommand()
{
    isFinished = true;
    lock.unlock();
    queueThread->join();
    delete queueThread;
}

const void* GroupCommand::Execution(void *value)
{
    if (!value)
    {
        PrintError(topic->name, "Invalid request, no value received!");
    }

    PrintVerbose(topic->name, "Received group command:\n");

    for (size_t i = 0; i < topic->chainTopics.size(); i++)
    {
        ProcessMessage* processMessage = new ProcessMessage(topic->inVars, topic->chainTopics[i]->placeId, this);
        this->topic->chainTopics[i]->alfQueue->newRequest(make_pair(processMessage, this->topic->chainTopics[i]));
    }

    newRequest();

    return NULL;
}

void GroupCommand::processRequest(GroupCommand* command)
{
    command->lock.lock();

    while (1)
    {
        command->lock.lock();

        if (command->isFinished)
        {
            return;
        }

        while (command->received.size() < command->topic->chainTopics.size())
        {
            usleep(10);
        }

        string response;
        for (size_t i = 0; i < command->topic->unitIds.size(); i++)
        {
            response += command->received[command->topic->unitIds[i]];
            if (i < command->topic->unitIds.size() - 1) response += "\n";
        }

        if (command->groupError) 
        {           
            command->topic->error->Update(response.c_str()); //_ERR
            PrintError(command->topic->name, "Updating group error service!");
        }
        else
        {
            command->topic->service->Update(response.c_str()); //_ANS
            PrintVerbose(command->topic->name, "Updating group service!");
        }
        
        command->groupError = false;
        command->received.clear();
    }

}

void GroupCommand::newRequest()
{
    lock.unlock();
}

void GroupCommand::receivedResponse(ChainTopic *topic, string response, bool error)
{
    if (error) groupError = true;

    size_t idx = distance(this->topic->chainTopics.begin(), find(this->topic->chainTopics.begin(), this->topic->chainTopics.end(), topic));
    this->received[this->topic->unitIds[idx]] = response;
}
