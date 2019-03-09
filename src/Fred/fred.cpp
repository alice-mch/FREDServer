#include "Fred/fred.h"
#include <signal.h>
#include <exception>
#include "Alfred/print.h"
#include "Parser/parser.h"
#include "Fred/Config/mapping.h"

Fred::Fred(string fredName, string dnsName, string mainDirectory): ALFRED::ALFRED(fredName, dnsName), alfClients(this), fredTopics(this)
{
    signal(SIGINT, &terminate);

    this->fredDns = dnsName;

    try
    {
        Parser parser(mainDirectory);
        sections = parser.parseSections();
    }
    catch (exception& e)
    {
        PrintError("Error occured during parsing configuration files!");
        exit(EXIT_FAILURE);
    }

    PrintInfo("Starting FRED!");

    generateAlfs();
    generateTopics();
}

void Fred::terminate(int)
{
    PrintWarning("Closing FRED!");
    exit(EXIT_SUCCESS);
}

void Fred::generateAlfs()
{
    for (size_t i = 0; i < sections.size(); i++)
    {
        map<int32_t, Location::AlfEntry>& alfs = sections[i].location.getAlfs();
        for (auto alf = alfs.begin(); alf != alfs.end(); alf++)
        {
            alfClients.registerAlf(alf->second);
        }
    }
}

void Fred::generateTopics()
{
    for (size_t i = 0; i < sections.size(); i++)
    {
        vector<Mapping::Unit>& units = sections[i].mapping.getUnits();
        for (auto unit = units.begin(); unit != units.end(); unit++)
        {
            fredTopics.registerUnit(sections[i].getName(), *unit, sections[i].instructions);
        }

        vector<Groups::Group>& groups = sections[i].groups.getGroups();
        for (auto group = groups.begin(); group != groups.end(); group++)
        {
            fredTopics.registerGroup(sections[i].getName(), *group);
        }
    }
}

AlfClients& Fred::getAlfClients()
{
    return alfClients;
}

FredTopics& Fred::getFredTopics()
{
    return fredTopics;
}

string Fred::getFredDns()
{
    return fredDns;
}

void Fred::registerMapiObject(string topic, MapiInterface* mapi)
{
    fredTopics.registerMapiObject(topic, mapi);
}
