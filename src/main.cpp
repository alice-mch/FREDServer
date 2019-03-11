#include <stdio.h>
#include "Fred/fred.h"

#include "Mapi/mapiexample.h"

int main()
{
    pair<string, string> config = Fred::readConfigFile();
    Fred fred(config.first, config.second, "./sections");

    MapiExample mapiExample;
    fred.registerMapiObject("FRED1/MAPI_EXAMPLE/LOOP0/TEST", &mapiExample);

    fred.Start();

    return 0;
}
