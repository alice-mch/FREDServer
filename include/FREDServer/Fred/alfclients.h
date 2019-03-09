#ifndef ALFCLIENTS_H
#define ALFCLIENTS_H

#include <string>
#include <vector>
#include <map>

#include "Fred/Config/location.h"
#include "Fred/Config/instructions.h"
#include "Fred/queue.h"
#include "Alfred/rpcinfo.h"

using namespace std;

class Fred;
class AlfRpcInfo;

class AlfClients
{
public:
    struct Nodes
    {
        AlfRpcInfo *sca, *swt;
    };

private:
    map<int32_t, map<int32_t, map<int32_t, Nodes> > > clients;
    map<int32_t, Queue*> queues;
    map<int32_t, string> dns;
    Fred* fred;

public:
    AlfClients(Fred* fred);
    ~AlfClients();

    void registerAlf(Location::AlfEntry& entry);
    Nodes createAlfInfo(string dns, int32_t serial, int32_t link);

    AlfRpcInfo* getAlfNode(int32_t alf, int32_t serial, int32_t link, Instructions::Type type);
    RpcInfoString *getAlfNode(int32_t alf, int32_t serial, int32_t link, Instructions::Type type, bool start);
    Queue* getAlfQueue(int32_t alf);
    string getAlfDns(int32_t alf);
    string getAlfSubscribeTopic(int32_t alf, int32_t serial, int32_t link, Instructions::Type type, string name);
};

#endif // ALFCLIENTS_H
