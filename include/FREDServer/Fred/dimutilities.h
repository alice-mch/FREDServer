#ifndef DIM_UTILITIES_H
#define DIM_UTILITIES_H

#include <vector>
#include <string>

using namespace std;

class DimUtilities
{
public:
    static vector<vector <string> > retrieveServers();
    static vector<vector <string> > retrieveServices(string server);

    static bool dimDnsIsUp(string dns);
    static bool alfIsUp(string alfId);
    static bool serviceIsUp(string server, string service);

    static void checkServices(vector<string> services);
};

#endif // DIM_UTILITIES_H
