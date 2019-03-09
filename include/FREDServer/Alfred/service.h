#ifndef __SERVICE
#define __SERVICE

#include <cstdlib>
#include <string>
#include <vector>
#include <dim/dis.hxx>

#include "Alfred/types.h"

using namespace std;

class ALFRED;

class Service
{
protected:
	DimService *service;
	size_t size;
	void *value;

	DIM_TYPE type;
	string name;
	ALFRED* alfred;

	static vector<string> names;
	static bool AlreadyRegistered(const string& name);
	static void RemoveElement(const string& name);

	Service(string name, ALFRED* alfred);

public:
	virtual ~Service();
	
	void* GetValuePnt();
	size_t GetValueSize();
	
    void Update();
	void Update(const void* value);

	DIM_TYPE Type();
	string Name();
	ALFRED* Parent();
};

/*----------------------------------------------------------------------------------------------*/

class ServiceInt: public Service
{
public:
	ServiceInt(string name, ALFRED* alfred);
	~ServiceInt();
};

/*----------------------------------------------------------------------------------------------*/

class ServiceFloat: public Service
{
public:
	ServiceFloat(string name, ALFRED* alfred);
	~ServiceFloat();
};

/*----------------------------------------------------------------------------------------------*/

class ServiceString: public Service
{
public:
    ServiceString(string name, ALFRED* alfred);
    ~ServiceString();
};

/*----------------------------------------------------------------------------------------------*/

class ServiceData: public Service
{
public:
	ServiceData(string name, ALFRED* alfred, size_t size, string format);
	~ServiceData();
};

#endif
