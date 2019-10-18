#ifndef ITERATIVEMAPI_H
#define ITERATIVEMAPI_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

class Fred;

using namespace std;

class Iterativemapi: public Mapi
{
public:
    Iterativemapi();

    void newRequest(string request);
    
    void publishAnswer(string message);
    void publishError(string error);

protected:
    bool useCru;
};

#endif // ITERATIVEMAPI_H
