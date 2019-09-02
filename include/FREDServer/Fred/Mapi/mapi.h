#ifndef MAPI_H
#define MAPI_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

class Fred;
struct ChainTopic;

using namespace std;

class Mapi
{
protected:
	string name;
	Fred* fred;
	ChainTopic* thisMapi;

public:
	bool noReturn = false; //do not publish to wincc (for iterativemapi and mapigroup)
	bool noRpcRequest = false; //do not send the rpc request
	bool returnError = false; //publish answer on _ERR or _ANS channel
	virtual string processInputMessage(string input) = 0;
	virtual string processOutputMessage(string output) = 0;

	void getFred(Fred* fred);
	void getName(string name);
};

#endif // MAPI_H
