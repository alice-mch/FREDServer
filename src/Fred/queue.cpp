#include "Fred/queue.h"
#include "Fred/alfrpcinfo.h"
#include "Fred/alfinfo.h"
#include "Alfred/print.h"

Queue::Queue(string alfId, Fred* fred)
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

            PrintVerbose(request.second->name, "Parsing message");

            string fullMessage;
            bool noRpcRequest = false; //if true, the RPC request is not sent

            try
            {
                if (request.second->mapi == NULL)
                {
                    fullMessage = request.first->generateFullMessage(*request.second->instruction);
                }
                else
                {
                    fullMessage = request.first->generateMapiMessage();
                    noRpcRequest = request.second->mapi->noRpcRequest;
                }
            }
            catch (exception& e)
            {
                string thrown = e.what();
                string errorMessage;
                if (thrown == "invar-arguments-number")
                {
                    errorMessage = "Number of arguments doesn't correspond with the config file!";
                    PrintError(request.second->name, errorMessage);
                }
                else
                {
                    errorMessage = "Error occured during parsing of the message!";
                    PrintError(request.second->name, errorMessage);
                }

                request.second->error->Update(errorMessage.c_str());
                PrintError(request.second->name, "Updating error service!");

                queue->isProcessing = false;
                continue;
            }

            char* buffer;

            if (!request.second->instruction->subscribe || request.second->mapi != NULL) //if MAPI or not subscribe topic
            {
                if (noRpcRequest)
                {
                    PrintVerbose(request.second->name, "Skipping RPC request");
                    noRpcRequest = false; //reset noRpcRequest 
                    fullMessage = "not empty string";
                    buffer = strdup(fullMessage.c_str());
                }
                else
                {
                    buffer = strdup(fullMessage.c_str());
                    PrintVerbose(request.second->name, "Sending RPC request:\n" + string(buffer));
                    request.second->alfLink->Send(buffer);
                }
            }
            else
            {
                Mapping::Unit* unit = request.second->unit;
                if (request.second->interval > 0.0)
                {
                    RpcInfoString* rpcInfo = queue->fred->getAlfClients().getAlfNode(unit->alfId, unit->serialId, unit->linkId, request.second->instruction->type, true);
                    fullMessage = request.second->name + "\n" + to_string(request.second->interval) + "\n" + fullMessage;
                    buffer = strdup(fullMessage.c_str());
                    PrintVerbose(request.second->name, "Sending RPC subscribe request:\n" + string(buffer));
                    request.second->alfInfo->setTransaction(request);
                    rpcInfo->Send(buffer);
                }
                else
                {
                    RpcInfoString* rpcInfo = queue->fred->getAlfClients().getAlfNode(unit->alfId, unit->serialId, unit->linkId, request.second->instruction->type, false);
                    fullMessage = request.second->name;
                    buffer = strdup(fullMessage.c_str());
                    PrintVerbose(request.second->name, "Sending RPC unsubscribe request:\n");
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
