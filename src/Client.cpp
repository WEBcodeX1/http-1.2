#include "Client.hpp"

using namespace std;

Client::Client(ClientFD_t ClientFD) :
    HTTPParser(4096),
    _ClientFD(ClientFD),
    _RequestNr(0),
    _SocketConnectTime(0),
    _SocketDisconnectTime(0)
{
    DBG(120, "Constructor");

    _SocketConnectTime = time(nullptr);
    _SocketDisconnectTime = time(nullptr);
}

Client::~Client()
{
    DBG(120, "Destructor");
}

ClientRequestNr_t Client::getCurrentReqNr()
{
    return _RequestNr;
}

void Client::incrementReqNr()
{
    _RequestNr += 1;
}
