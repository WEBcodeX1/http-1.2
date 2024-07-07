#ifndef Client_hpp
#define Client_hpp

#include "Debug.cpp"

#include <cstdint>
#include <string>
#include <ctime>

typedef uint16_t ClientFD_t;


class Client
{

public:

    Client(ClientFD_t);
    ~Client();

protected:

    ClientFD_t _ClientFD;

private:

    bool _Error;
    uint16_t _ErrorID;

    time_t _RequestStartTime;
    time_t _RequestEndTime;
    time_t _ResponseStartTime;
    time_t _ResponseEndTime;

    bool _TimeoutReached;

};

#endif
