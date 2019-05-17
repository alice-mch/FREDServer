#include "Fred/Config/location.h"
#include "Fred/Config/mapping.h"
#include "Parser/parser.h"
#include "Parser/utility.h"
#include <string>

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

        processLocation(unit.alfId, unit.serialId, unit.linkId);

        units.push_back(unit);
    }
}

void Mapping::processLocation(int32_t alfId, int32_t serialId, int32_t linkId)
{
    if (!alfs[alfId].id){ //new ALF

        AlfEntry alfEntry;
        alfEntry.id = alfId;
        
        AlfEntry::SerialEntry serialEntry;
        serialEntry.id = serialId;
        serialEntry.links.push_back(linkId);

        alfEntry.serials[serialEntry.id] = serialEntry;

        alfs[alfEntry.id] = alfEntry;
    }else //already existing ALF
    {
        if(!alfs[alfId].serials[serialId].links.size()) //new serial (without links)
        {
            AlfEntry::SerialEntry serialEntry;
            serialEntry.id = serialId;
            serialEntry.links.push_back(linkId);

            alfs[alfId].serials[serialId] = serialEntry;
        }else //already existing serial
        {
            alfs[alfId].serials[serialId].links.push_back(linkId);
        }
    }

}

vector<Mapping::Unit>& Mapping::getUnits()
{
    return units;
}

map<int32_t, Location::AlfEntry>& Mapping::alfList()
{
    return alfs;
}
