#ifndef GROUPCOMMAND_H
#define GROUPCOMMAND_H

#include <string>
#include <thread>
#include <mutex>
#include <map>
#include "Alfred/command.h"

class Fred;
class GroupTopic;
class ChainTopic;

class GroupCommand: public CommandString
{
public:
    GroupCommand(string name, Fred* fred, GroupTopic* topic);
    ~GroupCommand();

    void receivedResponse(ChainTopic* topic, string response, bool error);

private:
    thread* queueThread;
    GroupTopic* topic;
    bool isFinished;
    mutex lock;
    bool groupError;

    map<int32_t, string> received;

    const void* Execution(void* value);
    static void processRequest(GroupCommand* command);
    void newRequest();
};

#endif // GROUPCOMMAND_H
