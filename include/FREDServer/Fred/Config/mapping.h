#ifndef MAPPING_H
#define MAPPING_H

#include <string>
#include <vector>

using namespace std;

class Mapping
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

private:
    vector<Unit> units;

    void processUnit(string& left, string& right);
};

#endif // MAPPING_H
