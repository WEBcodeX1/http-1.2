#include "IPCHandler.hpp"

//- convert to C++ 20 iterator / C++ 23 generator
using namespace std;

SHMStaticFS::SHMStaticFS() :
    _ElementOffset(0)
{
    DBG(120, "Constructor");
    _OffsetSizes = ElementSizes;
}

SHMStaticFS::~SHMStaticFS()
{
    DBG(120, "Destructor");
}

void SHMStaticFS::setBaseAddress(SharedMemAddress_t BaseAddress)
{
    _BaseAddress = BaseAddress;
    resetOffset();
}

void SHMStaticFS::resetOffset()
{
    _OffsetAddress = _BaseAddress;
}

SharedMemAddress_t SHMStaticFS::getCurrentOffsetAddress()
{
    return _OffsetAddress;
}

SharedMemAddress_t SHMStaticFS::getNextAddress()
{
    _OffsetAddress = static_cast<char*>(_OffsetAddress) + _OffsetSizes.at(_ElementOffset);
    _stepElementOffset();
    return _OffsetAddress;
}

SharedMemAddress_t SHMStaticFS::getNextAddress(uint16_t ByteCount)
{
    _OffsetAddress = static_cast<char*>(_OffsetAddress) + ByteCount;
    _stepElementOffset();
    return _OffsetAddress;
}

void SHMStaticFS::_stepElementOffset() {
    _ElementOffset += 1;
    if (_ElementOffset == ElementSizes.size()) { _ElementOffset = 0; }
}
