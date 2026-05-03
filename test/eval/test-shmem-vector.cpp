#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "../../src/CustomVector.hpp"

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include <memory>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

BOOST_AUTO_TEST_CASE( test_custom_vector_placed_in_shared_memory )
{
    cout << "Check custom vector inside shared memory segment." << endl;

    void* shmpointer = mmap(NULL, 640000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    cout << "SHMEM address:" << shmpointer << endl;

    struct Payload_t {
        char Payload[1024];
        uint16_t PayloadLength;
    };

    size_t vector_obj_size = sizeof(CustomVector<Payload_t>);
    size_t alignment = alignof(Payload_t);
    size_t data_offset = ((vector_obj_size + alignment - 1) / alignment) * alignment;

    char* data_region = static_cast<char*>(shmpointer) + data_offset;
    size_t data_region_size = 640000 - data_offset;

    CustomVector<Payload_t>* shmvector = new(shmpointer) CustomVector<Payload_t>(
        sizeof(Payload_t),
        data_region,
        data_region_size
    );

    pid_t ForkResult = fork();

    if (ForkResult == -1) {
        exit(1);
    }

    else if (ForkResult > 0) {
        cout << "Parent process start" << endl;

        Payload_t payload1;
        strcpy(payload1.Payload, "Payload char array");
        payload1.PayloadLength = 18;

        Payload_t payload2;
        strcpy(payload2.Payload, "Second payload test");
        payload2.PayloadLength = 19;

        shmvector->push_back(payload1);
        shmvector->push_back(payload2);

        this_thread::sleep_for(chrono::seconds(5));
        cout << "Parent process vector element0:" << shmvector->at(0).PayloadLength << endl;
        BOOST_TEST(shmvector->at(0).PayloadLength == 18);
    }

    else if (ForkResult == 0) {
        cout << "Child process start SHMEM address:" << shmpointer << endl;
        this_thread::sleep_for(chrono::seconds(1));
        //vector<string>* shmvector_child = static_cast<vector<string>*>(shmpointer);
        cout << "Child process vector element1:" << shmvector->at(1).PayloadLength << endl;
        BOOST_TEST(shmvector->at(1).PayloadLength == 19);
    }
}
