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
    sizeof(uint16_t), //- http version
    sizeof(uint16_t), //- request number
    sizeof(uint16_t), //- payload-length (bytes)
    sizeof(nullptr)   //- payload-size (bytes), dynamic size
};

typedef struct {
    void* StaticFSPtr;
    void* PostASStatusPtr;
    void* PostASRequestsPtr;
    void* PostASResultsPtr;
} SHMSegmentPointer_t;


class SHMStaticFS
{

public:

    SHMStaticFS();
    ~SHMStaticFS();

    void setBaseAddress(void*);
    void resetOffset();
    SharedMemAddress_t getNextAddress();
    SharedMemAddress_t getNextAddress(uint16_t);
    SharedMemAddress_t getCurrentOffsetAddress();

private:

    void _stepElementOffset();

    SharedMemAddress_t _BaseAddress;
    SharedMemAddress_t _OffsetAddress;

    SharedMemOffset_t _OffsetSizes;
    uint8_t _ElementOffset;

};

#endif
