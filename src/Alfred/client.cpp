#include "Alfred/client.h"
#include "Alfred/print.h"
#include "Alfred/alfred.h"

vector<string> Client::names;

bool Client::AlreadyRegistered(const string& name)
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

void Client::RemoveElement(const string& name)
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

Client::Client(string name, ALFRED* alfred)
{
	this->name = name;
	type = DIM_TYPE::NONE;

	if (!alfred)
	{
        Print::PrintError(string("ALFRED for client ") + name + " not defined!");
		exit(EXIT_FAILURE);	
	}

	this->alfred = alfred;

	if (AlreadyRegistered(name))
	{
        Print::PrintError(string("Client ") + name + " already registered!");
		exit(EXIT_FAILURE);
	}
	else
	{
		names.push_back(name);
	}
}

Client::~Client()
{
	RemoveElement(Name());
}

void Client::Send(void* value)
{
	switch (type)
	{
		case DIM_TYPE::INT:
			((ClientInt*)this)->Send(*(int*)value);
			break;
		case DIM_TYPE::FLOAT:
			((ClientFloat*)this)->Send(*(float*)value);
			break;
        case DIM_TYPE::STRING:
            ((ClientString*)this)->Send((const char*)value);
            break;
		case DIM_TYPE::DATA:
			((ClientData*)this)->Send(value, size);
			break;
		default:
            Print::PrintError("Invalid type of client!");
			exit(EXIT_FAILURE);
	}
}

DIM_TYPE Client::Type()
{
	return type;
}

string Client::Name()
{
	return name;
}

ALFRED* Client::Parent()
{
	return alfred;
}

/*----------------------------------------------------------------------------------------------*/

ClientInt::ClientInt(string name, ALFRED* alfred): Client::Client(name, alfred)
{
	size = 4;
	type = DIM_TYPE::INT;

    Print::PrintVerbose(string("Client ") + name + " registered!");
}

ClientInt::~ClientInt()
{

}

void ClientInt::Send(int value)
{
	DimClient::sendCommand(name.c_str(), value);
}

/*----------------------------------------------------------------------------------------------*/

ClientFloat::ClientFloat(string name, ALFRED* alfred): Client::Client(name, alfred)
{
	size = 4;
	type = DIM_TYPE::FLOAT;

    Print::PrintVerbose(string("Client ") + name + " registered!");
}

ClientFloat::~ClientFloat()
{

}

void ClientFloat::Send(float value)
{
	DimClient::sendCommand(name.c_str(), value);
}

/*----------------------------------------------------------------------------------------------*/

ClientString::ClientString(string name, ALFRED* alfred): Client::Client(name, alfred)
{
    type = DIM_TYPE::STRING;

    Print::PrintVerbose(string("Client ") + name + " registered!");
}

ClientString::~ClientString()
{

}

void ClientString::Send(const char* value)
{
    DimClient::sendCommand(name.c_str(), value);
}

/*----------------------------------------------------------------------------------------------*/

ClientData::ClientData(string name, ALFRED* alfred, size_t size): Client::Client(name, alfred)
{
	this->size = size;
	type = DIM_TYPE::DATA;

    Print::PrintVerbose(string("Client ") + name + " registered!");
}

ClientData::~ClientData()
{
	
}

void ClientData::Send(void* value, size_t size)
{
	if (!value || !size)
	{
        Print::PrintWarning(string("Client ") + Name() + " not valid data!");
		return;
	}

	DimClient::sendCommand(name.c_str(), value, size);
}
