#pragma once

#include "Debug.cpp"
#include "../lib/http/httpparser.hpp"

#include <cstdint>
#include <string>
#include <ctime>

typedef uint16_t ClientFD_t;
typedef uint16_t ClientRequestNr_t;

class Client : public HTTPParser
{

public:

    Client(ClientFD_t);
    ~Client();

    void incrementReqNr();
    ClientRequestNr_t getCurrentReqNr();

protected:

    ClientFD_t _ClientFD;

private:

    ClientRequestNr_t _RequestNr;
    time_t _SocketConnectTime;
    time_t _SocketDisconnectTime;
};
