#ifndef __FRONTEND
#define __FRONTEND

#include <string>
#include <cstdlib>
#include <pthread.h>
#include <map>
#include <vector>

#include "Alfred/types.h"

using namespace std;

class FrontEnd;
class Service;
class Client;
class RpcInfo;
class ALFRED;

class Function
{
protected:
	static vector<string> names;
	static bool AlreadyRegistered(const string& name);
	static void RemoveElement(const string& name);

	FNC_TYPE type;
	string name;
	FrontEnd* frontend;

	Service* serviceCallback;
	Client* clientCallback;
    RpcInfo* rpcinfoCallback;

	void CallService(void* value);
	void CallClient(void* value);
    void* CallRpcInfo(void* value);

public:
	Function(string name, FrontEnd* frontend);
	virtual ~Function();
	virtual void* Execution(void* value) = 0;

	void ConnectService(Service* serviceCallback);
	void ConnectClient(Client* clientCallback);
    void ConnectRpcInfo(RpcInfo* rpcinfoCallback);

	void CallService(string name, void* value);
	void CallClient(string name, void* value);
    void* CallRpcInfo(string name, void* value);

	FNC_TYPE Type();
	string Name();
	FrontEnd* Parent();
};

/*----------------------------------------------------------------------------------------------*/

class FunctionTimed: public Function
{
private:
	pthread_t thread;
	uint32_t interval;
	void* argument;

	static void* ThreadFnc(void* pointer);

public:
	FunctionTimed(string name, FrontEnd* frontend, uint32_t interval, void* argument = NULL);
	~FunctionTimed();
};

/*----------------------------------------------------------------------------------------------*/

class FunctionShot: public Function
{
public:
	FunctionShot(string name, FrontEnd* frontend);
	~FunctionShot();

    void* Shot(void* argument);
};

/*----------------------------------------------------------------------------------------------*/

class FrontEnd
{
private:
	map<string, Function*> functions;
	ALFRED* alfred;

public:
	FrontEnd(ALFRED* alfred);
	~FrontEnd();

	void RegisterFunction(Function* function);
	Function* GetFunction(string name);

	ALFRED* Parent();
};

#endif
