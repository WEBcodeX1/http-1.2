#pragma once

#include "Debug.cpp"
#include "Constant.hpp"
#include "Global.hpp"

#include "../lib/http/httpparser.hpp"

#include <cstdint>
#include <string>
#include <ctime>

#include <unistd.h>


class Client : public HTTPParser
{

public:

    Client(Filedescriptor_t, char*);
    ~Client();

    bool receiveData();

protected:

    Filedescriptor_t _ClientFD;

private:

    char* _ReceiveBuffer;
    time_t _ConnectTime;
    char _SendBuffer[SOCKET_SEND_BUFFER_SIZE];

};
