#ifndef CPU_hpp
#define CPU_hpp

#include <vector>
#include <thread>

#include "Debug.cpp"

using namespace std;

typedef vector<uint8_t> CPUConfig_t;


class CPU {

public:

    CPU();
    ~CPU();

    void setCPUConfig(const CPUConfig_t);
    void applyCPUConfigProcess();
    void applyCPUConfigThread(thread*);

private:

    CPUConfig_t _CPUs;
    cpu_set_t _cpuset;
};

#endif
