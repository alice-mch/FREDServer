#include "Alfred/frontend.h"
#include "Alfred/service.h"
#include "Alfred/command.h"
#include "Alfred/client.h"
#include "Alfred/rpcinfo.h"
#include "Alfred/print.h"
#include "Alfred/alfred.h"

vector<string> Function::names;

bool Function::AlreadyRegistered(const string& name)
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

void Function::RemoveElement(const string& name)
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

Function::Function(string name, FrontEnd* frontend)
{
	serviceCallback = NULL;
	clientCallback = NULL;
    rpcinfoCallback = NULL;

	type = FNC_TYPE::BASE;
	this->name = name;

	if (!frontend)
	{
        Print::PrintError(string("FrontEnd for function ") + name + " not defined!");
		exit(EXIT_FAILURE);	
	}

	this->frontend = frontend;

	if (AlreadyRegistered(name))
	{
        Print::PrintError(string("Function ") + name + " already registered!");
		exit(EXIT_FAILURE);
	}
	else
	{
		names.push_back(name);
	}
}

Function::~Function()
{
	RemoveElement(Name());
}

void Function::ConnectService(Service* serviceCallback)
{
	this->serviceCallback = serviceCallback;
}

void Function::ConnectClient(Client* clientCallback)
{
	this->clientCallback = clientCallback;
}

void Function::ConnectRpcInfo(RpcInfo *rpcinfoCallback)
{
    this->rpcinfoCallback = rpcinfoCallback;
}

void Function::CallService(void* value)
{
	if (serviceCallback)
	{
		serviceCallback->Update(value);
	}
}

void Function::CallClient(void* value)
{
	if (clientCallback)
	{
		clientCallback->Send(value);
	}
}

void* Function::CallRpcInfo(void *value)
{
    if (rpcinfoCallback)
    {
        return rpcinfoCallback->Send(value);
    }

    return NULL;
}

void Function::CallService(string name, void* value)
{
	Parent()->Parent()->GetService(name)->Update(value);
}

void Function::CallClient(string name, void* value)
{
	Parent()->Parent()->GetClient(name)->Send(value);
}

void* Function::CallRpcInfo(string name, void *value)
{
    return Parent()->Parent()->GetRpcInfo(name)->Send(value);
}

FNC_TYPE Function::Type()
{
	return type;
}

string Function::Name()
{
	return name;
}

FrontEnd* Function::Parent()
{
	return frontend;
}

/*----------------------------------------------------------------------------------------------*/

FunctionTimed::FunctionTimed(string name, FrontEnd* frontend, uint32_t interval, void* argument): Function::Function(name, frontend)
{
	this->interval = interval;
	this->argument = argument;
	type = FNC_TYPE::TIME;

	if (pthread_create(&thread, NULL, &ThreadFnc, (void*)this))
	{
        Print::PrintError("Cannot create new thread!");
		exit(EXIT_FAILURE);
	}
}

FunctionTimed::~FunctionTimed()
{
	
}

void* FunctionTimed::ThreadFnc(void* pointer)
{
	FunctionTimed* object = (FunctionTimed*)pointer;

	while (1)	//todo exit handler
	{
		usleep(object->interval);

        void* result = object->Execution(object->argument);

		object->CallService(result);
		object->CallClient(result);
	}

	return NULL;
}

/*----------------------------------------------------------------------------------------------*/

FunctionShot::FunctionShot(string name, FrontEnd* frontend): Function::Function(name, frontend)
{
	type = FNC_TYPE::SHOT;
}

FunctionShot::~FunctionShot()
{
	
}

void *FunctionShot::Shot(void* argument)
{
	void* result = this->Execution(argument);
	
	CallService(result);
	CallClient(result);

    return result;
}

/*----------------------------------------------------------------------------------------------*/

FrontEnd::FrontEnd(ALFRED* alfred)
{
	if (!alfred)
	{
        Print::PrintError("ALFRED for FrontEnd not defined!");
		exit(EXIT_FAILURE);	
	}

	this->alfred = alfred;
}

FrontEnd::~FrontEnd()
{
	for (auto it = functions.begin(); it !=functions.end(); it++)
	{
		delete it->second;
	}
}

void FrontEnd::RegisterFunction(Function* function)
{
	if (function)
	{
		functions[function->Name()] = function;
        Print::PrintVerbose(string("Function ") + function->Name() + " registered!");
	}
}

Function* FrontEnd::GetFunction(string name)
{
	if (functions.count(name))
	{
		return functions[name];
	}
	else
	{
        Print::PrintError(string("No FrontEnd function ") + name + " exists!");
		exit(EXIT_FAILURE);
	}
}

ALFRED* FrontEnd::Parent()
{
	return alfred;
}
