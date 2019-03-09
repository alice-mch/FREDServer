#ifndef MAPIEXAMPLE_H
#define MAPIEXAMPLE_H

#include "Fred/mapiinterface.h"

class MapiExample: public MapiInterface
{
public:
    MapiExample();

    string processInputMessage(string input);
    string processOutputMessage(string output);
};

#endif // MAPIEXAMPLE_H
