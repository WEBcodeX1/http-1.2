#include "CPU.hpp"

using namespace std;

CPU::CPU()
{
    DBG(120, "Constructor");
}

CPU::~CPU()
{
    DBG(120, "Destructor");
}

void CPU::setCPUConfig(const CPUConfig_t CPUs)
{
    _CPUs = CPUs;
}

void CPU::applyCPUConfigProcess()
{
    DBG(90, "Apply CPU affinity config to current thread.");

    CPU_ZERO(&_cpuset);

    for (auto CpuID:_CPUs) {
        CPU_SET(CpuID, &_cpuset);
    }

    int rc = sched_setaffinity(
        getpid(),
        sizeof(cpu_set_t),
        &_cpuset
    );

    if (rc == -1) {
        ERR("Error setting CPU affinity.");
        exit(1);
    }
}

void CPU::applyCPUConfigThread(thread* ThreadRef)
{
    int rc = pthread_setaffinity_np(
        ThreadRef->native_handle(),
        sizeof(cpu_set_t),
        &_cpuset
    );
    if (rc == -1) {
        ERR("Can not set CPU affinity on thread.");
    }
}
