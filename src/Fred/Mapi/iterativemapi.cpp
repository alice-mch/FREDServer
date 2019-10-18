#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "Fred/fredtopics.h"
#include "Fred/fred.h"
#include "Alfred/print.h"
#include "Parser/processmessage.h"

Iterativemapi::Iterativemapi()
{
    this->useCru = true;
}

/*
 * Send a new request to the same MAPI topic 
 */
void Iterativemapi::newRequest(string request)
{
    ProcessMessage* processMessage = new ProcessMessage(this, request, this->useCru);
    Queue* queue = this->useCru ? this->thisMapi->alfQueue.first : this->thisMapi->alfQueue.second;
    if (!queue)
    {
        string error = "Required ALF/CANALF not available!";
        Print::PrintError(name, error);
        thisMapi->error->Update(error.c_str());
        delete processMessage;
        return;
    }

    queue->newRequest(make_pair(processMessage, thisMapi));
}

/*
 * Publish MAPI _ANS service 
 */
void Iterativemapi::publishAnswer(string message)
{
    thisMapi->service->Update(message.c_str());

    Print::PrintVerbose(name, "Service Updating MAPI service");
}

/*
 * Publish MAPI _ERR service 
 */
void Iterativemapi::publishError(string error)
{
    thisMapi->error->Update(error.c_str());

    Print::PrintError(name, "Updating MAPI error service!");
}
