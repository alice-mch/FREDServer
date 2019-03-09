#ifndef MAPIINTERFACE_H
#define MAPIINTERFACE_H

#include <string>

using namespace std;

class MapiInterface
{
public:
    virtual string processInputMessage(string input) = 0;
    virtual string processOutputMessage(string output) = 0;
};

#endif // MAPIINTERFACE_H
