#ifndef MAPIINTERFACE_H
#define MAPIINTERFACE_H

#include <string>

using namespace std;

class MapiInterface
{
public:
	bool returnError = false; //to publish answer on _ERR or _ANS channel
    virtual string processInputMessage(string input) = 0;
    virtual string processOutputMessage(string output) = 0;
};

#endif // MAPIINTERFACE_H
