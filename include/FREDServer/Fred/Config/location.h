#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class Location
{
public:
    
    struct AlfEntry
    {
        struct SerialEntry
        {
            int32_t id; //SERIAL_ID
            vector<int32_t> links;
        };

        string id; //ALF_ID
        map<int32_t, SerialEntry> serials;
    };

protected:
    map<string, AlfEntry> alfs;
};

#endif // LOCATION_H
