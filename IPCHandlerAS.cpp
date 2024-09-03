#include "IPCHandlerAS.hpp"

using namespace std;

SHMPythonAS::SHMPythonAS()
{
    DBG(120, "Constructor");
    uint16_t Offset = 0;
    for (const auto &Size:MetadataSizes) {
        Offset += static_cast<uint16_t>(Size);
        _MetadataOffsets.push_back(Offset);
        DBG(120, "Push Back Offset:" << Offset);
    }
    _MetaSegmentSize = Offset;
}

SHMPythonAS::~SHMPythonAS()
{
    DBG(120, "Destructor");
}

void SHMPythonAS::setBaseAddresses(const BaseAdresses_t BaseAdresses)
{
    _BaseAdresses = BaseAdresses;
    DBG(120, "Addresses Meta:" << BaseAdresses.PostASMetaPtr << " Requests:" << BaseAdresses.PostASRequestsPtr << " Results:" << BaseAdresses.PostASResultsPtr);
}

SharedMemAddress_t SHMPythonAS::getMetaAddress(const uint8_t SegmentIndex, const uint8_t MetaIndex)
{
    return static_cast<char*>(_BaseAdresses.PostASMetaPtr) + (SegmentIndex * _MetaSegmentSize) + _MetadataOffsets[MetaIndex];
}

SharedMemAddress_t SHMPythonAS::getRequestAddress(const uint8_t SegmentIndex)
{
    return static_cast<char*>(_BaseAdresses.PostASRequestsPtr) + (SegmentIndex * HTTP_REQUEST_MAX_SIZE);
}

SharedMemAddress_t SHMPythonAS::getResultAddress(const uint8_t SegmentIndex)
{
    return static_cast<char*>(_BaseAdresses.PostASResultsPtr) + (SegmentIndex * HTTP_REQUEST_MAX_SIZE);
}
