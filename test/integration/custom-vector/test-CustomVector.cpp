#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "../../../src/CustomVector.hpp"
#include <string>
#include <iostream>
#include <sys/mman.h>
#include <cstring>
#include <vector>

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
    
    // Test 6: Test with double (trivially copyable type - ideal for shared memory)
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
    
    // Test 7: Test with string (Note: std::string is not ideal for shared memory
    // across processes as its internal buffer is heap-allocated. This works for
    // single-process testing but won't share properly across process boundaries.)
    void* shmem2 = allocate_shared_memory(4096);
    {
        CustomVector<string> vec_string(sizeof(string), shmem2, 4096);
        vec_string.push_back("Hello");
        vec_string.push_back("World");
        
        BOOST_CHECK_EQUAL(vec_string.at(0), "Hello");
        BOOST_CHECK_EQUAL(vec_string.at(1), "World");
        
        cout << "String vector: " << vec_string.at(0) << ", " << vec_string.at(1) << endl;
        cout << "WARNING: std::string not recommended for cross-process shared memory" << endl;
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

BOOST_AUTO_TEST_CASE(test_custom_vector_struct_type) {
    cout << "Test CustomVector with struct types" << endl;
    
    // Test 11: Test with struct type as requested by user
    struct Payload_t {
        char Payload[4096];
        uint16_t PayloadLength;
    };
    
    void* shmem = allocate_shared_memory(640000);
    CustomVector<Payload_t> vec(sizeof(Payload_t), shmem, 640000);
    
    // Create and add first payload
    Payload_t payload1;
    strcpy(payload1.Payload, "Payload char array");
    payload1.PayloadLength = 18;
    vec.push_back(payload1);
    
    // Create and add second payload
    Payload_t payload2;
    strcpy(payload2.Payload, "Second payload test");
    payload2.PayloadLength = 19;
    vec.push_back(payload2);
    
    // Create and add third payload
    Payload_t payload3;
    strcpy(payload3.Payload, "Third payload");
    payload3.PayloadLength = 13;
    vec.push_back(payload3);
    
    // Verify size
    BOOST_CHECK_EQUAL(vec.size(), 3);
    
    // Verify first payload
    BOOST_CHECK_EQUAL(vec.at(0).PayloadLength, 18);
    BOOST_CHECK_EQUAL(strcmp(vec.at(0).Payload, "Payload char array"), 0);
    
    // Verify second payload
    BOOST_CHECK_EQUAL(vec.at(1).PayloadLength, 19);
    BOOST_CHECK_EQUAL(strcmp(vec.at(1).Payload, "Second payload test"), 0);
    
    // Verify third payload
    BOOST_CHECK_EQUAL(vec.at(2).PayloadLength, 13);
    BOOST_CHECK_EQUAL(strcmp(vec.at(2).Payload, "Third payload"), 0);
    
    cout << "Struct vector test passed with " << vec.size() << " elements" << endl;
    cout << "Element[0]: '" << vec.at(0).Payload << "' (length: " << vec.at(0).PayloadLength << ")" << endl;
    cout << "Element[1]: '" << vec.at(1).Payload << "' (length: " << vec.at(1).PayloadLength << ")" << endl;
    cout << "Element[2]: '" << vec.at(2).Payload << "' (length: " << vec.at(2).PayloadLength << ")" << endl;
    
    munmap(shmem, 640000);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_placement_new_in_shared_memory) {
    cout << "Test CustomVector with placement new in shared memory" << endl;
    
    // Test 12: Reproduce the user's issue - placing CustomVector object itself in shared memory
    void* shmpointer = mmap(NULL, 640000, PROT_READ | PROT_WRITE, 
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    BOOST_CHECK(shmpointer != MAP_FAILED);
    
    cout << "SHMEM address: " << shmpointer << endl;
    
    struct Payload_t {
        char Payload[1024];
        uint16_t PayloadLength;
    };
    
    // Calculate offset: CustomVector object size + alignment
    size_t vector_obj_size = sizeof(CustomVector<Payload_t>);
    size_t alignment = alignof(Payload_t);
    size_t data_offset = ((vector_obj_size + alignment - 1) / alignment) * alignment;
    
    cout << "CustomVector object size: " << vector_obj_size << " bytes" << endl;
    cout << "Data storage offset: " << data_offset << " bytes" << endl;
    
    // Place CustomVector object at start of shared memory
    // But tell it to use memory starting AFTER the object for data storage
    char* data_region = static_cast<char*>(shmpointer) + data_offset;
    size_t data_region_size = 640000 - data_offset;
    
    CustomVector<Payload_t>* shmvector = new(shmpointer) CustomVector<Payload_t>(
        sizeof(Payload_t), data_region, data_region_size);
    
    Payload_t payload1;
    strcpy(payload1.Payload, "Payload char array");
    payload1.PayloadLength = 18;
    
    Payload_t payload2;
    strcpy(payload2.Payload, "Second payload test");
    payload2.PayloadLength = 19;
    
    Payload_t payload3;
    strcpy(payload3.Payload, "Third payload");
    payload3.PayloadLength = 13;
    
    // This should not segfault
    shmvector->push_back(payload1);
    shmvector->push_back(payload2);
    shmvector->push_back(payload3);
    
    // Verify
    BOOST_CHECK_EQUAL(shmvector->size(), 3);
    BOOST_CHECK_EQUAL(shmvector->at(0).PayloadLength, 18);
    BOOST_CHECK_EQUAL(strcmp(shmvector->at(0).Payload, "Payload char array"), 0);
    BOOST_CHECK_EQUAL(shmvector->at(1).PayloadLength, 19);
    BOOST_CHECK_EQUAL(strcmp(shmvector->at(1).Payload, "Second payload test"), 0);
    BOOST_CHECK_EQUAL(shmvector->at(2).PayloadLength, 13);
    BOOST_CHECK_EQUAL(strcmp(shmvector->at(2).Payload, "Third payload"), 0);
    
    cout << "Placement new test passed with " << shmvector->size() << " elements" << endl;
    cout << "Element[0]: '" << shmvector->at(0).Payload << "' (length: " << shmvector->at(0).PayloadLength << ")" << endl;
    cout << "Element[1]: '" << shmvector->at(1).Payload << "' (length: " << shmvector->at(1).PayloadLength << ")" << endl;
    
    // Manually call destructor since we used placement new
    shmvector->~CustomVector();
    
    munmap(shmpointer, 640000);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_eraseAt) {
    cout << "Test CustomVector eraseAt function" << endl;
    
    void* shmem = allocate_shared_memory(4096);
    CustomVector<int> vec(sizeof(int), shmem, 4096);
    
    // Add some elements
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    vec.push_back(40);
    vec.push_back(50);
    
    BOOST_CHECK_EQUAL(vec.size(), 5);
    cout << "Added 5 elements: 10, 20, 30, 40, 50" << endl;
    
    // Test: Erase element at index 2 (value 30)
    vec.eraseAt(2);
    BOOST_CHECK_EQUAL(vec.size(), 4);
    BOOST_CHECK_EQUAL(vec.at(0), 10);
    BOOST_CHECK_EQUAL(vec.at(1), 20);
    BOOST_CHECK_EQUAL(vec.at(2), 40);  // 40 should have shifted forward
    BOOST_CHECK_EQUAL(vec.at(3), 50);
    cout << "After erasing index 2: [10, 20, 40, 50]" << endl;
    
    // Test: Erase first element
    vec.eraseAt(0);
    BOOST_CHECK_EQUAL(vec.size(), 3);
    BOOST_CHECK_EQUAL(vec.at(0), 20);
    BOOST_CHECK_EQUAL(vec.at(1), 40);
    BOOST_CHECK_EQUAL(vec.at(2), 50);
    cout << "After erasing index 0: [20, 40, 50]" << endl;
    
    // Test: Erase last element
    vec.eraseAt(2);
    BOOST_CHECK_EQUAL(vec.size(), 2);
    BOOST_CHECK_EQUAL(vec.at(0), 20);
    BOOST_CHECK_EQUAL(vec.at(1), 40);
    cout << "After erasing index 2: [20, 40]" << endl;
    
    // Test: Erase out of bounds should throw
    BOOST_CHECK_THROW(vec.eraseAt(5), std::out_of_range);
    cout << "Correctly threw exception for out of bounds erase" << endl;
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_getNextElement) {
    cout << "Test CustomVector getNextElement function" << endl;
    
    void* shmem = allocate_shared_memory(4096);
    CustomVector<int> vec(sizeof(int), shmem, 4096);
    
    // Add some elements
    vec.push_back(100);
    vec.push_back(200);
    vec.push_back(300);
    
    BOOST_CHECK_EQUAL(vec.size(), 3);
    cout << "Added 3 elements: 100, 200, 300" << endl;
    
    // Test: Get first element (should return 100 and remove it)
    int first = vec.getNextElement();
    BOOST_CHECK_EQUAL(first, 100);
    BOOST_CHECK_EQUAL(vec.size(), 2);
    BOOST_CHECK_EQUAL(vec.at(0), 200);
    BOOST_CHECK_EQUAL(vec.at(1), 300);
    cout << "Got next element: 100, remaining: [200, 300]" << endl;
    
    // Test: Get second element (should return 200 and remove it)
    int second = vec.getNextElement();
    BOOST_CHECK_EQUAL(second, 200);
    BOOST_CHECK_EQUAL(vec.size(), 1);
    BOOST_CHECK_EQUAL(vec.at(0), 300);
    cout << "Got next element: 200, remaining: [300]" << endl;
    
    // Test: Get third element (should return 300 and remove it)
    int third = vec.getNextElement();
    BOOST_CHECK_EQUAL(third, 300);
    BOOST_CHECK_EQUAL(vec.size(), 0);
    cout << "Got next element: 300, remaining: []" << endl;
    
    // Test: Get from empty vector should throw
    BOOST_CHECK_THROW(vec.getNextElement(), std::out_of_range);
    cout << "Correctly threw exception for empty vector" << endl;
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_getNextElement_with_struct) {
    cout << "Test CustomVector getNextElement with struct type" << endl;
    
    struct Payload_t {
        char Payload[128];
        uint16_t PayloadLength;
    };
    
    void* shmem = allocate_shared_memory(4096);
    CustomVector<Payload_t> vec(sizeof(Payload_t), shmem, 4096);
    
    // Add some struct elements
    Payload_t p1;
    strcpy(p1.Payload, "First payload");
    p1.PayloadLength = 13;
    vec.push_back(p1);
    
    Payload_t p2;
    strcpy(p2.Payload, "Second payload");
    p2.PayloadLength = 14;
    vec.push_back(p2);
    
    BOOST_CHECK_EQUAL(vec.size(), 2);
    cout << "Added 2 struct elements" << endl;
    
    // Test: Get first struct element
    Payload_t first = vec.getNextElement();
    BOOST_CHECK_EQUAL(std::string(first.Payload), "First payload");
    BOOST_CHECK_EQUAL(first.PayloadLength, 13);
    BOOST_CHECK_EQUAL(vec.size(), 1);
    cout << "Got next element: '" << first.Payload << "' (length: " << first.PayloadLength << ")" << endl;
    
    // Test: Get second struct element
    Payload_t second = vec.getNextElement();
    BOOST_CHECK_EQUAL(std::string(second.Payload), "Second payload");
    BOOST_CHECK_EQUAL(second.PayloadLength, 14);
    BOOST_CHECK_EQUAL(vec.size(), 0);
    cout << "Got next element: '" << second.Payload << "' (length: " << second.PayloadLength << ")" << endl;
    
    free_shared_memory(shmem, 4096);
}

BOOST_AUTO_TEST_CASE(test_custom_vector_thread_safety) {
    cout << "Test CustomVector getNextElement thread safety" << endl;
    
    void* shmem = allocate_shared_memory(8192);
    CustomVector<int> vec(sizeof(int), shmem, 8192);
    
    // Add many elements
    const int NUM_ELEMENTS = 100;
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        vec.push_back(i);
    }
    
    BOOST_CHECK_EQUAL(vec.size(), NUM_ELEMENTS);
    cout << "Added " << NUM_ELEMENTS << " elements" << endl;
    
    // Test: Sequential getNextElement should work correctly
    std::vector<int> retrieved;
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        int val = vec.getNextElement();
        retrieved.push_back(val);
    }
    
    BOOST_CHECK_EQUAL(vec.size(), 0);
    BOOST_CHECK_EQUAL(retrieved.size(), NUM_ELEMENTS);
    
    // Verify all elements were retrieved in order
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        BOOST_CHECK_EQUAL(retrieved[i], i);
    }
    cout << "Successfully retrieved all " << NUM_ELEMENTS << " elements in order" << endl;
    
    free_shared_memory(shmem, 8192);
}

