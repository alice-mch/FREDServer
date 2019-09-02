#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "Fred/fredtopics.h"
#include "Fred/fred.h"
#include "Alfred/print.h"
#include "Parser/processmessage.h"

void Mapi::getFred(Fred* fred)
{
    this->fred = fred;
}

void Mapi::getName(string name)
{
    this->name = name;

    FredTopics& topics = this->fred->getFredTopics();

    map<string, ChainTopic>& map = topics.getTopicsMap();

    thisMapi = &map[name];
}
