#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "../../../src/CustomVector.hpp"
#include <string>
#include <iostream>
#include <sys/mman.h>

using namespace std;

// Helper to allocate shared memory
void* allocate_shared_memory(size_t size) {
    void* shmem = mmap(NULL, size, PROT_READ | PROT_WRITE, 
                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shmem == MAP_FAILED) {
        throw std::bad_alloc();
    }
    return shmem;
}

// Helper to free shared memory
void free_shared_memory(void* ptr, size_t size) {
    munmap(ptr, size);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_basic) {
    cout << "Test CustomVector basic functionality" << endl;
    
    // Test 1: Create vector with segment size and shared memory
    void* shmem = allocate_shared_memory(4096);
    CustomVector<int> vec(sizeof(int), shmem, 4096);
    
    BOOST_CHECK_EQUAL(vec.size(), 0);
    BOOST_CHECK_EQUAL(vec.segment_size(), sizeof(int));
    
    cout << "Created vector with segment size: " << vec.segment_size() << " bytes" << endl;
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_reserve) {
    cout << "Test CustomVector reserve" << endl;
    
    void* shmem = allocate_shared_memory(4096);
    CustomVector<int> vec(sizeof(int), shmem, 4096);
    
    // Test 2: Reserve capacity
    vec.reserve(10);
    BOOST_CHECK_EQUAL(vec.size(), 0);
    BOOST_CHECK_EQUAL(vec.capacity(), 10);
    
    cout << "Reserved capacity: " << vec.capacity() << " elements" << endl;
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_push_back) {
    cout << "Test CustomVector push_back" << endl;
    
    void* shmem = allocate_shared_memory(4096);
    CustomVector<int> vec(sizeof(int), shmem, 4096);
    
    // Test 3: Push back elements
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    
    BOOST_CHECK_EQUAL(vec.size(), 3);
    BOOST_CHECK_EQUAL(vec.at(0), 10);
    BOOST_CHECK_EQUAL(vec.at(1), 20);
    BOOST_CHECK_EQUAL(vec.at(2), 30);
    
    cout << "Pushed 3 elements: " << vec.at(0) << ", " << vec.at(1) << ", " << vec.at(2) << endl;
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_get_element) {
    cout << "Test CustomVector element access" << endl;
    
    void* shmem = allocate_shared_memory(4096);
    CustomVector<int> vec(sizeof(int), shmem, 4096);
    
    // Test 4: Get element at index
    for (int i = 0; i < 5; ++i) {
        vec.push_back(i * 10);
    }
    
    BOOST_CHECK_EQUAL(vec.at(0), 0);
    BOOST_CHECK_EQUAL(vec.at(1), 10);
    BOOST_CHECK_EQUAL(vec.at(2), 20);
    BOOST_CHECK_EQUAL(vec.at(3), 30);
    BOOST_CHECK_EQUAL(vec.at(4), 40);
    
    cout << "Element count: " << vec.size() << endl;
    for (size_t i = 0; i < vec.size(); ++i) {
        cout << "Element[" << i << "] = " << vec.at(i) << endl;
    }
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_size) {
    cout << "Test CustomVector size tracking" << endl;
    
    void* shmem = allocate_shared_memory(4096);
    CustomVector<int> vec(sizeof(int), shmem, 4096);
    
    // Test 5: Size tracking
    BOOST_CHECK_EQUAL(vec.size(), 0);
    
    vec.push_back(1);
    BOOST_CHECK_EQUAL(vec.size(), 1);
    
    vec.push_back(2);
    BOOST_CHECK_EQUAL(vec.size(), 2);
    
    vec.push_back(3);
    BOOST_CHECK_EQUAL(vec.size(), 3);
    
    cout << "Final size: " << vec.size() << endl;
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_different_types) {
    cout << "Test CustomVector with different types" << endl;
    
    // Test 6: Test with double
    void* shmem1 = allocate_shared_memory(4096);
    {
        CustomVector<double> vec_double(sizeof(double), shmem1, 4096);
        vec_double.push_back(3.14);
        vec_double.push_back(2.71);
        
        BOOST_CHECK_CLOSE(vec_double.at(0), 3.14, 0.001);
        BOOST_CHECK_CLOSE(vec_double.at(1), 2.71, 0.001);
        
        cout << "Double vector: " << vec_double.at(0) << ", " << vec_double.at(1) << endl;
    } // vec_double destroyed here before munmap
    free_shared_memory(shmem1, 4096);
    
    // Test 7: Test with string
    void* shmem2 = allocate_shared_memory(4096);
    {
        CustomVector<string> vec_string(sizeof(string), shmem2, 4096);
        vec_string.push_back("Hello");
        vec_string.push_back("World");
        
        BOOST_CHECK_EQUAL(vec_string.at(0), "Hello");
        BOOST_CHECK_EQUAL(vec_string.at(1), "World");
        
        cout << "String vector: " << vec_string.at(0) << ", " << vec_string.at(1) << endl;
    } // vec_string destroyed here before munmap
    free_shared_memory(shmem2, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_memory_layout) {
    cout << "Test CustomVector memory layout" << endl;
    
    // Test 8: Verify memory layout with custom segment size
    // Use segment size larger than sizeof(int) to test the address calculation
    size_t segment_size = 16; // 16 bytes per segment
    void* shmem = allocate_shared_memory(4096);
    CustomVector<int> vec(segment_size, shmem, 4096);
    
    vec.reserve(5);
    vec.push_back(100);
    vec.push_back(200);
    vec.push_back(300);
    
    BOOST_CHECK_EQUAL(vec.segment_size(), segment_size);
    BOOST_CHECK_EQUAL(vec.at(0), 100);
    BOOST_CHECK_EQUAL(vec.at(1), 200);
    BOOST_CHECK_EQUAL(vec.at(2), 300);
    
    cout << "Custom segment size: " << segment_size << " bytes" << endl;
    cout << "Elements: " << vec.at(0) << ", " << vec.at(1) << ", " << vec.at(2) << endl;
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_out_of_bounds) {
    cout << "Test CustomVector out of bounds exception" << endl;
    
    void* shmem = allocate_shared_memory(4096);
    CustomVector<int> vec(sizeof(int), shmem, 4096);
    vec.push_back(42);
    
    // Test 9: Out of bounds access should throw
    BOOST_CHECK_THROW(vec.at(1), std::out_of_range);
    BOOST_CHECK_THROW(vec.at(100), std::out_of_range);
    
    cout << "Out of bounds checks passed" << endl;
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_shared_memory) {
    cout << "Test CustomVector with actual mmap shared memory" << endl;
    
    // Test 10: Use mmap as specified in requirements
    void* shmpointer = mmap(NULL, 640000, PROT_READ | PROT_WRITE, 
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    BOOST_CHECK(shmpointer != MAP_FAILED);
    
    CustomVector<int> vec(sizeof(int), shmpointer, 640000);
    
    // Add many elements to test larger memory usage
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i * 100);
    }
    
    BOOST_CHECK_EQUAL(vec.size(), 100);
    BOOST_CHECK_EQUAL(vec.at(0), 0);
    BOOST_CHECK_EQUAL(vec.at(50), 5000);
    BOOST_CHECK_EQUAL(vec.at(99), 9900);
    
    cout << "Created vector in 640KB shared memory with 100 elements" << endl;
    cout << "First element: " << vec.at(0) << ", Middle: " << vec.at(50) << ", Last: " << vec.at(99) << endl;
    
    munmap(shmpointer, 640000);
}
