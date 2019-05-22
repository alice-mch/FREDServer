#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class Location
{
public:
    Location(vector<string> data = vector<string>());

    struct AlfEntry
    {
        struct SerialEntry
        {
            int32_t id;
            vector<int32_t> links;
        };

        string dns;
        int32_t id;
        map<int32_t, SerialEntry> serials;
    };

    map<int32_t, AlfEntry>& getAlfs();

private:
    map<int32_t, AlfEntry> alfs;

    void processAlf(string name, vector<string> data);
    AlfEntry::SerialEntry processSerial(string serial);
};

#endif // LOCATION_H
