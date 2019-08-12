#ifndef ALFRPCINFO_H
#define ALFRPCINFO_H

#include <string>
#include "Alfred/rpcinfo.h"
#include "Fred/fred.h"

using namespace std;

class AlfRpcInfo: public RpcInfoString
{
private:
    pair<ProcessMessage*, ChainTopic*> currentTransaction;

    const void* Execution(void* value);
    void clearTransaction();
    bool isTransactionAvailable();
    string name;

public:
    AlfRpcInfo(string name, string dns, Fred* fred);
    void setTransaction(pair<ProcessMessage*, ChainTopic*> currentTransaction);
};

#endif // ALFRPCINFO_H
