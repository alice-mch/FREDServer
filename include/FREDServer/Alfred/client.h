#ifndef __CLIENT
#define __CLIENT

#include <cstdlib>
#include <string>
#include <vector>
#include <dim/dic.hxx>

#include "Alfred/types.h"

using namespace std;

class ALFRED;

class Client
{
protected:
	DIM_TYPE type;
	ALFRED* alfred;

	string name;
	size_t size;

	static vector<string> names;
	static bool AlreadyRegistered(const string& name);
	static void RemoveElement(const string& name);

	Client(string name, ALFRED* alfred);

public:
	virtual ~Client();
	
	void Send(void* value);

	DIM_TYPE Type();
	string Name();
	ALFRED* Parent();
};

/*----------------------------------------------------------------------------------------------*/

class ClientInt: public Client
{
public:
	ClientInt(string name, ALFRED* alfred);
	~ClientInt();

	void Send(int value);
};

/*----------------------------------------------------------------------------------------------*/

class ClientFloat: public Client
{
public:
	ClientFloat(string name, ALFRED* alfred);
	~ClientFloat();
	
	void Send(float value);
};

/*----------------------------------------------------------------------------------------------*/

class ClientString: public Client
{
public:
    ClientString(string name, ALFRED* alfred);
    ~ClientString();

    void Send(const char* value);
};

/*----------------------------------------------------------------------------------------------*/

class ClientData: public Client
{
public:
	ClientData(string name, ALFRED* alfred, size_t size);
	~ClientData();
	
	void Send(void* value, size_t size);
};

#endif
