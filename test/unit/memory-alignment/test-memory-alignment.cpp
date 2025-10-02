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
