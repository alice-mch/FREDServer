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

bool Fred::terminate = false;

/*
 * Fred constructor
 */
Fred::Fred(bool parseOnly, string fredName, string dnsName, string mainDirectory): ALFRED::ALFRED(fredName, dnsName), alfClients(this), fredTopics(this)
{
    signal(SIGINT, &termFred);

    this->fredDns = dnsName;

    Print::PrintInfo("Parsing started.");
    try //parsing
    {
        Parser parser(mainDirectory);
        sections = parser.parseSections();

        if(parser.badFiles)
        {
            Print::PrintError("Parser discovered errors! (See output above)");
            exit(EXIT_FAILURE);
        }
    }
    catch (exception& e)
    {
        Print::PrintError("Error occured during parsing configuration files!");
        exit(EXIT_FAILURE);
    }

    if (parseOnly)
    {
        Print::PrintInfo("Parsing completed! No problems discovered.");
        exit(EXIT_SUCCESS);
    }

    Print::PrintInfo("Parsing Completed. Starting FRED.");
    generateAlfs();
    generateTopics();         
    Print::PrintInfo("FRED running.");
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
        Print::PrintError("Cannot load FRED config file!");
        exit(-1);
    }

    Print::PrintError("Invalid config file!");
    exit(-1);
}

void Fred::termFred(int)
{
    Print::PrintWarning("Closing FRED!");
    Fred::terminate = true;
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
    namespace po = boost::program_options;
    po::options_description description("FRED options");
    description.add_options()
    ("help, h", "Print::Print help message")
    ("verbose, v", "Verbose output")
    ("parser, p", "Parse config files then exit")
    ("log, l", po::value<string>(),"Log to file <file>");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, description), vm);
        if (vm.count("help"))
        {
            cout << description << endl; //Print::Print help menu
            exit(EXIT_SUCCESS);
        }
        if (vm.count("log"))
        {
            string logFilePath = vm["log"].as<string>();
            ofstream logFile;

            logFile.open(logFilePath, ios_base::app);
            if (logFile)
            {
                Print::PrintInfo("FRED launched, logging to " + logFilePath); //inform user
                Print::setLogFile(logFilePath);
                Print::PrintInfo("FRED launched, logging to " + logFilePath); //inform via log file
            }
            else
            {
                Print::PrintError("FRED launched, log file " + logFilePath + " is not writable, falling back to standard output!");
            }
            logFile.close();
        }
        else
        {
            Print::PrintInfo("FRED launched!");
        }
        if (vm.count("verbose"))
        {
            Print::setVerbose(true);
            Print::PrintWarning("FRED is verbose!");
        }
        if (vm.count("parser"))
        {
            Print::PrintWarning("Parse only mode!");
            return true;
        }
        po::notify(vm);
    }
    catch (po::error& e)
    {
        Print::PrintError(e.what());
        cerr << description << endl; //Print::Print help menu
        exit(EXIT_FAILURE);
    }

    return false;
}

void Fred::Start()
{
    StartOnce();

    while (!Fred::terminate)
    {
        usleep(100000);
    }
}
