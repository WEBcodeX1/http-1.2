#include "Client.hpp"

using namespace std;

Client::Client(ClientFD_t ClientFD) :
    _ClientFD(ClientFD),
    _Error(false),
    _ErrorID(0),
    _RequestStartTime(0),
    _RequestEndTime(0),
    _TimeoutReached(false)
{
    DBG(120, "Constructor");

    _RequestStartTime = time(nullptr);
    _ResponseStartTime = time(nullptr);
}

Client::~Client()
{
    DBG(120, "Destructor");
}
