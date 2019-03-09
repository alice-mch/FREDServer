#include "Fred/Config/location.h"
#include "Parser/parser.h"
#include "Parser/utility.h"

Location::Location(vector<string> data)
{
    vector<string> subsection = data;
    while (subsection.size() > 3)
    {
        string alfName;
        vector<string> rest;
        vector<string> alfSection = Parser::getSubsection(subsection, "{}", alfName, rest);

        if (alfName.find("ALF") != string::npos)
        {
            processAlf(alfName, alfSection);
        }

        subsection = rest;
    }
}

void Location::processAlf(string name, vector<string> data)
{
    AlfEntry alfEntry;
    alfEntry.id = stoi(name.substr(3)); //get id from ALFx name

    size_t idx = 0;
    while (idx < data.size())
    {
        if (data[idx].find("DNS") != string::npos)
        {
            alfEntry.dns = data[idx].substr(data[idx].find("=") + 1);
        }
        else if (data[idx].find("SERIALS") != string::npos)
        {
            do
            {
                idx++;

                if (data[idx].find("SERIAL_") != string::npos)
                {
                    AlfEntry::SerialEntry serialEntry = processSerial(data[idx]);
                    alfEntry.serials[serialEntry.id] = serialEntry;
                }
            }
            while (data[idx+1].find("}") == string::npos);
        }

        idx++;
    }

    alfs[alfEntry.id] = alfEntry;
}

Location::AlfEntry::SerialEntry Location::processSerial(string serial)
{
    AlfEntry::SerialEntry serialEntry;
    serialEntry.id = stoi(serial.substr(7, serial.find("=") - 7));

    vector<string> splitLinks = Utility::splitString(serial.substr(serial.find("[") + 1, serial.find("]") - serial.find("[") - 1), ",");
    for (size_t i = 0; i < splitLinks.size(); i++)
    {
        serialEntry.links.push_back(stoi(splitLinks[i]));
    }

    return serialEntry;
}

map<int32_t, Location::AlfEntry>& Location::getAlfs()
{
    return alfs;
}
