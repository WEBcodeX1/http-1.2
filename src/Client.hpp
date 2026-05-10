#pragma once

#include "Debug.cpp"
#include "Constant.hpp"
#include "../lib/http/httpparser.hpp"

#include <cstdint>
#include <string>
#include <ctime>

typedef uint16_t ClientFD_t;

class Client : public HTTPParser
{

public:

    Client(ClientFD_t, char*);
    ~Client();

    ssize_t receiveData();

protected:

    ClientFD_t _ClientFD;

private:

    char* _ReceiveBuffer;
    time_t _ConnectTime;
    char _SendBuffer[SOCKET_SEND_BUFFER_SIZE];

};
