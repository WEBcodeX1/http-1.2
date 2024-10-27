#ifndef IPCHandlerAS_hpp
#define IPCHandlerAS_hpp

#include "Debug.cpp"
#include "IPCHandler.hpp"

#include <cstdint>
#include <vector>

using namespace std;

typedef vector<size_t> SHMOffset_t;

typedef struct {
    void* PostASMetaPtr;
    void* PostASRequestsPtr;
    void* PostASResultsPtr;
} BaseAdresses_t;


static const SHMOffset_t MetadataSizes{
    sizeof(uint16_t), //- can read user space lock
    sizeof(uint16_t), //- write ready user space lock
    sizeof(uint16_t), //- client file descriptor
    sizeof(uint16_t), //- http version
    sizeof(uint16_t), //- http method
    sizeof(uint16_t), //- request number
    sizeof(uint32_t), //- payload length
};


class SHMPythonAS
{

public:

    SHMPythonAS();
    ~SHMPythonAS();

    void setBaseAddresses(const BaseAdresses_t);
    SharedMemAddress_t getMetaAddress(const uint8_t, const uint8_t);
    SharedMemAddress_t getRequestAddress(const uint8_t);
    SharedMemAddress_t getResultAddress(const uint8_t);

private:

    uint8_t _MetaSegmentSize;
    vector<uint8_t> _MetadataOffsets;
    BaseAdresses_t _BaseAdresses;

};

#endif
