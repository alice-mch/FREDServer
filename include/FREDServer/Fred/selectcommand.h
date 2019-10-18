#ifndef SELECTCOMMAND_H
#define SELECTCOMMAND_H

#include <string>
#include <vector>
#include "Fred/mappedcommand.h"
#include "Alfred/command.h"
#include "Alfred/service.h"

class Fred;
class ChainTopic;

class SelectCommand: public CommandString
{
public:
    SelectCommand(string name, Fred* fred, vector<MappedCommand*> unitCommands, ServiceString* service);
    ~SelectCommand();

private:
    vector<MappedCommand*> unitCommands;
    ServiceString* service;

    const void* Execution(void* value);
};

#endif // MAPPEDCOMMAND_H
