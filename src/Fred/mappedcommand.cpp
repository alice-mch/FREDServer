#include "Fred/mappedcommand.h"
#include "Alfred/print.h"
#include "Parser/processmessage.h"
#include "Fred/fredtopics.h"
#include "Fred/queue.h"

MappedCommand::MappedCommand(string name, Fred* fred, ChainTopic *topic, int32_t placeId): CommandString::CommandString(name, (ALFRED*)fred)
{
    this->topic = topic;
    this->placeId = placeId;
}

MappedCommand::~MappedCommand()
{

}

const void* MappedCommand::Execution(void *value)
{
    if (!value)
    {
        PrintError(name, "Invalid request, no value received!");
    }

    string request(static_cast<char*>(value));

    PrintVerbose(name, "Received command: \n" + request);

    if (topic->mapi == NULL)
    {
        ProcessMessage* processMessage = new ProcessMessage(request, this->placeId);
        if (processMessage->isCorrect())
        {
            this->topic->alfQueue->newRequest(make_pair(processMessage, this->topic));
        }
        else
        {
            PrintError(name, "Invalid input received!");
            delete processMessage;
        }
    }
    else
    {
        ProcessMessage* processMessage = new ProcessMessage(topic->mapi, request);
        this->topic->alfQueue->newRequest(make_pair(processMessage, this->topic));
    }

    return NULL;
}
