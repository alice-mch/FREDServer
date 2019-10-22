#include "Fred/fred.h"
#include <signal.h>
#include <fstream>
#include <exception>
#include "Alfred/print.h"
#include "Parser/parser.h"
#include "Fred/Config/mapping.h"
#include "Fred/cruregistercommand.h"
#include "Fred/Mapi/mapi.h"
#include "Fred/Mapi/iterativemapi.h"
#include "Fred/Mapi/mapigroup.h"
#include <boost/program_options.hpp>
#include "Fred/dimutilities.h"
#include "Fred/alfrpcinfo.h"

/*
 * Fred constructor
 */
Fred::Fred(bool parseOnly, string fredName, string dnsName, string mainDirectory): ALFRED::ALFRED(fredName, dnsName), alfClients(this), fredTopics(this)
{
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

    if (parseOnly)
    {
        PrintInfo("Parsing completed! No problems discovered.");
        exit(EXIT_SUCCESS);
    }

    PrintInfo("Parsing Completed. Starting FRED.");
    generateAlfs();
    generateTopics();
    checkAlfs();
    PrintInfo("FRED running.");
}

/*
 * Read FRED name and DIM DNS from fred.conf
 */
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
        map<string, Location::AlfEntry>& alfs = sections[i].mapping.alfList();
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

void Fred::checkAlfs()
{
    vector<string> services;

    map<string, ChainTopic> topicsMapi = fredTopics.getTopicsMap();
    for (auto topic = topicsMapi.begin(); topic != topicsMapi.end(); topic++)
    {
        pair <string, string> alfred;
        
        alfred.first = topic->second.name;
        alfred.second = topic->second.alfLink->getName();

        //cout << alfred.first << " -> " << alfred.second << endl; // print "topic -> alfLink"
        
        services.push_back(alfred.second);
    }

    DimUtilities::checkServices(services);
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

void Fred::registerMapiObject(string topic, Mapi* mapi)
{
    mapi->getFred(this);
    mapi->getName(topic);
    fredTopics.registerMapiObject(topic, mapi);
}

/*
 * Compute eventual command line arguments, return true if FRED is in parse only mode
 */
bool Fred::commandLineArguments(int argc, char** argv)
{
    extern bool verbose;
    extern bool logToFile;
    extern string logFilePath;

    namespace po = boost::program_options;
    po::options_description description("FRED options");
    description.add_options()
    ("help, h", "Print help message")
    ("verbose, v", "Verbose output")
    ("parser, p", "Parse config files then exit")
    ("log, l", po::value<string>(),"Log to file <file>");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, description), vm);
        if (vm.count("help"))
        {
            cout << description << endl; //print help menu
            exit(EXIT_SUCCESS);
        }
        if (vm.count("log"))
        {
            logFilePath = vm["log"].as<string>();
            ofstream logFile;

            logFile.open(logFilePath, ios_base::app);
            if (logFile)
            {
                PrintInfo("FRED launched, logging to " + logFilePath); //inform user
                logToFile = true;
                PrintInfo("FRED launched, logging to " + logFilePath); //inform via log file
            }
            else
            {
                PrintError("FRED launched, log file " + logFilePath + " is not writable, falling back to standard output!");
            }
            logFile.close();
        }
        else
        {
            PrintInfo("FRED launched!");
        }
        if (vm.count("verbose"))
        {
            verbose = true;
            PrintWarning("FRED is verbose!");
        }
        if (vm.count("parser"))
        {
            PrintWarning("Parse only mode!");
            return true;
        }
        po::notify(vm);
    }
    catch (po::error& e)
    {
        PrintError(e.what());
        cerr << description << endl; //print help menu
        exit(EXIT_FAILURE);
    }

    return false;
}
