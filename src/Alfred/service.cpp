#include "Alfred/service.h"
#include "Alfred/print.h"
#include "Alfred/alfred.h"

vector<string> Service::names;

bool Service::AlreadyRegistered(const string& name)
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

void Service::RemoveElement(const string& name)
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

Service::Service(string name, ALFRED* alfred)
{
	value = NULL;
	service = NULL;
	this->name = name;

	type = DIM_TYPE::NONE;

	if (!alfred)
	{
        Print::PrintError(string("ALFRED for service ") + name + " not defined!");
		exit(EXIT_FAILURE);	
	}

	this->alfred = alfred;

	if (AlreadyRegistered(name))
	{
        Print::PrintError(string("Service ") + name + " already registered!");
		exit(EXIT_FAILURE);
	}
	else
	{
		names.push_back(name);
	}
}

Service::~Service()
{
	RemoveElement(Name());
}

void* Service::GetValuePnt()
{
	if (!value)
	{
        Print::PrintError("Accessing invalid pointer!");
		exit(EXIT_FAILURE);
	}

	return value;
}

size_t Service::GetValueSize()
{
	return size;
}

void Service::Update()
{
	if (!service)
	{
        Print::PrintError("Accessing invalid pointer!");
		exit(EXIT_FAILURE);
	}

	service->updateService();
}

void Service::Update(const void* value)
{
    if (type == DIM_TYPE::STRING)
    {
        memcpy(this->value, value, strlen((char*)value) + 1);
        service->updateService((char*)this->value);
        return;
    }

	if (!this->value || !value)
	{
        Print::PrintWarning(string("Service ") + Name() + " not valid data!");
		return;
	}

	memcpy(this->value, value, size);
	Update();
}

DIM_TYPE Service::Type()
{
	return type;
}

string Service::Name()
{
	return name;
}

ALFRED* Service::Parent()
{
	return alfred;
}

/*----------------------------------------------------------------------------------------------*/

ServiceInt::ServiceInt(string name, ALFRED* alfred): Service::Service(name, alfred)
{
	size = sizeof(int);
	value = (void*)new int;
	service = new DimService(name.c_str(), *(int*)value);
	type = DIM_TYPE::INT;

    Print::PrintVerbose(string("Service ") + name + " registered!");
}

ServiceInt::~ServiceInt()
{
	delete service;
	delete (int*)value;
}

/*----------------------------------------------------------------------------------------------*/

ServiceFloat::ServiceFloat(string name, ALFRED* alfred): Service::Service(name, alfred)
{
	size = sizeof(float);
	value = (void*)new float;
	service = new DimService(name.c_str(), *(float*)value);
	type = DIM_TYPE::FLOAT;

    Print::PrintVerbose(string("Service ") + name + " registered!");
}

ServiceFloat::~ServiceFloat()
{
	delete service;
	delete (float*)value;
}

/*----------------------------------------------------------------------------------------------*/

ServiceString::ServiceString(string name, ALFRED* alfred): Service::Service(name, alfred)
{
    value = (void*)new char[102400];
    service = new DimService(name.c_str(), (char*)value);
    type = DIM_TYPE::STRING;

    Print::PrintVerbose(string("Service ") + name + " registered!");
}

ServiceString::~ServiceString()
{
    delete service;
    delete (char*)value;
}

/*----------------------------------------------------------------------------------------------*/

ServiceData::ServiceData(string name, ALFRED* alfred, size_t size, string format): Service::Service(name, alfred)
{
	this->size = size;
	value = (void*)new uint8_t[size];
	service = new DimService(name.c_str(), format.c_str(), value, size);
	type = DIM_TYPE::DATA;

    Print::PrintVerbose(string("Service ") + name + " registered!");
}

ServiceData::~ServiceData()
{
	delete service;
	delete[] (uint8_t*)value;
}
