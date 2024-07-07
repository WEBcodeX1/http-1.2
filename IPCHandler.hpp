#ifndef IPCHandler_hpp
#define IPCHandler_hpp

#include "Debug.cpp"

#include <cstdint>
#include <vector>

using namespace std;

typedef vector<size_t> SharedMemOffset_t;
typedef void* SharedMemAddress_t;

static const SharedMemOffset_t ElementSizes{
    sizeof(uint16_t), //- client filedescriptor
    sizeof(uint16_t), //- msg-length (bytes)
    sizeof(nullptr)   //- message-size (bytes), dynamic size
};


class SharedMemManager
{

public:

    SharedMemManager();
    ~SharedMemManager();

    void setSharedMemBaseAddress(void*);
    void resetSharedMemOffsetAddress();
    SharedMemAddress_t getNextSharedMemAddress();
    SharedMemAddress_t getNextSharedMemAddress(uint16_t);
    SharedMemAddress_t getCurrentOffsetAddress();

private:

    void _stepSharedMemElementOffset();

    SharedMemAddress_t _BaseAddress;
    SharedMemAddress_t _OffsetAddress;

    SharedMemOffset_t _OffsetSizes;
    int _ElementOffset;

};

#endif
