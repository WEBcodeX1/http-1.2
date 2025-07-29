#ifndef MemoryManager_hpp
#define MemoryManager_hpp

#include "Debug.cpp"

#include <sys/mman.h>


using namespace std;

template<class T>
class MemoryManager
{

public:

    //- do not use with types like STL containers using dynamic heap allocation
    MemoryManager(uint16_t SegmentCount, uint16_t SegmentSize):
        SegmentCount(SegmentCount),
        SegmentSize(SegmentSize)
    {
        DBG(120, "Contructor");
        SegmentOffset = 0;
        allocateMemory();
    }

    ~MemoryManager() {
        DBG(120, "Destructor");
        free(MemoryBaseAddress);
        DBG(120, "Freed memory");
    }

    T* getNextMemPointer() {
        SegmentOffset >= SegmentCount ? 0 : SegmentOffset++;
        return getMemPointer(SegmentOffset);
    }

    T* getMemBaseAddress() {
        return MemoryBaseAddress;
    }

private:

    uint16_t SegmentCount;
    uint16_t SegmentSize;
    uint16_t SegmentOffset;

    T* MemoryBaseAddress;

    void allocateMemory() {
        unsigned int MemSizeBytes = SegmentCount*sizeof(T)*SegmentSize;
        MemoryBaseAddress = static_cast<T*>(malloc(MemSizeBytes));
        madvise(MemoryBaseAddress, MemSizeBytes, MADV_HUGEPAGE);
        DBG(95, "Allocate Memory Address:" << static_cast<void*>(MemoryBaseAddress));
    }

    T* getMemPointer(uint16_t SegmentOffset) {

        T* MemPointer = MemoryBaseAddress;

        if (SegmentOffset < SegmentCount) {
            MemPointer += (SegmentOffset*SegmentSize)*sizeof(T);
        }

        DBG(180, "Memory Base Address:" << static_cast<void*>(MemoryBaseAddress) << " SegmentOffset:" << SegmentOffset << " MemPointer Address:" << static_cast<void*>(MemPointer));

        return MemPointer;

    }

};

#endif
