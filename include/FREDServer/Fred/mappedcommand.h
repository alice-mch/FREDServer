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

    void setUseCru(bool useCru);
    bool getUseCru();

private:
    ChainTopic* topic;
    int32_t placeId;
    bool useCru;

    const void* Execution(void* value);
};

#endif // MAPPEDCOMMAND_H
