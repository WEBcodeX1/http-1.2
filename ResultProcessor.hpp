#ifndef ResultProcessor_hpp
#define ResultProcessor_hpp

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <atomic>
#include <thread>

#include "CPU.hpp"
#include "Global.hpp"
#include "IPCHandler.hpp"
#include "Configuration.hpp"
#include "ThreadHandler.hpp"

typedef struct {
    void* StaticFSPtr;
    void* PostASRequestsPtr;
    void* PostASResultsPtr;
} ResultProcessorSHMPointer_t;


class ResultProcessor : public SHMStaticFS, public CPU, private ThreadHandler
{

public:

    ResultProcessor();
    ~ResultProcessor();

    void loadStaticFSData(Namespaces_t, string, Mimetypes_t);
    void forkProcessResultProcessor(ResultProcessorSHMPointer_t);
    void setTerminationHandler();

    static void terminate(int);

private:

    void _processClients(uint16_t);
    inline void _parseHTTPBaseProps(string&);

    pid_t _ForkResult;
    pidfd_t _ParentPidFD;
    Namespaces_t _Namespaces;

    ThreadHandler* _ThreadHandler;

};

#endif
