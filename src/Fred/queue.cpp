#include "Fred/queue.h"
#include "Fred/alfrpcinfo.h"
#include "Fred/alfinfo.h"
#include "Alfred/print.h"

Queue::Queue(int32_t alfId, Fred* fred)
{
    this->id = alfId;
    this->fred = fred;
    this->isFinished = false;
    this->isProcessing = false;
    this->queueThread = new thread(clearQueue, this);
}

Queue::~Queue()
{
    this->isFinished = true;
    lock.unlock();
    queueThread->join();
    delete queueThread;
}

void Queue::clearQueue(Queue *queue)
{
    while (1)
    {
        if (queue->isFinished)
        {
            return;
        }

        queue->lock.lock();

        while (!queue->stack.empty())
        {
            queue->isProcessing = true;

            pair<ProcessMessage*, ChainTopic*> request = queue->stack.front();
            queue->stack.pop_front();
            //do processing
            request.second->alfLink->setTransaction(request);

            PrintVerbose("Parsing message");

            string fullMessage;

            try
            {
                if (request.second->mapi == NULL)
                {
                    fullMessage = request.first->generateFullMessage(*request.second->instruction);
                }
                else
                {
                    fullMessage = request.first->generateMapiMessage();
                }
            }
            catch (exception& e)
            {
                PrintError("Error occured during parsing of the message!");
                queue->isProcessing = false;
                continue;
            }

            char* buffer;

            if (!request.second->instruction->subscribe || request.second->mapi != NULL)
            {
                buffer = strdup(fullMessage.c_str());
                PrintVerbose("Sending RPC request:\n" + string(buffer));
                request.second->alfLink->Send(buffer);
            }
            else
            {
                Mapping::Unit* unit = request.second->unit;
                if (request.second->interval > 0.0)
                {
                    RpcInfoString* rpcInfo = queue->fred->getAlfClients().getAlfNode(unit->alfId, unit->serialId, unit->linkId, request.second->instruction->type, true);
                    fullMessage = request.second->name + "\n" + to_string(request.second->interval) + "\n" + fullMessage;
                    buffer = strdup(fullMessage.c_str());
                    PrintVerbose("Sending RPC subscribe request:\n" + string(buffer));
                    request.second->alfInfo->setTransaction(request);
                    rpcInfo->Send(buffer);
                }
                else
                {
                    RpcInfoString* rpcInfo = queue->fred->getAlfClients().getAlfNode(unit->alfId, unit->serialId, unit->linkId, request.second->instruction->type, false);
                    fullMessage = request.second->name;
                    buffer = strdup(fullMessage.c_str());
                    PrintVerbose("Sending RPC unsubscribe request:\n");
                    request.second->alfInfo->clearTransaction();
                    rpcInfo->Send(buffer);
                }
            }

            free(buffer);


            queue->isProcessing = false;
        }
    }
}

void Queue::newRequest(pair<ProcessMessage*, ChainTopic*> request)
{
    stack.push_back(request);

    if (!isProcessing)
    {
        lock.unlock();
    }
}
