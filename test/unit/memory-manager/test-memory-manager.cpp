#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <memory>

#include "../../../src/MemoryManager.hpp"

using namespace std;

// 8bit (1 byte)
BOOST_AUTO_TEST_CASE( test_basic_memory_manager_char_type )
{
    MemoryManager<char> BufferMemory(15, 11);
    char* BaseAddr = BufferMemory.getMemBaseAddress();
    char* MemPointer;
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr)+11);
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr)+(2*11));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr)+(3*11));
}

// 64bit (8 byte)
BOOST_AUTO_TEST_CASE( test_basic_memory_manager_int64_type )
{
    MemoryManager<int64_t> BufferMemory(27, 13);
    int64_t* BaseAddr = BufferMemory.getMemBaseAddress();
    int64_t* MemPointer;
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+(13*8)));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+2*(13*8)));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+3*(13*8)));
}

// 16bit (2 byte)
BOOST_AUTO_TEST_CASE( test_basic_memory_manager_short_type )
{
    MemoryManager<short> BufferMemory(120, 37);
    short* BaseAddr = BufferMemory.getMemBaseAddress();
    short* MemPointer;
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+(37*2)));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+2*(37*2)));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+3*(37*2)));
}
