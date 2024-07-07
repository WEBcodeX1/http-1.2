#include "IPCHandler.hpp"

//- convert to C++ 20 iterator / C++ 23 generator
using namespace std;

SharedMemManager::SharedMemManager() :
    _ElementOffset(0)
{
    DBG(120, "Constructor");
    _OffsetSizes = ElementSizes;
}

SharedMemManager::~SharedMemManager()
{
    DBG(120, "Destructor");
}

void SharedMemManager::setSharedMemBaseAddress(void* BaseAddress)
{
    _BaseAddress = BaseAddress;
    resetSharedMemOffsetAddress();
}

void SharedMemManager::resetSharedMemOffsetAddress()
{
    _OffsetAddress = _BaseAddress;
}

SharedMemAddress_t SharedMemManager::getCurrentOffsetAddress()
{
    return _OffsetAddress;
}

SharedMemAddress_t SharedMemManager::getNextSharedMemAddress()
{
    _OffsetAddress = static_cast<char*>(_OffsetAddress) + _OffsetSizes.at(_ElementOffset);
    _stepSharedMemElementOffset();
    return _OffsetAddress;
}

SharedMemAddress_t SharedMemManager::getNextSharedMemAddress(uint16_t ByteCount)
{
    _OffsetAddress = static_cast<char*>(_OffsetAddress) + ByteCount;
    _stepSharedMemElementOffset();
    return _OffsetAddress;
}

void SharedMemManager::_stepSharedMemElementOffset() {
    _ElementOffset += 1;
    //- TOD: not hardcoded, check struct element count
    if (_ElementOffset == 3) { _ElementOffset = 0; }
}
