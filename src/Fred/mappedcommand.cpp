#include "Fred/mappedcommand.h"
#include "Alfred/print.h"
#include "Parser/processmessage.h"
#include "Fred/fredtopics.h"
#include "Fred/queue.h"

MappedCommand::MappedCommand(string name, Fred* fred, ChainTopic *topic, int32_t placeId): CommandString::CommandString(name, (ALFRED*)fred)
{
    this->topic = topic;
    this->placeId = placeId;
    this->useCru = true;
}

MappedCommand::~MappedCommand()
{

}

const void* MappedCommand::Execution(void *value)
{
    if (!value)
    {
        Print::PrintError(name, "Invalid request, no value received!");
    }

    string request(static_cast<char*>(value));

    Print::PrintVerbose(name, "Received command: \n" + request);

    if (topic->mapi == NULL)
    {
        ProcessMessage* processMessage = new ProcessMessage(request, this->placeId, this->useCru);
        if (processMessage->isCorrect())
        {
            Queue* queue = this->useCru ? this->topic->alfQueue.first : this->topic->alfQueue.second;
            if (!queue)
            {
                string error = "Required ALF/CANALF not available!";
                Print::PrintError(name, error);
                topic->error->Update(error.c_str());
                Print::PrintError(topic->name, "Updating error service!");
                delete processMessage;
                return NULL;
            }

            queue->newRequest(make_pair(processMessage, this->topic));
        }
        else
        {
            string error = "Invalid input received!";
            Print::PrintError(name, error);
            topic->error->Update(error.c_str());
            Print::PrintError(topic->name, "Updating error service!");
            delete processMessage;
        }
    }
    else
    {
        ProcessMessage* processMessage = new ProcessMessage(topic->mapi, request, this->useCru);

        Queue* queue = this->useCru ? this->topic->alfQueue.first : this->topic->alfQueue.second;
        if (!queue)
        {
            string error = "Required ALF/CANALF not available!";
            Print::PrintError(name, error);
            topic->error->Update(error.c_str());
            Print::PrintError(topic->name, "Updating error service!");
            delete processMessage;
            return NULL;
        }

        queue->newRequest(make_pair(processMessage, this->topic));
    }

    return NULL;
}

void MappedCommand::setUseCru(bool useCru)
{
    this->useCru = useCru;
}

bool MappedCommand::getUseCru()
{
    return this->useCru;
}
