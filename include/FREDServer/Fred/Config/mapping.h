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

        string alfId;
        int32_t serialId, linkId;
    };

    Mapping(vector<string> data = vector<string>());
    vector<Unit>& getUnits();

    map<string, AlfEntry>& alfList();

private:
    vector<Unit> units;

    void processUnit(string& left, string& right);
    void processLocation(string alfId, int32_t serialId, int32_t linkId);
};

#endif // MAPPING_H
