#include "Fred/Config/mapping.h"
#include "Parser/utility.h"

Mapping::Mapping(vector<string> data)
{
    for (size_t i = 0; i < data.size(); i++)
    {
        if (data[i].find("=") != string::npos)
        {
            string left = data[i].substr(0, data[i].find("="));
            string right = data[i].substr(data[i].find("=") + 1);
            processUnit(left, right);
        }
    }
}

void Mapping::processUnit(string& left, string& right)
{
    Unit unit;

    string name = left.substr(0, left.find("["));
    unit.unitName = name;

    vector<string> ids = Utility::splitString(left.substr(left.find("[") + 1, left.find("]") - left.find("[") - 1), ",");
    for (size_t i = 0; i < ids.size(); i++)
    {
        if (ids[i] == "x") unit.unitIds.push_back(-1);
        else unit.unitIds.push_back(stoi(ids[i]));
    }

    vector<string> path = Utility::splitString(right, "/");
    if (path.size() == 3)
    {
        unit.alfId = stoi(path[0].substr(3)); //ALFx
        unit.serialId = stoi(path[1].substr(7)); //SERIAL_x
        unit.linkId = stoi(path[2].substr(5)); //LINK_x

        units.push_back(unit);
    }
}

vector<Mapping::Unit> &Mapping::getUnits()
{
    return units;
}
