#include <thread>
#include "Alfred/rpc.h"
#include "Alfred/service.h"
#include "Alfred/frontend.h"
#include "Alfred/client.h"
#include "Alfred/print.h"
#include "Alfred/alfred.h"

vector<string> Rpc::names;

bool Rpc::AlreadyRegistered(const string& name)
{
    for (size_t i = 0; i < names.size(); i++)
    {
        if (names[i] == name)
        {
            return true;
        }
    }

    return false;
}

void Rpc::RemoveElement(const string& name)
{
    for (size_t i = 0; i < names.size(); i++)
    {
        if (names[i] == name)
        {
            names.erase(names.begin() + i);
            return;
        }
    }
}


Rpc::Rpc(string name, ALFRED* alfred)
{
    serviceCallback = NULL;
    functionCallback = NULL;
    clientCallback = NULL;

    type = DIM_TYPE::NONE;
    this->name = name;

    if (!alfred)
    {
        Print::PrintError(string("ALFRED for rpc ") + name + " not defined!");
        exit(EXIT_FAILURE);
    }

    this->alfred = alfred;

    if (AlreadyRegistered(name))
    {
        Print::PrintError(string("Rpc ") + name + " already registered!");
        exit(EXIT_FAILURE);
    }
    else
    {
        names.push_back(name);
    }
}

Rpc::~Rpc()
{
    RemoveElement(Name());
}

const void* Rpc::Execution(void* value)
{
    return value;
}

void Rpc::ConnectService(Service* serviceCallback)
{
    this->serviceCallback = serviceCallback;
}

void Rpc::ConnectFunction(FunctionShot* functionCallback)
{
    this->functionCallback = functionCallback;
}

void Rpc::ConnectClient(Client* clientCallback)
{
    this->clientCallback = clientCallback;
}

void Rpc::CallService(string name, void* value)
{
    Parent()->GetService(name)->Update(value);
}

void Rpc::CallFunction(string name, void* value)
{
    Function* function = Parent()->GetFunction(name);

    if (function->Type() != FNC_TYPE::SHOT)
    {
        Print::PrintError("Cannot call non-shot function!");
        exit(EXIT_FAILURE);
    }
    else
    {
        ((FunctionShot*)function)->Shot(value);
    }
}

void Rpc::CallClient(string name, void* value)
{
    Parent()->GetClient(name)->Send(value);
}

DIM_TYPE Rpc::Type()
{
    return type;
}

string Rpc::Name()
{
    return name;
}

ALFRED* Rpc::Parent()
{
    return alfred;
}

/*----------------------------------------------------------------------------------------------*/

RpcInt::RpcInt(string name, ALFRED* alfred): Rpc::Rpc(name, alfred), DimRpc::DimRpc(name.c_str(), "I", "I")
{
    type = DIM_TYPE::INT;

    Print::PrintVerbose(string("Rpc ") + name + " registered!");
}

RpcInt::~RpcInt()
{

}

void RpcInt::rpcHandler()
{
    value = getInt();

    void* result = (void*)Execution((void*)&value);

    if (serviceCallback)
    {
        serviceCallback->Update(result);
    }

    if (functionCallback)
    {
        functionCallback->Shot(result);
    }

    if (clientCallback)
    {
        clientCallback->Send(result);
    }

    if (!result)
    {
        Print::PrintWarning(string("Rpc ") + Name() + " not valid data!");
        setData(noLink);
        return;
    }

    setData(*(int*)result);
}

/*----------------------------------------------------------------------------------------------*/

RpcFloat::RpcFloat(string name, ALFRED* alfred): Rpc::Rpc(name, alfred), DimRpc::DimRpc(name.c_str(), "F", "F")
{
    type = DIM_TYPE::FLOAT;

    Print::PrintVerbose(string("Rpc ") + name + " registered!");
}

RpcFloat::~RpcFloat()
{

}

void RpcFloat::rpcHandler()
{
    value = getFloat();

    void* result = (void*)Execution((void*)&value);

    if (serviceCallback)
    {
        serviceCallback->Update(result);
    }

    if (functionCallback)
    {
        functionCallback->Shot(result);
    }

    if (clientCallback)
    {
        clientCallback->Send(result);
    }

    if (!result)
    {
        Print::PrintWarning(string("Rpc ") + Name() + " not valid data!");
        setData(noLink);
        return;
    }

    setData(*(float*)result);
}

/*----------------------------------------------------------------------------------------------*/

RpcString::RpcString(string name, ALFRED* alfred): Rpc::Rpc(name, alfred), DimRpc::DimRpc(name.c_str(), "C", "C")
{
    type = DIM_TYPE::STRING;
    noLink[0] = '\0';

    Print::PrintVerbose(string("Rpc ") + name + " registered!");
}

RpcString::~RpcString()
{

}

void RpcString::rpcHandler()
{
    value = getString();

    void* result = (void*)Execution((void*)value);

    if (serviceCallback)
    {
        serviceCallback->Update(result);
    }

    if (functionCallback)
    {
        functionCallback->Shot(result);
    }

    if (clientCallback)
    {
        clientCallback->Send(result);
    }

    if (!result)
    {
        Print::PrintWarning(string("Rpc ") + Name() + " not valid data!");
        setData(noLink);
        return;
    }

    setData((char*)result);
}

/*----------------------------------------------------------------------------------------------*/

RpcData::RpcData(string name, ALFRED* alfred, size_t size, string formatIn, string formatOut): Rpc::Rpc(name, alfred), DimRpc::DimRpc(name.c_str(), formatIn.c_str(), formatOut.c_str())
{
    type = DIM_TYPE::DATA;
    this->size = size;

    Print::PrintVerbose(string("Rpc ") + name + " registered!");
}

RpcData::~RpcData()
{

}

void RpcData::rpcHandler()
{
    value = getData();

    if (!value)
    {
        Print::PrintWarning(string("Rpc ") + Name() + " not valid data!");
        setData(&noLink);
        return;
    }

    void* result = (void*)Execution(value);

    if (serviceCallback)
    {
        serviceCallback->Update(result);
    }

    if (functionCallback)
    {
        functionCallback->Shot(result);
    }

    if (clientCallback)
    {
        clientCallback->Send(result);
    }

    if (!result)
    {
        Print::PrintWarning(string("Rpc ") + Name() + " not valid data!");
        setData(&noLink);
        return;
    }

    setData(result, (int)size);
}
