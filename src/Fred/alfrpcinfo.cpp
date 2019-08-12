#include "Fred/alfrpcinfo.h"
#include "Alfred/print.h"
#include <algorithm>

AlfRpcInfo::AlfRpcInfo(string name, string dns, Fred* fred): RpcInfoString::RpcInfoString(name, dns, (ALFRED*)fred)
{
    this->currentTransaction.first = NULL;
    this->currentTransaction.second = NULL;
    this->name = name;
}

const void* AlfRpcInfo::Execution(void *value) //todo
{
    if (!value)
    {
        PrintError(currentTransaction.second->name, "Invalid RPC Info received!");
        return NULL;
    }

    PrintVerbose(currentTransaction.second->name, "Received RPC Info from " + name + ":\n" + string(static_cast<char*>(value)));

    if (isTransactionAvailable())
    {
        if (this->currentTransaction.second->mapi == NULL)
        {
            this->currentTransaction.first->evaluateMessage(static_cast<char*>(value), *this->currentTransaction.second);
        }
        else
        {
            this->currentTransaction.first->evaluateMapiMessage(static_cast<char*>(value), *this->currentTransaction.second);
        }
    }
    else
    {
        PrintWarning(currentTransaction.second->name, "No current transaction available! Discarding message");
    }

    clearTransaction();

    return NULL;
}

void AlfRpcInfo::setTransaction(pair<ProcessMessage*, ChainTopic*> currentTransaction)
{
    this->currentTransaction = currentTransaction;
}

void AlfRpcInfo::clearTransaction()
{
    if (this->currentTransaction.first) delete this->currentTransaction.first;
    this->currentTransaction.first = NULL;
    this->currentTransaction.second = NULL;
}

bool AlfRpcInfo::isTransactionAvailable()
{
    return this->currentTransaction.first && this->currentTransaction.second;
}
