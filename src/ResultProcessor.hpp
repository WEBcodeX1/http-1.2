#ifndef ResultProcessor_hpp
#define ResultProcessor_hpp

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h>

#include <atomic>
#include <thread>
#include <linux/seccomp.h>

#include "CPU.hpp"
#include "Global.hpp"
#include "IPCHandler.hpp"
#include "Configuration.hpp"
#include "ResultOrder.hpp"

typedef struct {
    void* StaticFSPtr;
    void* PostASMetaPtr;
    void* PostASRequestsPtr;
    void* PostASResultsPtr;
} ResultProcessorSHMPointer_t;


class ResultProcessor : private SHMStaticFS, public CPU, private ResultOrder, private SHMPythonAS
{

public:

    ResultProcessor();
    ~ResultProcessor();

    void loadStaticFSData(Namespaces_t, string, Mimetypes_t);
    void forkProcessResultProcessor(ResultProcessorSHMPointer_t);
    void setTerminationHandler();
    void setVHostOffsets(VHostOffsetsPrecalc_t);

    static void terminate(int);

private:

    void _processStaticFSRequests(uint16_t);
    inline void _parseHTTPBaseProps(string&);
    uint16_t _processPythonASResults();

    pid_t _ForkResult;
    pidfd_t _ParentPidFD;
    Namespaces_t _Namespaces;
    VHostOffsetsPrecalc_t _VHostOffsetsPrecalc;

};

#endif
