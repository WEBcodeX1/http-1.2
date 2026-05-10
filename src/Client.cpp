#include "Client.hpp"

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

bool Client::receiveData()
{
    bool DataInKernelBuffer = true;

    while (DataInKernelBuffer == true) {

        ssize_t RcvBytes = read(_ClientFD, _ReceiveBuffer, SOCKET_RECEIVE_BUFFER_SIZE);
        DBG(220, "RcvBytes:" << RcvBytes << " ClientFD:" << _ClientFD);

        if (RcvBytes > 0) {
            appendBuffer(_ReceiveBuffer, RcvBytes);
        }
        else if (RcvBytes == 0) {
            DataInKernelBuffer = false;
        }
        else if (RcvBytes < 0) {
            const int RecvErrno = errno;
            DataInKernelBuffer = false;
            if (RecvErrno == EAGAIN || RecvErrno == EWOULDBLOCK || RecvErrno == EINTR) {
                return false;
            }
        }
    }
    return true;
}
