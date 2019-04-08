#ifndef CRUREGISTERCOMMAND_H
#define CRUREGISTERCOMMAND_H

#include <string>
#include "Alfred/command.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <list>
#include <algorithm>

class Fred;
class RpcInfoString;

class CruRegisterCommand: public CommandString
{
public:
    enum Type
    {
      READ,
      WRITE
    };

    CruRegisterCommand(Type type, Fred* fred);
    ~CruRegisterCommand();

private:
    Type type;

    const void* Execution(void* value);

    void executeWrite(vector<uint32_t>& message);
    void executeRead(vector<uint32_t>& message);
    string builAlfTopic(Type type, uint32_t alf, uint32_t serial);

    mutex lock;
    thread* clearThread;
    atomic<bool> isFinished, isProcessing;
    list<pair<string, RpcInfoString*> > stack;

    static void clearRequests(CruRegisterCommand* self);
    void newRequest(pair<string, RpcInfoString*> request);
};

#endif // CRUREGISTERCOMMAND_H
