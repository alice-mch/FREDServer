#include "Alfred/alfred.h"
#include "Alfred/print.h"
#include "Alfred/service.h"
#include "Alfred/command.h"
#include "Alfred/client.h"
#include "Alfred/frontend.h"
#include "Alfred/info.h"
#include "Alfred/rpc.h"
#include "Alfred/rpcinfo.h"

ALFRED::ALFRED(string server, string dns, string network)
{
	this->server = server;
	this->dns = dns;
	this->network = network;
	this->frontend = NULL;

    //cout << dns << "\n";

	setenv("DIM_HOST_NODE", network == "" ? GetHost().c_str() : GetIP().c_str(), 1);
    setenv("DIM_DNS_NODE", dns.c_str(), 1);

	if (ServerRegistered(server))
	{
        Print::PrintError(string("Server ") + server + " is already registered on DIM DNS!");
		exit(EXIT_FAILURE);
	}
}

ALFRED::~ALFRED()
{
	if (frontend)
	{
		delete frontend;
	}

	for (auto it = services.begin(); it !=services.end(); it++)
	{
		delete it->second;
	}

	for (auto it = commands.begin(); it !=commands.end(); it++)
	{
		delete it->second;
	}

	for (auto it = infos.begin(); it !=infos.end(); it++)
	{
		delete it->second;
	}

	for (auto it = clients.begin(); it !=clients.end(); it++)
	{
		delete it->second;
	}

    for (auto it = rpcs.begin(); it != rpcs.end(); it++)
    {
        delete it->second;
    }

    for (auto it = rpcinfos.begin(); it != rpcinfos.end(); it++)
    {
        delete it->second;
    }
}

void ALFRED::RegisterService(string name, DIM_TYPE type, size_t size, string format)
{
	if (type == DIM_TYPE::DATA && size == 0)
	{
        Print::PrintError("Invalid size of servis!");
		exit(EXIT_FAILURE);
	}

	switch (type)
	{
		case DIM_TYPE::INT:
			services[name] = new ServiceInt(name, this);
			break;
		case DIM_TYPE::FLOAT:
			services[name] = new ServiceFloat(name, this);
			break;
        case DIM_TYPE::STRING:
            services[name] = new ServiceString(name, this);
            break;
		case DIM_TYPE::DATA:
			services[name] = new ServiceData(name, this, size, format);
			break;
		default:
            Print::PrintError("Invalid service type!");
			exit(EXIT_FAILURE);
	}
}

void ALFRED::RegisterCommand(string name, DIM_TYPE type, string format)
{
	switch (type)
	{
		case DIM_TYPE::INT:
			commands[name] = new CommandInt(name, this);
			break;
		case DIM_TYPE::FLOAT:
			commands[name] = new CommandFloat(name, this);
			break;
        case DIM_TYPE::STRING:
            commands[name] = new CommandString(name, this);
            break;
		case DIM_TYPE::DATA:
			commands[name] = new CommandData(name, format, this);
			break;
		default:
            Print::PrintError("Invalid command type!");
			exit(EXIT_FAILURE);
	}
}

void ALFRED::RegisterInfo(string name, DIM_TYPE type)
{
	switch (type)
	{
		case DIM_TYPE::INT:
			infos[name] = new InfoInt(name, this);
			break;
		case DIM_TYPE::FLOAT:
			infos[name] = new InfoFloat(name, this);
			break;
        case DIM_TYPE::STRING:
            infos[name] = new InfoString(name, this);
            break;
		case DIM_TYPE::DATA:
			infos[name] = new InfoData(name, this);
			break;
		default:
            Print::PrintError("Invalid info type!");
			exit(EXIT_FAILURE);
	}
}

void ALFRED::RegisterClient(string name, DIM_TYPE type, size_t size)
{
	if (type == DIM_TYPE::DATA && size == 0)
	{
        Print::PrintError("Invalid size of client!");
		exit(EXIT_FAILURE);
	}

	switch (type)
	{
		case DIM_TYPE::INT:
			clients[name] = new ClientInt(name, this);
			break;
		case DIM_TYPE::FLOAT:
			clients[name] = new ClientFloat(name, this);
			break;
        case DIM_TYPE::STRING:
            clients[name] = new ClientString(name, this);
            break;
		case DIM_TYPE::DATA:
			clients[name] = new ClientData(name, this, size);
			break;
		default:
            Print::PrintError("Invalid client type!");
			exit(EXIT_FAILURE);
	}
}

void ALFRED::RegisterRpc(string name, DIM_TYPE type, size_t size, string formatIn, string formatOut)
{
    if (type == DIM_TYPE::DATA && size == 0)
    {
        Print::PrintError("Invalid size of rpc!");
        exit(EXIT_FAILURE);
    }

    switch (type)
    {
        case DIM_TYPE::INT:
            rpcs[name] = new RpcInt(name, this);
            break;
        case DIM_TYPE::FLOAT:
            rpcs[name] = new RpcFloat(name, this);
            break;
        case DIM_TYPE::STRING:
            rpcs[name] = new RpcString(name, this);
            break;
        case DIM_TYPE::DATA:
            rpcs[name] = new RpcData(name, this, size, formatIn, formatOut);
            break;
        default:
            Print::PrintError("Invalid rpc type!");
            exit(EXIT_FAILURE);
    }
}

void ALFRED::RegisterRpcInfo(string name, string dns, DIM_TYPE type, size_t size)
{
    if (type == DIM_TYPE::DATA && size == 0)
    {
        Print::PrintError("Invalid size of rpcinfo!");
        exit(EXIT_FAILURE);
    }

    switch (type)
    {
        case DIM_TYPE::INT:
            rpcinfos[name] = new RpcInfoInt(name, dns, this);
            break;
        case DIM_TYPE::FLOAT:
            rpcinfos[name] = new RpcInfoFloat(name, dns, this);
            break;
        case DIM_TYPE::STRING:
            rpcinfos[name] = new RpcInfoString(name, dns, this);
            break;
        case DIM_TYPE::DATA:
            rpcinfos[name] = new RpcInfoData(name, dns, this, size);
            break;
        default:
            Print::PrintError("Invalid rpcinfo type!");
            exit(EXIT_FAILURE);
    }
}

void ALFRED::RegisterFrontend(FrontEnd* frontend)
{
	this->frontend = frontend;
}

void ALFRED::RegisterCommand(Command* command)
{
	if (!command)
	{
        Print::PrintError("Invalid command!");
		exit(EXIT_FAILURE);
	}
	else
	{
		commands[command->Name()] = command;
	}
}

void ALFRED::RegisterService(Service* service)
{
    if (!service)
    {
        Print::PrintError("Invalid service!");
        exit(EXIT_FAILURE);
    }
    else
    {
        services[service->Name()] = service;
    }
}

void ALFRED::RegisterInfo(Info* info)
{
	if (!info)
	{
        Print::PrintError("Invalid info!");
		exit(EXIT_FAILURE);
	}
	else
	{
		infos[info->Name()] = info;
	}
}

void ALFRED::RegisterRpc(Rpc *rpc)
{
    if (!rpc)
    {
        Print::PrintError("Invalid rpc!");
        exit(EXIT_FAILURE);
    }
    else
    {
        rpcs[rpc->Name()] = rpc;
    }
}

void ALFRED::RegisterRpcInfo(RpcInfo *rpcinfo)
{
    if (!rpcinfo)
    {
        Print::PrintError("Invalid rpcinfo!");
        exit(EXIT_FAILURE);
    }
    else
    {
        rpcinfos[rpcinfo->Name()] = rpcinfo;
    }
}

void ALFRED::Connect(CONNECT type, string source, string destination)
{
	switch (type)
	{
		case CONNECT::COMMAND_SERVICE:
			ConnectCmdSrv(source, destination);
			break;
		case CONNECT::COMMAND_FUNCTION:
			ConnectCmdFnc(source, destination);
			break;
		case CONNECT::FUNCTION_SERVICE:
			ConnectFncSrv(source, destination);
			break;
		case CONNECT::INFO_SERVICE:
			ConnectInfSrv(source, destination);
			break;
		case CONNECT::INFO_FUNCTION:
			ConnectInfFnc(source, destination);
			break;
		case CONNECT::COMMAND_CLIENT:
			ConnectCmdCnt(source, destination);
			break;
		case CONNECT::FUNCTION_CLIENT:
			ConnectFncCnt(source, destination);
			break;
		case CONNECT::INFO_CLIENT:
			ConnectInfCnt(source, destination);
			break;
        case CONNECT::COMMAND_RPCINFO:
            ConnectCmdRpcinf(source, destination);
            break;
        case CONNECT::FUNCTION_RPCINFO:
            ConnectFncRpcinf(source, destination);
            break;
        case CONNECT::INFO_RPCINFO:
            ConnectInfRpcinf(source, destination);
            break;
        case CONNECT::RPCINFO_SERVICE:
            ConnectRpcinfSrv(source, destination);
            break;
        case CONNECT::RPCINFO_CLIENT:
            ConnectRpcinfCnt(source, destination);
            break;
        case CONNECT::RPCINFO_FUNCTION:
            ConnectRpcinfFnc(source, destination);
            break;
        case CONNECT::RPC_SERVICE:
            ConnectRpcSrv(source, destination);
            break;
        case CONNECT::RPC_CLIENT:
            ConnectRpcCnt(source, destination);
            break;
        case CONNECT::RPC_FUNCTION:
            ConnectRpcFnc(source, destination);
            break;
		default:
            Print::PrintError("Invalid connect type!");
			exit(EXIT_FAILURE);
	}

    Print::PrintVerbose(source + " connected to " + destination);
}

void ALFRED::Disconnect(CONNECT type, string source, string destination)
{
    switch (type)
    {
        case CONNECT::COMMAND_SERVICE:
            ConnectCmdSrv(source, destination, false);
            break;
        case CONNECT::COMMAND_FUNCTION:
            ConnectCmdFnc(source, destination, false);
            break;
        case CONNECT::FUNCTION_SERVICE:
            ConnectFncSrv(source, destination, false);
            break;
        case CONNECT::INFO_SERVICE:
            ConnectInfSrv(source, destination, false);
            break;
        case CONNECT::INFO_FUNCTION:
            ConnectInfFnc(source, destination, false);
            break;
        case CONNECT::COMMAND_CLIENT:
            ConnectCmdCnt(source, destination, false);
            break;
        case CONNECT::FUNCTION_CLIENT:
            ConnectFncCnt(source, destination, false);
            break;
        case CONNECT::INFO_CLIENT:
            ConnectInfCnt(source, destination, false);
            break;
        case CONNECT::COMMAND_RPCINFO:
            ConnectCmdRpcinf(source, destination, false);
            break;
        case CONNECT::FUNCTION_RPCINFO:
            ConnectFncRpcinf(source, destination, false);
            break;
        case CONNECT::INFO_RPCINFO:
            ConnectInfRpcinf(source, destination, false);
            break;
        case CONNECT::RPCINFO_SERVICE:
            ConnectRpcinfSrv(source, destination, false);
            break;
        case CONNECT::RPCINFO_CLIENT:
            ConnectRpcinfCnt(source, destination, false);
            break;
        case CONNECT::RPCINFO_FUNCTION:
            ConnectRpcinfFnc(source, destination, false);
            break;
        case CONNECT::RPC_SERVICE:
            ConnectRpcSrv(source, destination, false);
            break;
        case CONNECT::RPC_CLIENT:
            ConnectRpcCnt(source, destination, false);
            break;
        case CONNECT::RPC_FUNCTION:
            ConnectRpcFnc(source, destination, false);
            break;
        default:
            Print::PrintError("Invalid connect type!");
            exit(EXIT_FAILURE);
    }

    Print::PrintVerbose(source + " disconnected from " + destination);
}

void ALFRED::ConnectCmdSrv(string source, string destination, bool connect)
{
	Command* command = GetCommand(source);
	Service* service = GetService(destination);

    if (command->Type() != service->Type() && connect)
	{
        Print::PrintWarning("Connecting different types of command and service!");
	}

    command->ConnectService(connect ? service : NULL);
}

void ALFRED::ConnectCmdFnc(string source, string destination, bool connect)
{
	Command* command = GetCommand(source);
	Function* function = GetFunction(destination);

	if (function->Type() != FNC_TYPE::SHOT)
	{
        Print::PrintError("Cannot connect command to non-shot function!");
		exit(EXIT_FAILURE);
	}
	else
	{
        command->ConnectFunction(connect ? (FunctionShot*)function : NULL);
	}
}

void ALFRED::ConnectFncSrv(string source, string destination, bool connect)
{
	Function* function = GetFunction(source);
	Service* service = GetService(destination);

    function->ConnectService(connect ? service : NULL);
}

void ALFRED::ConnectInfSrv(string source, string destination, bool connect)
{
	Info* info = GetInfo(source);
	Service* service = GetService(destination);

    if (info->Type() != service->Type() && connect)
	{
        Print::PrintWarning("Connecting different types of info and service!");
	}

    info->ConnectService(connect ? service : NULL);
}

void ALFRED::ConnectInfFnc(string source, string destination, bool connect)
{
	Info* info = GetInfo(source);
	Function* function = GetFunction(destination);

	if (function->Type() != FNC_TYPE::SHOT)
	{
        Print::PrintError("Cannot connect info to non-shot function!");
		exit(EXIT_FAILURE);
	}
	else
	{
        info->ConnectFunction(connect ? (FunctionShot*)function : NULL);
	}
}

void ALFRED::ConnectCmdCnt(string source, string destination, bool connect)
{
	Command* command = GetCommand(source);
	Client* client = GetClient(destination);

    if (command->Type() != client->Type() && connect)
	{
        Print::PrintWarning("Connecting different types of command and client!");
	}

    command->ConnectClient(connect ? client : NULL);
}

void ALFRED::ConnectFncCnt(string source, string destination, bool connect)
{
	Function* function = GetFunction(source);
	Client* client = GetClient(destination);

    function->ConnectClient(connect ? client : NULL);
}

void ALFRED::ConnectInfCnt(string source, string destination, bool connect)
{
	Info* info = GetInfo(source);
	Client* client = GetClient(destination);

    if (info->Type() != client->Type() && connect)
	{
        Print::PrintWarning("Connecting different types of info and client!");
	}

    info->ConnectClient(connect ? client : NULL);
}

void ALFRED::ConnectCmdRpcinf(string source, string destination, bool connect)
{
    Command* command = GetCommand(source);
    RpcInfo* rpcinfo = GetRpcInfo(destination);

    if (command->Type() != rpcinfo->Type() && connect)
    {
        Print::PrintWarning("Connecting different types of command and rpcinfo!");
    }

    command->ConnectRpcInfo(connect ? rpcinfo : NULL);
}

void ALFRED::ConnectFncRpcinf(string source, string destination, bool connect)
{
    Function* function = GetFunction(source);
    RpcInfo* rpcinfo = GetRpcInfo(destination);

    function->ConnectRpcInfo(connect ? rpcinfo : NULL);
}

void ALFRED::ConnectInfRpcinf(string source, string destination, bool connect)
{
    Info* info = GetInfo(source);
    RpcInfo* rpcinfo = GetRpcInfo(destination);

    if (info->Type() != rpcinfo->Type() && connect)
    {
        Print::PrintWarning("Connecting different types of info and rpcinfo!");
    }

    info->ConnectRpcInfo(connect ? rpcinfo : NULL);
}

void ALFRED::ConnectRpcinfSrv(string source, string destination, bool connect)
{
    RpcInfo* rpcinfo = GetRpcInfo(source);
    Service* service = GetService(destination);

    if (rpcinfo->Type() != service->Type() && connect)
    {
        Print::PrintWarning("Connecting different types of rpcinfo and service!");
    }

    rpcinfo->ConnectService(connect ? service : NULL);
}

void ALFRED::ConnectRpcinfCnt(string source, string destination, bool connect)
{
    RpcInfo* rpcinfo = GetRpcInfo(source);
    Client* client = GetClient(destination);

    if (rpcinfo->Type() != client->Type() && connect)
    {
        Print::PrintWarning("Connecting different types of rpcinfo and client!");
    }

    rpcinfo->ConnectClient(connect ? client : NULL);
}

void ALFRED::ConnectRpcinfFnc(string source, string destination, bool connect)
{
    RpcInfo* rpcinfo = GetRpcInfo(source);
    Function* function = GetFunction(destination);

    if (function->Type() != FNC_TYPE::SHOT)
    {
        Print::PrintError("Cannot connect rpcinfo to non-shot function!");
        exit(EXIT_FAILURE);
    }
    else
    {
        rpcinfo->ConnectFunction(connect ? (FunctionShot*)function : NULL);
    }
}

void ALFRED::ConnectRpcSrv(string source, string destination, bool connect)
{
    Rpc* rpc = GetRpc(source);
    Service* service = GetService(destination);

    if (rpc->Type() != service->Type() && connect)
    {
        Print::PrintWarning("Connecting different types of rpc and service!");
    }

    rpc->ConnectService(connect ? service : NULL);
}

void ALFRED::ConnectRpcCnt(string source, string destination, bool connect)
{
    Rpc* rpc = GetRpc(source);
    Client* client = GetClient(destination);

    if (rpc->Type() != client->Type() && connect)
    {
        Print::PrintWarning("Connecting different types of rpc and client!");
    }

    rpc->ConnectClient(connect ? client : NULL);
}

void ALFRED::ConnectRpcFnc(string source, string destination, bool connect)
{
    Rpc* rpc = GetRpc(source);
    Function* function = GetFunction(destination);

    if (function->Type() != FNC_TYPE::SHOT)
    {
        Print::PrintError("Cannot connect rpc to non-shot function!");
        exit(EXIT_FAILURE);
    }
    else
    {
        rpc->ConnectFunction(connect ? (FunctionShot*)function : NULL);
    }
}

Service* ALFRED::GetService(string name)
{
	if (services.count(name))
	{
		return services[name];
	}
	else
	{
        Print::PrintError(string("No service ") + name + " exists!");
		exit(EXIT_FAILURE);
	}
}

Command* ALFRED::GetCommand(string name)
{
	if (commands.count(name))
	{
		return commands[name];
	}
	else
	{
        Print::PrintError(string("No command ") + name + " exists!");
		exit(EXIT_FAILURE);
	}
}

Info* ALFRED::GetInfo(string name)
{
	if (infos.count(name))
	{
		return infos[name];
	}
	else
	{
        Print::PrintError(string("No info ") + name + " exists!");
		exit(EXIT_FAILURE);
	}
}

Client* ALFRED::GetClient(string name)
{
	if (clients.count(name))
	{
		return clients[name];
	}
	else
	{
        Print::PrintError(string("No client ") + name + " exists!");
		exit(EXIT_FAILURE);
	}
}

Function* ALFRED::GetFunction(string name)
{
	if (!frontend)
	{
        Print::PrintError("No FrontEnd defined!");
		exit(EXIT_FAILURE);
	}
	else
	{
		return frontend->GetFunction(name);
	}
}

Rpc* ALFRED::GetRpc(string name)
{
    if (rpcs.count(name))
    {
        return rpcs[name];
    }
    else
    {
        Print::PrintError(string("No rpc ") + name + " exists!");
        exit(EXIT_FAILURE);
    }
}

RpcInfo* ALFRED::GetRpcInfo(string name)
{
    if (rpcinfos.count(name))
    {
        return rpcinfos[name];
    }
    else
    {
        Print::PrintError(string("No rpcinfo ") + name + " exists!");
        exit(EXIT_FAILURE);
    }
}

void ALFRED::StartOnce()
{
	DimServer::start(server.c_str());
    Print::PrintInfo(string("Server ") + server + " started!");
}

void ALFRED::Start()
{
	StartOnce();

	while (1)
	{
		usleep(1000000);
	}
}

string ALFRED::GetIP()
{
	FILE *fpipe = popen((string("ip addr show ") + network + " | grep -E -o \"[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\" | head -1").c_str(), "r");

	if (!fpipe)
	{
        Print::PrintError("Cannot get IP address!");
		exit(EXIT_FAILURE);
	}
	else
	{
		char ip[17] = "\0";
		fgets(ip, 16, fpipe);
		pclose(fpipe);

		char *endOfLine = strchr(ip, '\n');
	    if (endOfLine)
	    {
			*endOfLine = '\0';
	    }

	    return string(ip);
	}

}

string ALFRED::GetHost()
{
	FILE *fpipe = popen("hostname", "r");

	if (!fpipe)
	{
        Print::PrintError("Cannot get Hostname!");
		exit(EXIT_FAILURE);
	}
	else
	{
		char host[17] = "\0";
		fgets(host, 16, fpipe);
		pclose(fpipe);

		char *endOfLine = strchr(host, '\n');
	    if (endOfLine)
	    {
			*endOfLine = '\0';
	    }

	    return string(host);
	}

}

string ALFRED::Name()
{
	return server;
}

bool ALFRED::ServerRegistered(string name)
{
	DimBrowser browser;
	char *server, *node;

	browser.getServers();
	while (browser.getNextServer(server, node))
	{
		if (name == server)
		{
			return true;
		}
	}

	return false;
}
