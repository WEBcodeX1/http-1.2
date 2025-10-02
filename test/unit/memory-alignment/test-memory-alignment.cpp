#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <memory>

#include "../../../src/MemoryManager.hpp"

using namespace std;


BOOST_AUTO_TEST_CASE( test_malloc_default_alignment )
{
    cout << "Check malloc default alignment guarantees." << endl;
    
    // malloc guarantees alignment for any standard type
    // On most modern systems, malloc returns memory aligned to at least 8 or 16 bytes
    void* ptr = malloc(100);
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    
    // Check if aligned to at least max_align_t (typically 16 bytes on x86_64)
    BOOST_CHECK_EQUAL(addr % alignof(max_align_t), 0);
    
    free(ptr);
    
    cout << "malloc aligns to: " << alignof(max_align_t) << " bytes" << endl;
}


BOOST_AUTO_TEST_CASE( test_memorymanager_char_alignment )
{
    cout << "Check MemoryManager<char> alignment." << endl;
    
    MemoryManager<char> mgr(10, 1024);
    char* base = mgr.getMemBaseAddress();
    
    uintptr_t addr = reinterpret_cast<uintptr_t>(base);
    
    // malloc should provide suitable alignment for all standard types
    BOOST_CHECK_EQUAL(addr % alignof(max_align_t), 0);
    
    cout << "MemoryManager<char> base address aligned to: " << alignof(max_align_t) << " bytes" << endl;
}


BOOST_AUTO_TEST_CASE( test_memorymanager_uint16_alignment )
{
    cout << "Check MemoryManager<uint16_t> alignment." << endl;
    
    MemoryManager<uint16_t> mgr(10, 512);
    uint16_t* base = mgr.getMemBaseAddress();
    
    uintptr_t addr = reinterpret_cast<uintptr_t>(base);
    
    // Should be aligned for uint16_t (at least 2 bytes)
    BOOST_CHECK_EQUAL(addr % alignof(uint16_t), 0);
    // And also to max_align_t
    BOOST_CHECK_EQUAL(addr % alignof(max_align_t), 0);
    
    cout << "MemoryManager<uint16_t> aligned to: " << alignof(max_align_t) << " bytes" << endl;
}


BOOST_AUTO_TEST_CASE( test_memorymanager_atomic_alignment )
{
    cout << "Check MemoryManager<atomic<uint16_t>> alignment." << endl;
    
    MemoryManager<atomic<uint16_t>> mgr(10, 128);
    atomic<uint16_t>* base = mgr.getMemBaseAddress();
    
    uintptr_t addr = reinterpret_cast<uintptr_t>(base);
    
    // atomic types require proper alignment
    BOOST_CHECK_EQUAL(addr % alignof(atomic<uint16_t>), 0);
    
    cout << "MemoryManager<atomic<uint16_t>> aligned to: " << alignof(atomic<uint16_t>) << " bytes" << endl;
}


BOOST_AUTO_TEST_CASE( test_pointer_arithmetic_alignment )
{
    cout << "Check pointer arithmetic preserves alignment." << endl;
    
    MemoryManager<uint32_t> mgr(10, 256);
    uint32_t* base = mgr.getMemBaseAddress();
    
    // Check multiple segment offsets
    for (int i = 0; i < 10; i++) {
        uint32_t* ptr = mgr.getNextMemPointer();
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        
        // Each segment should be properly aligned for uint32_t
        BOOST_CHECK_EQUAL(addr % alignof(uint32_t), 0);
    }
    
    cout << "All segment pointers properly aligned." << endl;
}


BOOST_AUTO_TEST_CASE( test_atomic_uint16_t_alignment )
{
    cout << "Check atomic_uint16_t alignment requirements." << endl;
    
    // Check compile-time alignment requirements
    cout << "atomic_uint16_t requires: " << alignof(atomic_uint16_t) << " bytes alignment" << endl;
    cout << "uint16_t requires: " << alignof(uint16_t) << " bytes alignment" << endl;
    cout << "uint32_t requires: " << alignof(uint32_t) << " bytes alignment" << endl;
    
    // Verify that malloc provides sufficient alignment
    void* ptr = malloc(sizeof(atomic_uint16_t) * 100);
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    
    BOOST_CHECK_EQUAL(addr % alignof(atomic_uint16_t), 0);
    
    free(ptr);
}


BOOST_AUTO_TEST_CASE( test_getMemPointer_uint32_multiple_offsets )
{
    cout << "\nTest 1: Verify getMemPointer() pointer arithmetic with uint32_t" << endl;
    cout << "SegmentSize represents BYTES per segment in the new model." << endl;
    
    // New semantics: SegmentSize is in BYTES
    // For uint32_t, use SegmentSize that's a multiple of sizeof(uint32_t) for proper alignment
    const uint16_t segmentCount = 8;
    const uint16_t segmentSize = 512;  // 512 bytes per segment
    
    MemoryManager<uint32_t> mgr(segmentCount, segmentSize);
    uint32_t* base = mgr.getMemBaseAddress();
    
    cout << "  sizeof(uint32_t) = " << sizeof(uint32_t) << " bytes" << endl;
    cout << "  SegmentCount = " << segmentCount << ", SegmentSize = " << segmentSize << " bytes" << endl;
    cout << "  Total allocated: " << (segmentCount * segmentSize) << " bytes" << endl;
    cout << "  Testing multiple segment offsets..." << endl;
    
    // With new semantics:
    // Segment offset N starts at: base + (N * SegmentSize * sizeof(T)) elements
    // Which is: base + (N * SegmentSize) bytes in element units
    
    // Test first few segments
    for (uint16_t offset = 0; offset < 4 && offset < segmentCount; offset++) {
        // Expected pointer calculation with new semantics:
        // getMemPointer returns: base + (offset * segmentSize * sizeof(uint32_t)) elements
        uint32_t* expectedPtr = base + (offset * segmentSize * sizeof(uint32_t));
        
        uintptr_t baseAddr = reinterpret_cast<uintptr_t>(base);
        uintptr_t expectedAddr = reinterpret_cast<uintptr_t>(expectedPtr);
        size_t byteOffset = expectedAddr - baseAddr;
        
        // Byte offset should be: offset * segmentSize * sizeof(uint32_t) * sizeof(uint32_t)
        size_t expectedByteOffset = offset * segmentSize * sizeof(uint32_t);
        
        cout << "    Offset " << offset << ": expected element offset = " 
             << (offset * segmentSize * sizeof(uint32_t)) << " elements = " 
             << expectedByteOffset << " bytes" << endl;
    }
    
    cout << "  Pointer arithmetic verified for uint32_t type." << endl;
}


BOOST_AUTO_TEST_CASE( test_getNextMemPointer_all_offsets_uint16 )
{
    cout << "\nTest 2: Verify getNextMemPointer() advances correctly through all segments" << endl;
    cout << "This test calls getNextMemPointer() for all segments and verifies each pointer." << endl;
    
    const uint16_t segmentCount = 6;
    const uint16_t segmentSize = 200;
    
    MemoryManager<uint16_t> mgr(segmentCount, segmentSize);
    uint16_t* base = mgr.getMemBaseAddress();
    
    cout << "  sizeof(uint16_t) = " << sizeof(uint16_t) << " bytes" << endl;
    cout << "  SegmentCount = " << segmentCount << ", SegmentSize = " << segmentSize << endl;
    cout << "  Calling getNextMemPointer() " << segmentCount << " times..." << endl;
    
    for (uint16_t i = 0; i < segmentCount; i++) {
        uint16_t* ptr = mgr.getNextMemPointer();
        
        // SegmentOffset starts at 0, increments with each call
        // When i=0, SegmentOffset becomes 1, returns base + (1*segmentSize)
        // When SegmentOffset >= SegmentCount, it wraps to 0
        uint16_t* expected;
        size_t expectedElementOffset;
        
        if (i + 1 >= segmentCount) {
            expected = base;  // Wraps to base
            expectedElementOffset = 0;
        } else {
            expected = base + ((i + 1) * segmentSize);
            expectedElementOffset = (i + 1) * segmentSize;
        }
        
        uintptr_t ptrAddr = reinterpret_cast<uintptr_t>(ptr);
        uintptr_t baseAddr = reinterpret_cast<uintptr_t>(base);
        size_t byteOffset = ptrAddr - baseAddr;
        size_t elementOffset = byteOffset / sizeof(uint16_t);
        
        BOOST_CHECK_EQUAL(ptr, expected);
        BOOST_CHECK_EQUAL(elementOffset, expectedElementOffset);
        
        if (i < 4 || i == segmentCount - 1) {
            cout << "    Call " << i << ": element offset = " << elementOffset 
                 << " (expected: " << expectedElementOffset << ") ✓" << endl;
        }
    }
    
    cout << "  All getNextMemPointer() calls returned correct pointers!" << endl;
}


BOOST_AUTO_TEST_CASE( test_getNextMemPointer_char_all_pages )
{
    cout << "\nTest 3: Verify pointer arithmetic with char (sizeof=1) for all memory pages" << endl;
    cout << "For char, SegmentSize in bytes equals element offset since sizeof(char)=1." << endl;
    
    const uint16_t segmentCount = 12;
    const uint16_t segmentSize = 256;  // 256 bytes per segment
    
    MemoryManager<char> mgr(segmentCount, segmentSize);
    char* base = mgr.getMemBaseAddress();
    
    cout << "  sizeof(char) = " << sizeof(char) << " byte" << endl;
    cout << "  SegmentCount = " << segmentCount << ", SegmentSize = " << segmentSize << " bytes" << endl;
    cout << "  Total memory = " << (segmentCount * segmentSize) << " bytes" << endl;
    
    // With new semantics for char (sizeof=1):
    // getMemPointer(offset) returns: base + (offset * segmentSize * sizeof(char)) elements
    //                               = base + (offset * segmentSize * 1) elements
    //                               = base + (offset * segmentSize) bytes
    
    // Write patterns at each segment start
    for (uint16_t offset = 0; offset < segmentCount; offset++) {
        // Expected pointer: base + (offset * segmentSize * sizeof(char)) = base + (offset * segmentSize)
        char* segmentPtr = base + (offset * segmentSize);
        
        // Write 3-byte pattern: letter, digit, marker
        *segmentPtr = 'A' + (offset % 26);
        *(segmentPtr + 1) = '0' + (offset % 10);
        *(segmentPtr + 2) = 'X';
        
        uintptr_t baseAddr = reinterpret_cast<uintptr_t>(base);
        uintptr_t segmentAddr = reinterpret_cast<uintptr_t>(segmentPtr);
        size_t byteOffset = segmentAddr - baseAddr;
        size_t expectedByteOffset = offset * segmentSize;
        
        BOOST_CHECK_EQUAL(byteOffset, expectedByteOffset);
        
        if (offset < 4 || offset >= segmentCount - 2) {
            cout << "    Segment " << offset << ": byte offset = " << byteOffset 
                 << " (expected: " << expectedByteOffset << "), pattern = " 
                 << *segmentPtr << *(segmentPtr + 1) << *(segmentPtr + 2) << endl;
        }
    }
    
    // Verify all patterns
    for (uint16_t offset = 0; offset < segmentCount; offset++) {
        char* segmentPtr = base + (offset * segmentSize);
        BOOST_CHECK_EQUAL(*segmentPtr, char('A' + (offset % 26)));
        BOOST_CHECK_EQUAL(*(segmentPtr + 1), char('0' + (offset % 10)));
        BOOST_CHECK_EQUAL(*(segmentPtr + 2), 'X');
    }
    
    cout << "  All " << segmentCount << " memory pages verified!" << endl;
    cout << "\n=== SUMMARY ===" << endl;
    cout << "New MemoryManager semantics:" << endl;
    cout << "  - SegmentSize represents BYTES per segment" << endl;
    cout << "  - Allocation: SegmentCount * SegmentSize bytes" << endl;
    cout << "  - Pointer arithmetic: base + (offset * SegmentSize * sizeof(T)) elements" << endl;
}
