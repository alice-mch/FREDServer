#ifndef __ALFRED
#define __ALFRED

#include <iostream>
#include <string>
#include <map>

#include "Alfred/types.h"

using namespace std;

class Service;
class Command;
class FrontEnd;
class Function;
class Info;
class Client;
class Rpc;
class RpcInfo;

class ALFRED
{
	friend class Service;
	friend class Command;
	friend class Info;
	friend class Client;
	friend class Function;
    friend class Rpc;
    friend class RpcInfo;

private:
	map<string, Service*> services;
	map<string, Command*> commands;
	map<string, Info*> infos;
	map<string, Client*> clients;
    map<string, Rpc*> rpcs;
    map<string, RpcInfo*> rpcinfos;
	FrontEnd* frontend;

	string server, dns, network;

    void ConnectCmdSrv(string source, string destination, bool connect = true);
    void ConnectCmdFnc(string source, string destination, bool connect = true);
    void ConnectFncSrv(string source, string destination, bool connect = true);
    void ConnectInfSrv(string source, string destination, bool connect = true);
    void ConnectInfFnc(string source, string destination, bool connect = true);
    void ConnectCmdCnt(string source, string destination, bool connect = true);
    void ConnectFncCnt(string source, string destination, bool connect = true);
    void ConnectInfCnt(string source, string destination, bool connect = true);
    void ConnectCmdRpcinf(string source, string destination, bool connect = true);
    void ConnectFncRpcinf(string source, string destination, bool connect = true);
    void ConnectInfRpcinf(string source, string destination, bool connect = true);
    void ConnectRpcinfSrv(string source, string destination, bool connect = true);
    void ConnectRpcinfCnt(string source, string destination, bool connect = true);
    void ConnectRpcinfFnc(string source, string destination, bool connect = true);
    void ConnectRpcSrv(string source, string destination, bool connect = true);
    void ConnectRpcCnt(string source, string destination, bool connect = true);
    void ConnectRpcFnc(string source, string destination, bool connect = true);

	string GetIP();
	string GetHost();

public:
	ALFRED(string server, string dns, string network = "");
	~ALFRED();

	void RegisterService(string name, DIM_TYPE type, size_t size = 0, string format = "");
	void RegisterCommand(string name, DIM_TYPE type, string format = "");
	void RegisterInfo(string name, DIM_TYPE type);
	void RegisterClient(string name, DIM_TYPE type, size_t size = 0);
    void RegisterRpc(string name, DIM_TYPE type, size_t size = 0, string formatIn = "", string formatOut = "");
    void RegisterRpcInfo(string name, string dns, DIM_TYPE type, size_t size = 0);

	void RegisterFrontend(FrontEnd* frontend);
	void RegisterCommand(Command* command);
	void RegisterInfo(Info* info);
    void RegisterRpc(Rpc* rpc);
    void RegisterRpcInfo(RpcInfo* rpcinfo);
    void RegisterService(Service* service);

	Service* GetService(string name);
	Command* GetCommand(string name);
	Info* GetInfo(string name);
	Client* GetClient(string name);
	Function* GetFunction(string name);
    Rpc* GetRpc(string name);
    RpcInfo* GetRpcInfo(string name);

	void Connect(CONNECT type, string source, string destination);
    void Disconnect(CONNECT type, string source, string destination);

	void StartOnce();
    virtual void Start();

	string Name();
	static bool ServerRegistered(string name);
};

#endif
