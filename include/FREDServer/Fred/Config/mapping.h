#ifndef MAPPING_H
#define MAPPING_H

#include "Fred/Config/location.h"
#include <string>
#include <vector>

using namespace std;

class Mapping: public Location
{
public:

    struct Unit
    {
        string unitName;
        vector<int32_t> unitIds;

        int32_t alfId, serialId, linkId;
    };

    Mapping(vector<string> data = vector<string>());
    vector<Unit>& getUnits();

    map<int32_t, AlfEntry>& alfList();

private:
    vector<Unit> units;

    void processUnit(string& left, string& right);
    void processLocation(int32_t alfId, int32_t serialId, int32_t linkId);
};

#endif // MAPPING_H
