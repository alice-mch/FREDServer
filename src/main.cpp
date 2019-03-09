#include <stdio.h>
#include "Fred/fred.h"

#include "Mapi/mapiexample.h"

int main()
{
    Fred fred("FRED1", "aldcs012", "./sections");

    MapiExample mapiExample;
    fred.registerMapiObject("FRED1/MAPI_EXAMPLE/LOOP0/TEST", &mapiExample);

    fred.Start();

    return 0;
}
