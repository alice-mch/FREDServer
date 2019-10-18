#include "Alfred/info.h"
#include "Alfred/service.h"
#include "Alfred/frontend.h"
#include "Alfred/client.h"
#include "Alfred/rpcinfo.h"
#include "Alfred/print.h"
#include "Alfred/alfred.h"

vector<string> Info::names;

bool Info::AlreadyRegistered(const string& name)
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

void Info::RemoveElement(const string& name)
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


Info::Info(string name, ALFRED* alfred)
{
	serviceCallback = NULL;
	functionCallback = NULL;
	clientCallback = NULL;
    rpcinfoCallback = NULL;

	type = DIM_TYPE::NONE;
	this->name = name;

	if (!alfred)
	{
        Print::PrintError(string("ALFRED for info ") + name + " not defined!");
		exit(EXIT_FAILURE);	
	}

	this->alfred = alfred;

	if (AlreadyRegistered(name))
	{
        Print::PrintError(string("Info ") + name + " already registered!");
		exit(EXIT_FAILURE);
	}
	else
	{
		names.push_back(name);
	}
}

Info::~Info()
{
	RemoveElement(Name());
}

const void* Info::Execution(void* value)
{
	return value;
}

void Info::ConnectService(Service* serviceCallback)
{
	this->serviceCallback = serviceCallback;
}

void Info::ConnectFunction(FunctionShot* functionCallback)
{
	this->functionCallback = functionCallback;
}

void Info::ConnectClient(Client* clientCallback)
{
	this->clientCallback = clientCallback;
}

void Info::ConnectRpcInfo(RpcInfo *rpcinfoCallback)
{
    this->rpcinfoCallback = rpcinfoCallback;
}

void Info::CallService(string name, void* value)
{
	Parent()->GetService(name)->Update(value);
}

void Info::CallFunction(string name, void* value)
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

void Info::CallClient(string name, void* value)
{
	Parent()->GetClient(name)->Send(value);
}

void* Info::CallRpcInfo(string name, void *value)
{
    return Parent()->GetRpcInfo(name)->Send(value);
}

DIM_TYPE Info::Type()
{
	return type;
}

string Info::Name()
{
	return name;
}

ALFRED* Info::Parent()
{
	return alfred;
}

/*----------------------------------------------------------------------------------------------*/

InfoInt::InfoInt(string name, ALFRED* alfred): Info::Info(name, alfred), DimInfo::DimInfo(name.c_str(), -1)
{
	type = DIM_TYPE::INT;

    Print::PrintVerbose(string("Info ") + name + " registered!");
}

InfoInt::~InfoInt()
{

}

void InfoInt::infoHandler()
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

    if (rpcinfoCallback)
    {
        rpcinfoCallback->Send(result);
    }
}

/*----------------------------------------------------------------------------------------------*/

InfoFloat::InfoFloat(string name, ALFRED* alfred): Info::Info(name, alfred), DimInfo::DimInfo(name.c_str(), -1.0)
{
	type = DIM_TYPE::FLOAT;

    Print::PrintVerbose(string("Info ") + name + " registered!");
}

InfoFloat::~InfoFloat()
{

}

void InfoFloat::infoHandler()
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

    if (rpcinfoCallback)
    {
        rpcinfoCallback->Send(result);
    }
}

/*----------------------------------------------------------------------------------------------*/

InfoString::InfoString(string name, ALFRED* alfred): Info::Info(name, alfred), DimInfo::DimInfo(name.c_str(), noLink)
{
    type = DIM_TYPE::STRING;
    noLink[0] = '\0';

    Print::PrintVerbose(string("Info ") + name + " registered!");
}

InfoString::~InfoString()
{

}

void InfoString::infoHandler()
{
    value = getString();

    void* result = (void*)Execution((void*)value.c_str());

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

    if (rpcinfoCallback)
    {
        rpcinfoCallback->Send(result);
    }
}

/*----------------------------------------------------------------------------------------------*/

InfoData::InfoData(string name, ALFRED* alfred): Info::Info(name, alfred), DimInfo::DimInfo(name.c_str(), (void*)NULL, 0)
{
	type = DIM_TYPE::DATA;

    Print::PrintVerbose(string("Info ") + name + " registered!");
}

InfoData::~InfoData()
{
	
}

void InfoData::infoHandler()
{
	value = getData();

	if (!value)
	{
        Print::PrintWarning(string("Info ") + Name() + " not valid data!");
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

    if (rpcinfoCallback)
    {
        rpcinfoCallback->Send(result);
    }
}
