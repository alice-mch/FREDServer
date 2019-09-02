#include "Fred/Mapi/mapi.h"
#include "Fred/Mapi/mapigroup.h"

class Testgroup: public Mapigroup
{
public:
    Testgroup();

    string processInputMessage(string input);
    string processOutputMessage(string output);
};
