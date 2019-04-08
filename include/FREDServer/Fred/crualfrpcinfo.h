#ifndef CRUALFRPCINFO_H
#define CRUALFRPCINFO_H

#include <string>
#include "Alfred/rpcinfo.h"
#include "Alfred/service.h"
#include "Fred/fred.h"

using namespace std;

class CruAlfRpcInfo: public RpcInfoString
{
public:
    enum Type
    {
        WRITE,
        READ
    };

    CruAlfRpcInfo(string name, Fred* fred, Type type);

private:
    const void* Execution(void* value);
    Type type;
};

#endif // CRUALFRPCINFO_H
