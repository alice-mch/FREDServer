#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "Fred/fredtopics.h"
#include "Fred/fred.h"
#include "Alfred/print.h"
#include "Parser/processmessage.h"

/*
 * Send a new request to the same MAPI topic 
 */
void Iterativemapi::newRequest(string request)
{
    ProcessMessage* processMessage = new ProcessMessage(this, request);
    thisMapi->alfQueue->newRequest(make_pair(processMessage, thisMapi));
}

/*
 * Publish MAPI _ANS service 
 */
void Iterativemapi::publishAnswer(string message)
{
    thisMapi->service->Update(message.c_str());

    PrintVerbose(name, "Service Updating MAPI service");
}

/*
 * Publish MAPI _ERR service 
 */
void Iterativemapi::publishError(string error)
{
    thisMapi->error->Update(error.c_str());

	PrintError(name, "Updating MAPI error service!");
}
