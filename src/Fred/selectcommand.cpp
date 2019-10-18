#include "Fred/selectcommand.h"
#include "Alfred/print.h"

SelectCommand::SelectCommand(string name, Fred* fred, vector<MappedCommand*> unitCommands, ServiceString *service): CommandString::CommandString(name, (ALFRED*)fred)
{
    this->unitCommands = unitCommands;
    this->service = service;
}

SelectCommand::~SelectCommand()
{

}

const void* SelectCommand::Execution(void* value)
{
    if (!value)
    {
        Print::PrintError(name, "Invalid request, no value received!");
    }

    string request(static_cast<char*>(value));

    if (request != "ALF" && request != "CANALF")
    {
        Print::PrintError("Cannot switch to: " + request + " - uknown option");
        return NULL;
    }

    Print::PrintVerbose(name, "Received command to switch to: " + request);
    this->service->Update(request.c_str());

    bool useCru = request == "ALF";
    for (size_t  i = 0; i < this->unitCommands.size(); i++)
    {
        this->unitCommands[i]->setUseCru(useCru);
    }

    return NULL;
}
