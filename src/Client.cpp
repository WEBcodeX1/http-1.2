#include "Client.hpp"

#include <cerrno>

using namespace std;

Client::Client(Filedescriptor_t ClientFD, char* BufferAddress) :
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
    ssize_t RcvBytes;
    do {
        RcvBytes = read(_ClientFD, _ReceiveBuffer, SOCKET_RECEIVE_BUFFER_SIZE);
    } while (RcvBytes == -1 && errno == EINTR);

    DBG(220, "RcvBytes:" << RcvBytes << " ClientFD:" << _ClientFD);

    if (RcvBytes > 0) {
        appendBuffer(_ReceiveBuffer, RcvBytes);
    }

    return RcvBytes;
}
