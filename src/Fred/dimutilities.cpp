#include <cstdio>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <cctype>
#include <algorithm>
#include "Alfred/print.h"
#include "Fred/dimutilities.h"
#include <dim/dic.hxx>

/*
 * Retrieve DIM server list from DIM DNS
 */
vector<vector <string> > DimUtilities::retrieveServers()
{
    DimBrowser serverBrowser;
    serverBrowser.getServers();

    vector<vector <string> > servers;
    char *name, *node;
    int pid;

    while (serverBrowser.getNextServer(name, node, pid))
    {
        vector <string> server;

        server.push_back(name);
        server.push_back(node);
        server.push_back(to_string(pid));
        
        servers.push_back(server);
    }

    return servers;
}

/*
 * Retrieve DIM service list from the choosen server
 */
vector<vector <string> > DimUtilities::retrieveServices(string server)
{
    DimBrowser serviceBrowser;
    vector<vector <string> > services;
    char *name, *node;

    serviceBrowser.getServerServices(const_cast<char*>(server.c_str()));

    while (serviceBrowser.getNextServerService(name, node))
    {
        vector <string> service;

        service.push_back(name);
        service.push_back(node);

        services.push_back(service);
    }

    return services;
}

/*
 * Check if the choosen DIM DNS is running
 */
bool DimUtilities::dimDnsIsUp(string dns)
{
	DimBrowser serverBrowser;
	return (serverBrowser.getServers()) ? true : false;
}

/*
 * Check if the choosen DIM server (in this case ALFServer) is up
 */
bool DimUtilities::alfIsUp(string alfId)
{
	vector<vector <string> > servers = DimUtilities::retrieveServers();
	vector <string> names;

	for (size_t i = 0; i < servers.size(); i++)
	{
		names.push_back(servers[i][0]);
	}

	return (find(names.begin(), names.end(), alfId) != names.end()) ? true : false;
}

/*
 * Check if the choosen DIM service from the choosen server is up
 */
bool DimUtilities::serviceIsUp(string server, string service)
{
	vector<vector <string> > services = retrieveServices(server);

	vector <string> names;
	for (size_t i = 0; i < services.size(); i++)
	{
		names.push_back(services[i][0]);
	}

	return (find(names.begin(), names.end(), service) != names.end()) ? true : false;
}

/*
 * Check if the DIM services used in the current FRED configuration are up
 */
void DimUtilities::checkServices(vector<string> services)
{
	map<string, vector<string>> alfMap;

	for (size_t i = 0; i < services.size(); i ++)
	{
		string service = services[i]; 
		string alfId = services[i].erase(services[i].find('/'));

		if (find(alfMap[alfId].begin(), alfMap[alfId].end(), service) == alfMap[alfId].end())
		{
			alfMap[alfId].push_back(service);
		}
	}

    for (auto it = alfMap.begin(); it != alfMap.end(); it++)
    {
    	string alfId = it->first;
		if (alfIsUp(alfId))
		{
	        for (size_t i = 0; i < it->second.size(); i++)
	        {
	        	string service = it->second[i];
	        	
	        	if (!serviceIsUp(alfId, service))
	        	{
					PrintWarning("Service " + service + " is not present on server " + alfId + "!");
	        	}
	        }
		}
		else
		{
			PrintWarning("ALFServer " + alfId + " is NOT UP!");
		}
    }
}
