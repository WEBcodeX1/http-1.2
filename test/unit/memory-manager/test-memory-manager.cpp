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
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr)+(1*11));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr)+(2*11));
}

// 64bit (8 byte)
BOOST_AUTO_TEST_CASE( test_basic_memory_manager_int64_type )
{
    MemoryManager<int64_t> BufferMemory(27, 16);
    int64_t* BaseAddr = BufferMemory.getMemBaseAddress();
    int64_t* MemPointer;
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+1*(16*8)));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+2*(16*8)));
}

// 16bit (2 byte)
BOOST_AUTO_TEST_CASE( test_basic_memory_manager_short_type )
{
    MemoryManager<short> BufferMemory(120, 38);
    short* BaseAddr = BufferMemory.getMemBaseAddress();
    short* MemPointer;
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+1*(38*2)));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+2*(38*2)));
}

// 64bit (8 byte) out of bounds check
BOOST_AUTO_TEST_CASE( test_basic_memory_manager_int64_type_oob )
{
    MemoryManager<int64_t> BufferMemory(5, 24);
    int64_t* BaseAddr = BufferMemory.getMemBaseAddress();
    int64_t* MemPointer;
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+1*(24*8)));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+2*(24*8)));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+3*(24*8)));
    MemPointer = BufferMemory.getNextMemPointer();
    BOOST_CHECK_EQUAL(MemPointer, static_cast<void*>(BaseAddr+4*(24*8)));
    MemPointer = BufferMemory.getNextMemPointer();
}

BOOST_AUTO_TEST_CASE( test_memory_manager_wraps_after_segment_count )
{
    MemoryManager<char> BufferMemory(3, 4);
    char* BaseAddr = BufferMemory.getMemBaseAddress();

    BOOST_CHECK_EQUAL(BufferMemory.getNextMemPointer(), static_cast<void*>(BaseAddr));
    BOOST_CHECK_EQUAL(BufferMemory.getNextMemPointer(), static_cast<void*>(BaseAddr + 4));
    BOOST_CHECK_EQUAL(BufferMemory.getNextMemPointer(), static_cast<void*>(BaseAddr + 8));
    BOOST_CHECK_EQUAL(BufferMemory.getNextMemPointer(), static_cast<void*>(BaseAddr));
    BOOST_CHECK_EQUAL(BufferMemory.getNextMemPointer(), static_cast<void*>(BaseAddr + 4));
}
