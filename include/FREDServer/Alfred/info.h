#ifndef __INFO
#define __INFO

#include <cstdlib>
#include <string>
#include <vector>
#include <dim/dic.hxx>

#include "Alfred/types.h"

using namespace std;

class Service;
class FunctionShot;
class Client;
class RpcInfo;
class ALFRED;

class Info
{
protected:
	static vector<string> names;
	static void RemoveElement(const string& name);

	DIM_TYPE type;
	string name;
	ALFRED* alfred;

	Info(string name, ALFRED* alfred);

	Service* serviceCallback;
	FunctionShot* functionCallback;
	Client* clientCallback;
    RpcInfo* rpcinfoCallback;

	virtual const void* Execution(void* value);

public:
	virtual ~Info();
    static bool AlreadyRegistered(const string& name);
	
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

class InfoInt: public Info, public DimInfo
{
private:
	int value;

	void infoHandler();

public:
	InfoInt(string name, ALFRED* alfred);
	~InfoInt();
};

/*----------------------------------------------------------------------------------------------*/

class InfoFloat: public Info, public DimInfo
{
private:
	float value;

	void infoHandler();

public:
	InfoFloat(string name, ALFRED* alfred);
	~InfoFloat();
};

/*----------------------------------------------------------------------------------------------*/

class InfoString: public Info, public DimInfo
{
private:
    string value;
    char noLink[1];

    void infoHandler();

public:
    InfoString(string name, ALFRED* alfred);
    ~InfoString();
};

/*----------------------------------------------------------------------------------------------*/

class InfoData: public Info, public DimInfo
{
private:
	void* value;

	void infoHandler();

public:
	InfoData(string name, ALFRED* alfred);
	~InfoData();
};

#endif
