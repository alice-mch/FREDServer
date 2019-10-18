#include "Fred/crualfrpcinfo.h"
#include "Alfred/print.h"

CruAlfRpcInfo::CruAlfRpcInfo(string name, Fred* fred, Type type): RpcInfoString::RpcInfoString(name, " ", (ALFRED*)fred)
{
    this->type = type;
}

const void* CruAlfRpcInfo::Execution(void *value) //todo
{
    if (!value)
    {
        Print::PrintError("Invalid RPC Info received!");
        return NULL;
    }

    string response(static_cast<char*>(value));

    Print::PrintVerbose("Received RPC Info:\n" + response);

    if (response.find("success") != string::npos || response.find("failure") != string::npos)
    {
        response.erase(0, response.find("\n") + 1);
    }

    Parent()->GetService(Parent()->Name() + (type == WRITE ? "/CRU_REGISTER/WRITE_ANS" : "/CRU_REGISTER/READ_ANS"))->Update(response.c_str());

    return NULL;
}
