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
        AlfRpcInfo *sca, *swt, *ic;
    };

private:
    map<string, map<int32_t, map<int32_t, Nodes> > > clients;
    map<string, Queue*> queues;
    //map<string, string> dns;
    Fred* fred;

public:
    AlfClients(Fred* fred);
    ~AlfClients();

    void registerAlf(Location::AlfEntry& entry);
    Nodes createAlfInfo(string id, int32_t serial, int32_t link);

    AlfRpcInfo* getAlfNode(string alf, int32_t serial, int32_t link, Instructions::Type type);
    RpcInfoString *getAlfNode(string alf, int32_t serial, int32_t link, Instructions::Type type, bool start);
    Queue* getAlfQueue(string alf);
    //string getAlfDns(string alf);
    string getAlfSubscribeTopic(string alf, int32_t serial, int32_t link, Instructions::Type type, string name);
};

#endif // ALFCLIENTS_H
