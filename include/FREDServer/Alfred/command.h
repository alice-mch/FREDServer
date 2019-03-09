#ifndef __COMMAND
#define __COMMAND

#include <cstdlib>
#include <string>
#include <vector>
#include <dim/dis.hxx>

#include "Alfred/types.h"

using namespace std;

class Service;
class FunctionShot;
class Client;
class RpcInfo;
class ALFRED;

class Command
{
protected:
	static vector<string> names;
	static bool AlreadyRegistered(const string& name);
	static void RemoveElement(const string& name);

	DIM_TYPE type;
	string name;
	ALFRED* alfred;

	Command(string name, ALFRED* alfred);

	Service* serviceCallback;
	FunctionShot* functionCallback;
	Client* clientCallback;
    RpcInfo* rpcinfoCallback;

	virtual const void* Execution(void* value);

public:
	virtual ~Command();
	
	void ConnectService(Service* serviceCallback);
	void ConnectFunction(FunctionShot* functionCallback);
	void ConnectClient(Client* clientCallback);
    void ConnectRpcInfo(RpcInfo* rpcinfoCallback);

	void CallService(string name, void* value);
	void CallFunction(string name, void* value);
	void CallClient(string name, void* value);
    void* CallRpcInfo(string name, void* value);

	DIM_TYPE Type();
	string Name();
	ALFRED* Parent();
};

/*----------------------------------------------------------------------------------------------*/

class CommandInt: public Command, public DimCommand
{
private:
	int value;

	void commandHandler();

public:
	CommandInt(string name, ALFRED* alfred);
	~CommandInt();
};

/*----------------------------------------------------------------------------------------------*/

class CommandFloat: public Command, public DimCommand
{
private:
	float value;

	void commandHandler();

public:
	CommandFloat(string name, ALFRED* alfred);
	~CommandFloat();
};

/*----------------------------------------------------------------------------------------------*/

class CommandString: public Command, public DimCommand
{
private:
    string value;

    void commandHandler();

public:
    CommandString(string name, ALFRED* alfred);
    ~CommandString();
};

/*----------------------------------------------------------------------------------------------*/

class CommandData: public Command, public DimCommand
{
private:
	void* value;

	void commandHandler();

public:
	CommandData(string name, string format, ALFRED* alfred);
	~CommandData();
};

#endif
