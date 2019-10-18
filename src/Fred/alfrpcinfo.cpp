#include "Fred/alfrpcinfo.h"
#include "Alfred/print.h"
#include "Parser/utility.h"
#include <algorithm>

#define MAX_POLL_COUNT 100

AlfRpcInfo::AlfRpcInfo(string name, string dns, Fred* fred): RpcInfoString::RpcInfoString(name, dns, (ALFRED*)fred)
{
    this->currentTransaction.first = NULL;
    this->currentTransaction.second = NULL;
    this->name = name;
    this->currentPart = 0;
    this->pollRepeat = 0;
}

const void* AlfRpcInfo::Execution(void *value)
{
    Print::PrintVerbose(currentTransaction.second->name, "Received RPC Info from " + name + ":\n" + string(static_cast<char*>(value)));

    if (isTransactionAvailable())
    {
        string response = static_cast<char*>(value);
        vector<string>* pollPatern = this->currentTransaction.first->getPollPattern();

        if (!pollPatern->size())
        {
            return NULL;
        }

        if (!this->currentTransaction.first->checkLink(response, *this->currentTransaction.second))
        {
            pollPatern->at(this->currentPart) = "ERROR"; //stop requests

            clearTransaction();

            return NULL;
        }

        if (pollPatern->at(this->currentPart).empty()) //if message part not for poll
        {
            if (this->currentPart > 0) //check if failure occured
            {
                if (response.find("failure") != string::npos)
                {
                    this->fullResponse.replace(0, this->fullResponse.find("\n"), "failure");
                }

                response = response.substr(response.find("\n") + 1);
            }

            this->fullResponse += response; //attach response to message, increase counter
            this->currentPart++;
        }
        else    //evaluate poll status
        {
            //todo - rewrite based on transaction type
            string equation = pollPatern->at(this->currentPart);
            vector<string> eqns = Utility::splitString(equation, "=");
            vector<string> vars = vector<string>({ "_RES_" });
            vector<double> vals = vector<double>({ double(stoul(response.substr(response.size() - 9, 8), NULL, 16)) });
            
            if (Utility::calculateEquation(eqns[0], vars, vals) == Utility::calculateEquation(eqns[1], vars, vals))
            {
                pollPatern->at(this->currentPart).clear();
                this->currentPart++;
                this->pollRepeat = 0;
            }
            else
            {
                this->pollRepeat++;
                if (this->pollRepeat > MAX_POLL_COUNT)
                {
                    pollPatern->at(this->currentPart) = "ERROR"; //stop polling
                    this->currentTransaction.first->updateResponse(*this->currentTransaction.second, "Polling timeout! Equation " + equation + " was not satisfied", true);

                    clearTransaction();
                    return NULL;
                }
            }
        }

        if (this->currentPart == pollPatern->size()) //if whole message received
        {
            if (this->currentTransaction.second->mapi == NULL)
            {
                this->currentTransaction.first->evaluateMessage(this->fullResponse, *this->currentTransaction.second);
            }
            else
            {
                this->currentTransaction.first->evaluateMapiMessage(this->fullResponse, *this->currentTransaction.second);
            }

            clearTransaction();
        }
    }
    else
    {
        Print::PrintWarning(currentTransaction.second->name, "No current transaction available! Discarding message");
        clearTransaction();
    }

    return NULL;
}

void AlfRpcInfo::setTransaction(pair<ProcessMessage*, ChainTopic*> currentTransaction)
{
    if (this->currentTransaction.first)
    {
        delete this->currentTransaction.first;
        this->currentTransaction.first = NULL;
    }
    this->currentTransaction = currentTransaction;
}

void AlfRpcInfo::clearTransaction()
{
    this->fullResponse = "";
    this->currentPart = 0;
    this->pollRepeat = 0;
    this->currentTransaction.second = NULL;
}

bool AlfRpcInfo::isTransactionAvailable()
{
    return this->currentTransaction.first && this->currentTransaction.second;
}
