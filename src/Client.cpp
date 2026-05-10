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
    ssize_t TotalRcvBytes = 0;

    for (;;) {
        const ssize_t RcvBytes = read(_ClientFD, _ReceiveBuffer, SOCKET_RECEIVE_BUFFER_SIZE);
        const int ReadErrno = errno;

        DBG(220, "RcvBytes:" << RcvBytes << " ClientFD:" << _ClientFD);

        if (RcvBytes > 0) {
            appendBuffer(_ReceiveBuffer, RcvBytes);
            TotalRcvBytes += RcvBytes;
            continue;
        }

        if (RcvBytes == 0) {
            return 0;
        }

        if (ReadErrno == EINTR) {
            continue;
        }

        if (
            (ReadErrno == EAGAIN || ReadErrno == EWOULDBLOCK) &&
            TotalRcvBytes > 0
        ) {
            return TotalRcvBytes;
        }

        errno = ReadErrno;
        return -1;
    }
}
