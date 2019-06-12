#include "Fred/fred.h"
#include <signal.h>
#include <exception>
#include "Alfred/print.h"
#include "Parser/parser.h"
#include "Fred/Config/mapping.h"
#include "Fred/cruregistercommand.h"
#include "Fred/fredMode.h"

Fred::Fred(string fredName, string dnsName, string mainDirectory): ALFRED::ALFRED(fredName, dnsName), alfClients(this), fredTopics(this)
{
    extern int fredMode;
    signal(SIGINT, &terminate);

    this->fredDns = dnsName;

    PrintInfo("Parsing started.");
    try //parsing
    {
        Parser parser(mainDirectory);
        sections = parser.parseSections();

        if(parser.badFiles)
        {
            PrintError("Parser discovered errors! (See output above)");
            exit(EXIT_FAILURE);
        }
    }
    catch (exception& e)
    {
        PrintError("Error occured during parsing configuration files!");
        exit(EXIT_FAILURE);
    }

    if (fredMode == PARSER)
    {
        PrintInfo("Parsing completed! No problems discovered.");
        exit(EXIT_SUCCESS);
    }

    PrintInfo("Parsing Completed. Starting FRED.");
    generateAlfs();
    generateTopics();         
    PrintInfo("FRED running.");
}

pair<string, string> Fred::readConfigFile()
{
    try
    {
        vector<string> lines = Parser::readFile("fred.conf", "./config");

        string name, dns;
        for (size_t i = 0; i < lines.size(); i++)
        {
            string left = lines[i].substr(0, lines[i].find("="));
            string right = lines[i].substr(lines[i].find("=") + 1);

            if (left == "NAME") name = right;
            else if (left == "DNS") dns = right;
        }

        if (name != "" && dns != "")
        {
            return make_pair(name, dns);
        }
    }
    catch (exception& e)
    {
        PrintError("Cannot load FRED config file!");
        exit(-1);
    }

    PrintError("Invalid config file!");
    exit(-1);
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
        map<int32_t, Location::AlfEntry>& alfs = sections[i].mapping.alfList();
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

    RegisterCommand(new CruRegisterCommand(CruRegisterCommand::WRITE, this));
    RegisterCommand(new CruRegisterCommand(CruRegisterCommand::READ, this));
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
