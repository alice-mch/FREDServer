#include "Fred/Config/location.h"
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

    vector<string> paths = Utility::splitString(right, ",");
    for (size_t i = 0; i < paths.size(); i++)
    {
        vector<string> path = Utility::splitString(paths[i], "/");
        if (path.size() == 3)
        {
            Unit::Alf alf;
            alf.alfId = path[0]; //ALF_x
            alf.serialId = stoi(path[1].substr(path[1].find("_") + 1)); //SERIAL_x
            alf.linkId = stoi(path[2].substr(path[2].find("_") + 1)); //LINK_x

            if (path[0].find("ALF") == 0)
            {
                processLocation(alf.alfId, alf.serialId, alf.linkId);
                unit.alfs.first = alf;
            }
            else if (path[0].find("CANALF") == 0)
            {
                processLocation(alf.alfId, alf.serialId, alf.linkId);
                unit.alfs.second = alf;
            }
        }
    }

    units.push_back(unit);
}

void Mapping::processLocation(string alfId, int32_t serialId, int32_t linkId)
{
    if (!alfs.count(alfId)) //new ALF
    {
        AlfEntry::SerialEntry serialEntry;
        serialEntry.id = serialId;
        serialEntry.links.push_back(linkId);

        AlfEntry NewAlfEntry;
        NewAlfEntry.id = alfId;
        NewAlfEntry.serials[serialId] = serialEntry;

        alfs[alfId] = NewAlfEntry;
    }
    else //already existing ALF
    {
        if (!alfs[alfId].serials.count(serialId)) //new serial
        {
            AlfEntry::SerialEntry NewSerialEntry;
            NewSerialEntry.id = serialId;
            NewSerialEntry.links.push_back(linkId);

            alfs[alfId].serials[serialId] = NewSerialEntry;
        }
        else //already existing serial
        {
            alfs[alfId].serials[serialId].links.push_back(linkId);
        }
    }
}

vector<Mapping::Unit>& Mapping::getUnits()
{
    return units;
}

map<string, Location::AlfEntry>& Mapping::alfList()
{
    return alfs;
}
