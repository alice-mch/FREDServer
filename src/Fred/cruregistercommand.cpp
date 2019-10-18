#include "Fred/cruregistercommand.h"
#include "Alfred/print.h"
#include "Fred/fred.h"
#include "Parser/utility.h"
#include <sstream>
#include <iomanip>
#include <exception>

CruRegisterCommand::CruRegisterCommand(Type type, Fred* fred): CommandString::CommandString(fred->Name() + (type == WRITE ? "/CRU_REGISTER/WRITE_REQ" : "/CRU_REGISTER/READ_REQ"), (ALFRED*)fred)
{
    this->type = type;
    fred->RegisterService(fred->Name() + (type == WRITE ? "/CRU_REGISTER/WRITE_ANS" : "/CRU_REGISTER/READ_ANS"), DIM_TYPE::STRING);

    this->isFinished = false;
    this->isProcessing = false;
    this->clearThread = new thread(clearRequests, this);
}

CruRegisterCommand::~CruRegisterCommand()
{
    this->isFinished = true;
    lock.unlock();
    clearThread->join();
    delete clearThread;
}

const void* CruRegisterCommand::Execution(void *value)
{
    if (!value)
    {
        Print::PrintError("Invalid request, no value received!");
    }

    string request(static_cast<char*>(value));

    Print::PrintVerbose("Received command:\n" + request);

    vector<uint32_t> splitted;
    vector<double> splittedDouble = Utility::splitString2Num(request, ",");
    for (size_t i = 0; i < splittedDouble.size(); i++)
    {
        splitted.push_back(uint32_t(splittedDouble[i]));
    }

    if (this->type == WRITE)
    {
        if (splitted.size() < 4)
        {
            Print::PrintError("Invalid number of arguments received for CRU_REGISTER WRITE");
            return NULL;
        }

        executeWrite(splitted);
    }
    else
    {
        if (splitted.size() < 3)
        {
            Print::PrintError("Invalid number of arguments received for CRU_REGISTER READ");
            return NULL;
        }

        executeRead(splitted);
    }

    return NULL;
}

void CruRegisterCommand::executeWrite(vector<uint32_t>& message)
{
    stringstream requestSS;
    requestSS << hex << message[2] << "\n" << hex << message[3];

    string request(requestSS.str());
    string alfTopic = builAlfTopic(WRITE, message[0], message[1]);

    RpcInfoString* rpcInfo = (RpcInfoString*)Parent()->GetRpcInfo(alfTopic);
    if (!rpcInfo)
    {
        Print::PrintError("Cannot find RPC Info " + alfTopic + "!");
        return;
    }

    newRequest(make_pair(request, rpcInfo));
}

void CruRegisterCommand::executeRead(vector<uint32_t>& message)
{
    stringstream requestSS;
    requestSS << hex << message[2];

    string request(requestSS.str());
    string alfTopic = builAlfTopic(READ, message[0], message[1]);

    RpcInfoString* rpcInfo = (RpcInfoString*)Parent()->GetRpcInfo(alfTopic);
    if (!rpcInfo)
    {
        Print::PrintError("Cannot find RPC Info " + alfTopic + "!");
        return;
    }

    newRequest(make_pair(request, rpcInfo));
}

string CruRegisterCommand::builAlfTopic(Type type, uint32_t alf, uint32_t serial)
{
    return "ALF" + to_string(alf) + "/SERIAL_" + to_string(serial) + "/LINK_0/REGISTER_" + (type == WRITE ? "WRITE" : "READ");
}

void CruRegisterCommand::clearRequests(CruRegisterCommand *self)
{
    while (1)
    {
        if (self->isFinished)
        {
            return;
        }

        self->lock.lock();

        while (!self->stack.empty())
        {
            self->isProcessing = true;

            pair<string, RpcInfoString*> request = self->stack.front();
            self->stack.pop_front();
            //do processing

            char* buffer = strdup(request.first.c_str());
            request.second->Send(buffer);
            free(buffer);

            self->isProcessing = false;
        }
    }
}

void CruRegisterCommand::newRequest(pair<string, RpcInfoString*> request)
{
    stack.push_back(request);

    if (!isProcessing)
    {
        lock.unlock();
    }
}
