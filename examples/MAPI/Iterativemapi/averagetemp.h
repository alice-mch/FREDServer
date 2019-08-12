#include "Fred/Mapi/mapi.h"
#include "Fred/Mapi/iterativemapi.h"

class Averagetemp: public Iterativemapi
{
public:
    Averagetemp();

    string processInputMessage(string input);
    string processOutputMessage(string output);

    int counter;
    vector<int> temperatures;
    string sequence;
};
