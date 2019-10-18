#include "Alfred/rpcinfo.h"
#include "Alfred/service.h"
#include "Alfred/frontend.h"
#include "Alfred/client.h"
#include "Alfred/print.h"
#include "Alfred/alfred.h"

vector<pair<string, string> > RpcInfo::names;

bool RpcInfo::AlreadyRegistered(const string& name, const string& dns)
{
    for (size_t i = 0; i < names.size(); i++)
    {
        if (names[i].first == name && names[i].second == dns)
        {
            return true;
        }
    }

    return false;
}

void RpcInfo::RemoveElement(const string& name, const string &dns)
{
    for (size_t i = 0; i < names.size(); i++)
    {
        if (names[i].first == name && names[i].second == dns)
        {
            names.erase(names.begin() + i);
            return;
        }
    }
}


RpcInfo::RpcInfo(string name, string dns, ALFRED* alfred)
{
    serviceCallback = NULL;
    functionCallback = NULL;
    clientCallback = NULL;

    type = DIM_TYPE::NONE;
    this->name = name;
    this->dns = dns;

    if (!alfred)
    {
        Print::PrintError(string("ALFRED for RpcInfo ") + name + " not defined!");
        exit(EXIT_FAILURE);
    }

    this->alfred = alfred;

    if (AlreadyRegistered(name, dns))
    {
        Print::PrintError(string("RpcInfo ") + name + " already registered!");
        exit(EXIT_FAILURE);
    }
    else
    {
        names.push_back(pair<string, string>(name, dns));
        //DimClient::setDnsNode(dns.c_str());
    }
}

RpcInfo::~RpcInfo()
{
    RemoveElement(Name(), Dns());
}

const void* RpcInfo::Execution(void* value)
{
    return value;
}

void RpcInfo::ConnectService(Service* serviceCallback)
{
    this->serviceCallback = serviceCallback;
}

void RpcInfo::ConnectFunction(FunctionShot* functionCallback)
{
    this->functionCallback = functionCallback;
}

void RpcInfo::ConnectClient(Client* clientCallback)
{
    this->clientCallback = clientCallback;
}

void RpcInfo::CallService(string name, void* value)
{
    Parent()->GetService(name)->Update(value);
}

void RpcInfo::CallFunction(string name, void* value)
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

void* RpcInfo::Send(void *value)
{
    switch (type)
    {
        case DIM_TYPE::INT:
            return ((RpcInfoInt*)this)->Send(*(int*)value);
        case DIM_TYPE::FLOAT:
            return ((RpcInfoFloat*)this)->Send(*(float*)value);
        case DIM_TYPE::STRING:
            return ((RpcInfoString*)this)->Send((char*) value);
        case DIM_TYPE::DATA:
            return ((RpcInfoData*)this)->Send(value);
        default:
            Print::PrintError("Invalid type of client!");
            exit(EXIT_FAILURE);
    }
}

void RpcInfo::CallClient(string name, void* value)
{
    Parent()->GetClient(name)->Send(value);
}

DIM_TYPE RpcInfo::Type()
{
    return type;
}

string RpcInfo::Name()
{
    return name;
}

string RpcInfo::Dns()
{
    return dns;
}

ALFRED* RpcInfo::Parent()
{
    return alfred;
}

/*----------------------------------------------------------------------------------------------*/

RpcInfoInt::RpcInfoInt(string name, string dns, ALFRED* alfred): RpcInfo::RpcInfo(name, dns, alfred)
{
    type = DIM_TYPE::INT;
    rpcInfo = new DimRpcInfo(name.c_str(), -1);

    Print::PrintVerbose(string("RpcInfo ") + name + " registered!");
}

RpcInfoInt::~RpcInfoInt()
{
    delete rpcInfo;
}

void* RpcInfoInt::Send(int value)
{
    rpcInfo->setData(value);

    int recValue = rpcInfo->getInt();

    void* result = (void*)Execution((void*)&recValue);

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
        //Print::PrintWarning(string("RpcInfo ") + Name() + " not valid data!");
        return NULL;
    }

    return result;
}

/*----------------------------------------------------------------------------------------------*/

RpcInfoFloat::RpcInfoFloat(string name, string dns, ALFRED* alfred): RpcInfo::RpcInfo(name, dns, alfred)
{
    type = DIM_TYPE::FLOAT;
    rpcInfo = new DimRpcInfo(name.c_str(), -1.0);

    Print::PrintVerbose(string("RpcInfo ") + name + " registered!");
}

RpcInfoFloat::~RpcInfoFloat()
{
    delete rpcInfo;
}

void* RpcInfoFloat::Send(float value)
{
    rpcInfo->setData(value);

    float recValue = rpcInfo->getFloat();

    void* result = (void*)Execution((void*)&recValue);

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
        //Print::PrintWarning(string("RpcInfo ") + Name() + " not valid data!");
        return NULL;
    }

    return result;
}

/*----------------------------------------------------------------------------------------------*/

RpcInfoString::RpcInfoString(string name, string dns, ALFRED* alfred): RpcInfo::RpcInfo(name, dns, alfred)
{
    type = DIM_TYPE::STRING;

    strcpy(noLink, "NO RPC LINK!"); //response from the callback when link is dow

    rpcInfo = new DimRpcInfo(name.c_str(), noLink);

    Print::PrintVerbose(string("RpcInfo ") + name + " registered!");
}

RpcInfoString::~RpcInfoString()
{
    delete rpcInfo;
}

void* RpcInfoString::Send(char *value)
{
    rpcInfo->setData(value);

    char* recValue = rpcInfo->getString();

    void* result = (void*)Execution((void*)recValue);

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
        //Print::PrintWarning(string("RpcInfo ") + Name() + " not valid data!");
        return NULL;
    }

    return result;
}

/*----------------------------------------------------------------------------------------------*/

RpcInfoData::RpcInfoData(string name, string dns, ALFRED* alfred, size_t size): RpcInfo::RpcInfo(name, dns, alfred)
{
    type = DIM_TYPE::DATA;
    this->size = size;
    rpcInfo = new DimRpcInfo(name.c_str(), (void*)NULL, (int)0);

    Print::PrintVerbose(string("RpcInfo ") + name + " registered!");
}

RpcInfoData::~RpcInfoData()
{
    delete rpcInfo;
}

void* RpcInfoData::Send(void* value)
{
    if (!value)
    {
        //Print::PrintWarning(string("RpcInfo ") + Name() + " not valid data!");
        return NULL;
    }

    rpcInfo->setData(value, size);

    void* recValue = rpcInfo->getData();

    if (!recValue)
    {
        //Print::PrintWarning(string("RpcInfo ") + Name() + " not valid data!");
        return NULL;
    }

    void* result = (void*)Execution((void*)&recValue);

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
        //Print::PrintWarning(string("Rpc ") + Name() + " not valid data!");
        return NULL;
    }

    return result;
}
