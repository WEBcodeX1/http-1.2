#ifndef MemoryManager_hpp
#define MemoryManager_hpp

#include "Debug.cpp"

#include <sys/mman.h>
#include <cstdlib>
#include <memory>


using namespace std;

template<class T>
class MemoryManager
{

public:

    // alignment requirement for type T (compile-time constant)
    static constexpr size_t Alignment = alignof(T);

    // do not use with types like STL containers using dynamic heap allocation
    MemoryManager(uint16_t SegmentCount, uint16_t SegmentSize):
        SegmentCount(SegmentCount),
        SegmentSize(SegmentSize)
    {
        DBG(120, "Contructor");
        SegmentOffset = 0;
        allocateMemory();

        #if defined(DEBUG_BUILD)
        // verify alignment in debug builds
        verifyAlignment();
        #endif
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
    
    // get compile-time alignment requirement
    static constexpr size_t getAlignment() {
        return Alignment;
    }
    
    // check if a pointer is properly aligned for type T
    static bool isAligned(const void* ptr) {
        return reinterpret_cast<uintptr_t>(ptr) % Alignment == 0;
    }

private:

    uint16_t SegmentCount;
    uint16_t SegmentSize;
    uint16_t SegmentOffset;

    T* MemoryBaseAddress;

    void allocateMemory() {
        unsigned int MemSizeBytes = SegmentCount*SegmentSize;

        MemoryBaseAddress = static_cast<T*>(malloc(MemSizeBytes));

        if (MemoryBaseAddress == nullptr) {
            ERR("Failed to allocate memory");
            throw std::bad_alloc();
        }

        madvise(MemoryBaseAddress, MemSizeBytes, MADV_HUGEPAGE);
        DBG(95, "Allocate Memory Address:" << static_cast<void*>(MemoryBaseAddress));
    }

    void verifyAlignment() {
        if (!isAligned(MemoryBaseAddress)) {
            DBG(10, "Memory base address not properly aligned for type T");
            DBG(10, "Required alignment: " << Alignment << " bytes");
            DBG(10, "Address: " << static_cast<void*>(MemoryBaseAddress));
        }
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
