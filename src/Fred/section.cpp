#include "Fred/section.h"

Section::Section(string name)
{
    this->name = name;
}

string Section::getName()
{
    return name;
}
