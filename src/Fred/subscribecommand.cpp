#include "Fred/subscribecommand.h"
#include "Alfred/print.h"
#include "Parser/processmessage.h"
#include "Fred/fredtopics.h"
#include "Fred/queue.h"

SubscribeCommand::SubscribeCommand(string name, Fred* fred, ChainTopic *topic, int32_t placeId): CommandString::CommandString(name, (ALFRED*)fred)
{
    this->topic = topic;
    this->placeId = placeId;
}

SubscribeCommand::~SubscribeCommand()
{

}

const void* SubscribeCommand::Execution(void *value)
{
    if (!value)
    {
        PrintError("Invalid request, no value received!");
    }

    string request(static_cast<char*>(value));
    PrintVerbose("Received command:\n" + request);

    if (request.size() > 0)
    {
        size_t length = 0;
        while (isdigit(request[length]) || request[length] == '.') length++;

        float interval;

        try
        {
            interval = stof(request.substr(0, length));
            request.erase(0, length + 1);
        }
        catch (exception& e)
        {
            PrintError("Subscribe command must contain interval as first argument!");
            return NULL;
        }

        ProcessMessage* processMessage = new ProcessMessage(request, this->placeId);
        if (processMessage->isCorrect())
        {
            this->topic->interval = interval;
            this->topic->alfQueue->newRequest(make_pair(processMessage, this->topic));
        }
        else
        {
            delete processMessage;
        }
    }

    return NULL;
}
