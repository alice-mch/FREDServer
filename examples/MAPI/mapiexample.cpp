#include "Mapi/mapiexample.h"


MapiExample::MapiExample()
{
    //your constructor
}

string MapiExample::processInputMessage(string input)
{
    //input - message received from DIM command

    string request;

    //generate message for ALF based on data received from WinCC
    if (input == "1") request = "11111";
    else if (input == "2") request = "22222";
    else request = "0";

    return request; //send message to the ALF
}

string MapiExample::processOutputMessage(string output)
{
    //output - message received from the ALF

    string response;

    if (output.find("success") != string::npos) //request succesfull
    {
        //generate answer based on response of the ALF
        if (output.find("11111") != string::npos) response = "33333";
        else if (output.find("22222") != string::npos) response = "44444";
        else response = "Unknown";

    }
    else
    {
        response = "Error";
    }

    return response; //send answer back to WinCC
}
