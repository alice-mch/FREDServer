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
        //dns[entry.id] = entry.dns;
    }

    for (auto serial = entry.serials.begin(); serial != entry.serials.end(); serial++)
    {
        if (clients[entry.id].count(serial->first) == 0)
        {
            clients[entry.id][serial->first] = map<int32_t, Nodes>();

            this->fred->RegisterRpcInfo(new CruAlfRpcInfo("ALF_" + entry.id + "/SERIAL_" + to_string(serial->first) + "/LINK_0/REGISTER_WRITE", this->fred, CruAlfRpcInfo::WRITE));
            this->fred->RegisterRpcInfo(new CruAlfRpcInfo("ALF_" + entry.id + "/SERIAL_" + to_string(serial->first) + "/LINK_0/REGISTER_READ", this->fred, CruAlfRpcInfo::READ));
        }

        for (size_t link = 0; link < serial->second.links.size(); link++)
        {
            if (clients[entry.id][serial->first].count(serial->second.links[link]) == 0)
            {
                clients[entry.id][serial->first][serial->second.links[link]] = createAlfInfo(entry.id, serial->first, serial->second.links[link]);
            }
        }
    }
}

AlfClients::Nodes AlfClients::createAlfInfo(string id, int32_t serial, int32_t link)
{
    //multiple DNS is deprecated
    //setenv("DIM_DNS_NODE", dns.c_str(), 1);

    Nodes nodes;

    nodes.sca = new AlfRpcInfo("ALF_" + id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/SCA_SEQUENCE", " ", this->fred);
    this->fred->RegisterRpcInfo(nodes.sca);
    nodes.swt = new AlfRpcInfo("ALF_" + id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/SWT_SEQUENCE", " ", this->fred);
    this->fred->RegisterRpcInfo(nodes.swt);
    nodes.ic = new AlfRpcInfo("ALF_" + id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/IC_SEQUENCE", " ", this->fred);
    this->fred->RegisterRpcInfo(nodes.ic);

    //this->fred->RegisterRpcInfo("ALF_" + id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/PUBLISH_SCA_SEQUENCE_START", " ",  DIM_TYPE::STRING);
    //this->fred->RegisterRpcInfo("ALF_" + id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/PUBLISH_SCA_SEQUENCE_STOP", " ", DIM_TYPE::STRING);
    //this->fred->RegisterRpcInfo("ALF_" + id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/PUBLISH_SWT_SEQUENCE_START", " ", DIM_TYPE::STRING);
    //this->fred->RegisterRpcInfo("ALF_" + id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/PUBLISH_SWT_SEQUENCE_STOP", " ", DIM_TYPE::STRING);
    //this->fred->RegisterRpcInfo("ALF_" + id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/PUBLISH_IC_SEQUENCE_START", " ", DIM_TYPE::STRING);
    //this->fred->RegisterRpcInfo("ALF_" + id + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/PUBLISH_IC_SEQUENCE_STOP", " ", DIM_TYPE::STRING);
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

//RpcInfoString* AlfClients::getAlfNode(string alf, int32_t serial, int32_t link, Instructions::Type type, bool start)
//{
//    switch (type)
//    {
//        case Instructions::Type::SCA:
//            return (RpcInfoString*)this->fred->GetRpcInfo("ALF_" + alf + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + (start ? "/PUBLISH_SCA_SEQUENCE_START" : "/PUBLISH_SCA_SEQUENCE_STOP"));

//        case Instructions::Type::SWT:
//            return (RpcInfoString*)this->fred->GetRpcInfo("ALF_" + alf + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + (start ? "/PUBLISH_SWT_SEQUENCE_START" : "/PUBLISH_SWT_SEQUENCE_STOP"));
      
//        case Instructions::Type::IC:
//            return (RpcInfoString*)this->fred->GetRpcInfo("ALF_" + alf + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + (start ? "/PUBLISH_IC_SEQUENCE_START" : "/PUBLISH_IC_SEQUENCE_STOP"));
//    }

//    return NULL;
//}

Queue* AlfClients::getAlfQueue(string alf)
{
    return queues[alf];
}

/*string AlfClients::getAlfDns(int32_t alf)
{
    return dns[alf];
}*/

string AlfClients::getAlfSubscribeTopic(string alf, int32_t serial, int32_t link, Instructions::Type type, string name)
{
    switch (type)
    {
        case Instructions::Type::SCA:
            return "ALF_" + alf + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/PUBLISH_SCA_SEQUENCE/" + name;
        case Instructions::Type::SWT:
            return "ALF_" + alf + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/PUBLISH_SWT_SEQUENCE/" + name;
        case Instructions::Type::IC:
            return "ALF_" + alf + "/SERIAL_" + to_string(serial) + "/LINK_" + to_string(link) + "/PUBLISH_IC_SEQUENCE/" + name;
    }

    return "";
}
