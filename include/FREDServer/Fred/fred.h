#ifndef FRED_H
#define FRED_H

#include "Alfred/alfred.h"
#include "Fred/section.h"
#include "Fred/alfclients.h"
#include "Fred/fredtopics.h"
#include "Fred/Mapi/mapi.h"
#include "Fred/Mapi/iterativemapi.h"
#include "Fred/Mapi/mapigroup.h"

class Fred: public ALFRED
{
private:
    static void termFred(int);
    static bool terminate;
    void generateAlfs();
    void generateTopics();

    vector<Section> sections;
    AlfClients alfClients;
    FredTopics fredTopics;

    string fredDns;

public:
    Fred(bool parseOnly, string fredName, string fredDns, string mainDirectory);
    static pair<string, string> readConfigFile();
    static bool commandLineArguments(int argc, char** argv);

    AlfClients &getAlfClients();
    FredTopics &getFredTopics();

    string getFredDns();

    void registerMapiObject(string topic, Mapi* mapi);
    void Start();
};

#endif // FRED_H
