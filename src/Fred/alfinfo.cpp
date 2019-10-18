#include "Fred/alfinfo.h"
#include "Alfred/print.h"

AlfInfo::AlfInfo(string name, Fred* fred): InfoString::InfoString(name, (ALFRED*)fred)
{
    this->currentTransaction.first = NULL;
    this->currentTransaction.second = NULL;
}

const void* AlfInfo::Execution(void* value)
{
    if (!value)
    {
        Print::PrintError("Invalid Info received!");
        return NULL;
    }

    //Print::PrintDebug("Received Info:\n" + string(static_cast<char*>(value)));    //ignored because of lot of messages

    if (isTransactionAvailable())
    {
        this->currentTransaction.first->evaluateMessage(static_cast<char*>(value), *this->currentTransaction.second, true);
    }

    return NULL;
}

void AlfInfo::setTransaction(pair<ProcessMessage*, ChainTopic*> currentTransaction)
{
    clearTransaction();
    this->currentTransaction = currentTransaction;
}

void AlfInfo::clearTransaction()
{
    if (this->currentTransaction.first) delete this->currentTransaction.first;
    this->currentTransaction.first = NULL;
    this->currentTransaction.second = NULL;
}

bool AlfInfo::isTransactionAvailable()
{
    return this->currentTransaction.first && this->currentTransaction.second;
}
