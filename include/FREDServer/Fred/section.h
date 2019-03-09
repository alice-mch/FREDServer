#ifndef SECTION_H
#define SECTION_H

#include <string>
#include <vector>

#include "Fred/Config/location.h"
#include "Fred/Config/instructions.h"
#include "Fred/Config/mapping.h"
#include "Fred/Config/groups.h"

using namespace std;

class Section
{
private:
    string name;

public:
    Location location;
    Instructions instructions;
    Mapping mapping;
    Groups groups;

    Section(string name);
    string getName();
};

#endif
