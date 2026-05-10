#include "Client.hpp"

using namespace std;

Client::Client(ClientFD_t ClientFD, char* BufferAddress) :
    HTTPParser(4096),
    _ClientFD(ClientFD),
    _ReceiveBuffer(BufferAddress),
    _ConnectTime(0)
{
    DBG(120, "Constructor");

    _ConnectTime = time(nullptr);
}

Client::~Client()
{
    DBG(120, "Destructor");
}

ssize_t Client::receiveData()
{
    ssize_t RcvBytes = read(_ClientFD, _ReceiveBuffer, SOCKET_RECEIVE_BUFFER_SIZE);
    DBG(220, "RcvBytes:" << RcvBytes << " ClientFD:" << _ClientFD);

    if (RcvBytes > 0) {
        appendBuffer(_ReceiveBuffer, RcvBytes);
    }

    return RcvBytes;
}
