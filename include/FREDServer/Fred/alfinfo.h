#ifndef ALFINFO_H
#define ALFINFO_H

#include <string>

#include "Alfred/info.h"
#include "Parser/processmessage.h"
#include "Fred/fredtopics.h"

using namespace std;

class Fred;

class AlfInfo: public InfoString
{
private:
    pair<ProcessMessage*, ChainTopic*> currentTransaction;

    const void* Execution(void* value);
    bool isTransactionAvailable();

public:
    AlfInfo(string name, Fred* fred);
    void setTransaction(pair<ProcessMessage*, ChainTopic*> currentTransaction);
    void clearTransaction();
};

#endif // ALFINFO_H
