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
    cout << "Test: malloc provides proper default alignment" << endl;
    
    void* ptr = malloc(100);
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    
    // malloc must align to max_align_t (typically 16 bytes on x86_64)
    BOOST_CHECK_EQUAL(addr % alignof(max_align_t), 0);
    
    free(ptr);
    
    cout << "  malloc aligns to: " << alignof(max_align_t) << " bytes ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_memorymanager_base_alignment )
{
    cout << "Test: MemoryManager base address is properly aligned" << endl;
    
    // Test with different types
    MemoryManager<char> mgr_char(10, 100);
    BOOST_CHECK_EQUAL(reinterpret_cast<uintptr_t>(mgr_char.getMemBaseAddress()) % alignof(max_align_t), 0);
    
    MemoryManager<uint16_t> mgr_u16(10, 100);
    BOOST_CHECK_EQUAL(reinterpret_cast<uintptr_t>(mgr_u16.getMemBaseAddress()) % alignof(uint16_t), 0);
    
    MemoryManager<uint32_t> mgr_u32(10, 100);
    BOOST_CHECK_EQUAL(reinterpret_cast<uintptr_t>(mgr_u32.getMemBaseAddress()) % alignof(uint32_t), 0);
    
    MemoryManager<atomic<uint16_t>> mgr_atomic(10, 100);
    BOOST_CHECK_EQUAL(reinterpret_cast<uintptr_t>(mgr_atomic.getMemBaseAddress()) % alignof(atomic<uint16_t>), 0);
    
    cout << "  All base addresses properly aligned ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_compile_time_alignment )
{
    cout << "Test: Compile-time alignment information" << endl;
    
    // Check that alignment is available at compile-time
    constexpr size_t align_char = MemoryManager<char>::Alignment;
    constexpr size_t align_u16 = MemoryManager<uint16_t>::Alignment;
    constexpr size_t align_u32 = MemoryManager<uint32_t>::Alignment;
    
    BOOST_CHECK_EQUAL(align_char, alignof(char));
    BOOST_CHECK_EQUAL(align_u16, alignof(uint16_t));
    BOOST_CHECK_EQUAL(align_u32, alignof(uint32_t));
    
    // Check getAlignment() method
    BOOST_CHECK_EQUAL(MemoryManager<char>::getAlignment(), alignof(char));
    BOOST_CHECK_EQUAL(MemoryManager<uint16_t>::getAlignment(), alignof(uint16_t));
    
    cout << "  Compile-time alignment: char=" << align_char 
         << ", uint16_t=" << align_u16 
         << ", uint32_t=" << align_u32 << " ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_char_pointer_arithmetic )
{
    cout << "Test: Pointer arithmetic for char type (sizeof=1)" << endl;
    
    // SegmentSize is in BYTES
    // For char: getNextMemPointer advances by (SegmentSize * sizeof(char)) elements = SegmentSize bytes
    MemoryManager<char> mgr(15, 11);  // 15 segments, 11 bytes each
    char* base = mgr.getMemBaseAddress();
    
    char* ptr1 = mgr.getNextMemPointer();
    BOOST_CHECK_EQUAL(ptr1, base + 11);  // base + (11 * 1) bytes
    
    char* ptr2 = mgr.getNextMemPointer();
    BOOST_CHECK_EQUAL(ptr2, base + 22);  // base + (2 * 11 * 1) bytes
    
    char* ptr3 = mgr.getNextMemPointer();
    BOOST_CHECK_EQUAL(ptr3, base + 33);  // base + (3 * 11 * 1) bytes
    
    cout << "  char pointers: offset 1 at +" << (ptr1 - base) << " bytes, "
         << "offset 2 at +" << (ptr2 - base) << " bytes, "
         << "offset 3 at +" << (ptr3 - base) << " bytes ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_uint16_pointer_arithmetic )
{
    cout << "Test: Pointer arithmetic for uint16_t type (sizeof=2)" << endl;
    
    // SegmentSize is in BYTES
    // For uint16_t: getNextMemPointer advances by (SegmentSize * sizeof(uint16_t)) elements
    MemoryManager<uint16_t> mgr(10, 20);  // 10 segments, 20 bytes each
    uint16_t* base = mgr.getMemBaseAddress();
    
    uint16_t* ptr1 = mgr.getNextMemPointer();
    // offset 1: base + (1 * 20 * 2) elements = base + 40 elements
    BOOST_CHECK_EQUAL(ptr1, base + 40);
    
    uint16_t* ptr2 = mgr.getNextMemPointer();
    // offset 2: base + (2 * 20 * 2) elements = base + 80 elements
    BOOST_CHECK_EQUAL(ptr2, base + 80);
    
    uint16_t* ptr3 = mgr.getNextMemPointer();
    // offset 3: base + (3 * 20 * 2) elements = base + 120 elements
    BOOST_CHECK_EQUAL(ptr3, base + 120);
    
    cout << "  uint16_t pointers: offset 1 at +" << (ptr1 - base) << " elements, "
         << "offset 2 at +" << (ptr2 - base) << " elements, "
         << "offset 3 at +" << (ptr3 - base) << " elements ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_uint32_pointer_arithmetic )
{
    cout << "Test: Pointer arithmetic for uint32_t type (sizeof=4)" << endl;
    
    // SegmentSize is in BYTES
    // For uint32_t: getNextMemPointer advances by (SegmentSize * sizeof(uint32_t)) elements
    MemoryManager<uint32_t> mgr(8, 16);  // 8 segments, 16 bytes each
    uint32_t* base = mgr.getMemBaseAddress();
    
    uint32_t* ptr1 = mgr.getNextMemPointer();
    // offset 1: base + (1 * 16 * 4) elements = base + 64 elements
    BOOST_CHECK_EQUAL(ptr1, base + 64);
    
    uint32_t* ptr2 = mgr.getNextMemPointer();
    // offset 2: base + (2 * 16 * 4) elements = base + 128 elements
    BOOST_CHECK_EQUAL(ptr2, base + 128);
    
    uint32_t* ptr3 = mgr.getNextMemPointer();
    // offset 3: base + (3 * 16 * 4) elements = base + 192 elements
    BOOST_CHECK_EQUAL(ptr3, base + 192);
    
    cout << "  uint32_t pointers: offset 1 at +" << (ptr1 - base) << " elements, "
         << "offset 2 at +" << (ptr2 - base) << " elements, "
         << "offset 3 at +" << (ptr3 - base) << " elements ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_int64_pointer_arithmetic )
{
    cout << "Test: Pointer arithmetic for int64_t type (sizeof=8)" << endl;
    
    // SegmentSize is in BYTES
    // For int64_t: getNextMemPointer advances by (SegmentSize * sizeof(int64_t)) elements
    MemoryManager<int64_t> mgr(27, 16);  // 27 segments, 16 bytes each
    int64_t* base = mgr.getMemBaseAddress();
    
    int64_t* ptr1 = mgr.getNextMemPointer();
    // offset 1: base + (1 * 16 * 8) elements = base + 128 elements
    BOOST_CHECK_EQUAL(ptr1, base + 128);
    
    int64_t* ptr2 = mgr.getNextMemPointer();
    // offset 2: base + (2 * 16 * 8) elements = base + 256 elements
    BOOST_CHECK_EQUAL(ptr2, base + 256);
    
    int64_t* ptr3 = mgr.getNextMemPointer();
    // offset 3: base + (3 * 16 * 8) elements = base + 384 elements
    BOOST_CHECK_EQUAL(ptr3, base + 384);
    
    cout << "  int64_t pointers: offset 1 at +" << (ptr1 - base) << " elements, "
         << "offset 2 at +" << (ptr2 - base) << " elements, "
         << "offset 3 at +" << (ptr3 - base) << " elements ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_short_pointer_arithmetic )
{
    cout << "Test: Pointer arithmetic for short type (sizeof=2)" << endl;
    
    // SegmentSize is in BYTES
    MemoryManager<short> mgr(120, 38);  // 120 segments, 38 bytes each
    short* base = mgr.getMemBaseAddress();
    
    short* ptr1 = mgr.getNextMemPointer();
    // offset 1: base + (1 * 38 * 2) elements = base + 76 elements
    BOOST_CHECK_EQUAL(ptr1, base + 76);
    
    short* ptr2 = mgr.getNextMemPointer();
    // offset 2: base + (2 * 38 * 2) elements = base + 152 elements
    BOOST_CHECK_EQUAL(ptr2, base + 152);
    
    short* ptr3 = mgr.getNextMemPointer();
    // offset 3: base + (3 * 38 * 2) elements = base + 228 elements
    BOOST_CHECK_EQUAL(ptr3, base + 228);
    
    cout << "  short pointers: offset 1 at +" << (ptr1 - base) << " elements, "
         << "offset 2 at +" << (ptr2 - base) << " elements, "
         << "offset 3 at +" << (ptr3 - base) << " elements ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_segment_wraparound )
{
    cout << "Test: Segment offset wraparound when exceeding SegmentCount" << endl;
    
    // When SegmentOffset >= SegmentCount, it should wrap to 0
    MemoryManager<int64_t> mgr(5, 24);  // 5 segments, 24 bytes each
    int64_t* base = mgr.getMemBaseAddress();
    
    int64_t* ptr1 = mgr.getNextMemPointer();
    BOOST_CHECK_EQUAL(ptr1, base + (1 * 24 * 8));  // 192 elements
    
    int64_t* ptr2 = mgr.getNextMemPointer();
    BOOST_CHECK_EQUAL(ptr2, base + (2 * 24 * 8));  // 384 elements
    
    int64_t* ptr3 = mgr.getNextMemPointer();
    BOOST_CHECK_EQUAL(ptr3, base + (3 * 24 * 8));  // 576 elements
    
    int64_t* ptr4 = mgr.getNextMemPointer();
    BOOST_CHECK_EQUAL(ptr4, base + (4 * 24 * 8));  // 768 elements
    
    // 5th call: SegmentOffset becomes 5, which is >= SegmentCount (5), so wraps to 0
    int64_t* ptr5 = mgr.getNextMemPointer();
    BOOST_CHECK_EQUAL(ptr5, base);  // Back to base
    
    cout << "  Wraparound works correctly: returns to base after " << 5 << " segments ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_alignment_preservation )
{
    cout << "Test: All segment pointers maintain proper alignment" << endl;
    
    MemoryManager<uint32_t> mgr(10, 256);  // 10 segments, 256 bytes each
    uint32_t* base = mgr.getMemBaseAddress();
    
    // Check alignment for all segments
    for (int i = 0; i < 10; i++) {
        uint32_t* ptr = mgr.getNextMemPointer();
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        
        // Must be aligned for uint32_t (4 bytes)
        BOOST_CHECK_EQUAL(addr % alignof(uint32_t), 0);
    }
    
    cout << "  All 10 segment pointers properly aligned for uint32_t ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_isAligned_utility )
{
    cout << "Test: isAligned() utility function" << endl;
    
    MemoryManager<uint32_t> mgr(5, 100);
    uint32_t* base = mgr.getMemBaseAddress();
    
    // Base should be aligned
    BOOST_CHECK(MemoryManager<uint32_t>::isAligned(base));
    
    // All segment pointers should be aligned
    for (int i = 0; i < 5; i++) {
        uint32_t* ptr = mgr.getNextMemPointer();
        BOOST_CHECK(MemoryManager<uint32_t>::isAligned(ptr));
    }
    
    cout << "  isAligned() correctly identifies aligned pointers ✓" << endl;
}


BOOST_AUTO_TEST_CASE( test_semantics_summary )
{
    cout << "\n=== MemoryManager Semantics Summary ===" << endl;
    cout << "SegmentSize: represents BYTES per segment" << endl;
    cout << "Allocation: SegmentCount * SegmentSize bytes" << endl;
    cout << "Pointer arithmetic: base + (offset * SegmentSize * sizeof(T)) elements" << endl;
    cout << "Formula in code: MemPointer += (SegmentOffset * SegmentSize) * sizeof(T)" << endl;
    cout << "========================================" << endl;
    
    // This is a documentation test - always passes
    BOOST_CHECK(true);
}
