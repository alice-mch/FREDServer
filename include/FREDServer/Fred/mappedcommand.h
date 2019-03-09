#ifndef MAPPEDCOMMAND_H
#define MAPPEDCOMMAND_H

#include <string>
#include "Alfred/command.h"

class Fred;
class ChainTopic;

class MappedCommand: public CommandString
{
public:
    MappedCommand(string name, Fred* fred, ChainTopic* topic, int32_t placeId);
    ~MappedCommand();

private:
    ChainTopic* topic;
    int32_t placeId;

    const void* Execution(void* value);
};

#endif // MAPPEDCOMMAND_H
