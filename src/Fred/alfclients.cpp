#include "Fred/alfclients.h"
#include "Fred/alfrpcinfo.h"
#include "Fred/crualfrpcinfo.h"
#include "Fred/fred.h"

AlfClients::AlfClients(Fred *fred)
{
    this->fred = fred;
}

AlfClients::~AlfClients()
{
    for (auto it = queues.begin(); it != queues.end(); it++)
    {
        delete it->second;
    }
}

void AlfClients::registerAlf(Location::AlfEntry &entry)
{
    if (clients.count(entry.id) == 0)
    {
        clients[entry.id] = map<int32_t, map<int32_t, Nodes> >();
        queues[entry.id] = new Queue(entry.id, this->fred);
    }

    for (auto serial = entry.serials.begin(); serial != entry.serials.end(); serial++)
    {
        if (clients[entry.id].count(serial->first) == 0)
        {
            clients[entry.id][serial->first] = map<int32_t, Nodes>();

            if (entry.id.find("ALF") == 0)
            {
                this->fred->RegisterRpcInfo(new CruAlfRpcInfo(entry.id + "/SERIAL_" + to_string(serial->first) + "/LINK_0/REGISTER_WRITE", this->fred, CruAlfRpcInfo::WRITE));
                this->fred->RegisterRpcInfo(new CruAlfRpcInfo(entry.id + "/SERIAL_" + to_string(serial->first) + "/LINK_0/REGISTER_READ", this->fred, CruAlfRpcInfo::READ));
            }
        }

        for (size_t link = 0; link < serial->second.links.size(); link++)
        {
            if (clients[entry.id][serial->first].count(serial->second.links[link]) == 0)
            {
                //cout << entry.id << " " << serial->first << " " << serial->second.links[link] << "\n";
                clients[entry.id][serial->first][serial->second.links[link]] = createAlfInfo(entry.id, serial->first, serial->second.links[link]);
            }
        }
    }
}

AlfClients::Nodes AlfClients::createAlfInfo(string id, int32_t serial, int32_t link)
{
    Nodes nodes = { .sca = NULL, .swt = NULL, .ic = NULL };

    nodes.swt = new AlfRpcInfo(id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/SWT_SEQUENCE", " ", this->fred);
    this->fred->RegisterRpcInfo(nodes.swt);

    if (id.find("ALF") == 0)
    {
        nodes.sca = new AlfRpcInfo(id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/SCA_SEQUENCE", " ", this->fred);
        this->fred->RegisterRpcInfo(nodes.sca);
        nodes.ic = new AlfRpcInfo(id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/IC_SEQUENCE", " ", this->fred);
        this->fred->RegisterRpcInfo(nodes.ic);
    }

    return nodes;
}

AlfRpcInfo* AlfClients::getAlfNode(string alf, int32_t serial, int32_t link, Instructions::Type type)
{
    Nodes& nodes = clients[alf][serial][link];

    switch (type)
    {
        case Instructions::Type::SCA:
            return nodes.sca;
        case Instructions::Type::SWT:
            return nodes.swt;
        case Instructions::Type::IC:
            return nodes.ic;
    }

    return NULL;
}

Queue* AlfClients::getAlfQueue(string alf)
{
    return queues[alf];
}
