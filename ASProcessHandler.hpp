#ifndef ASProcessHandler_hpp
#define ASProcessHandler_hpp

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <atomic>
#include <cstdlib>

#include "CPU.hpp"
#include "Global.hpp"
#include "IPCHandlerAS.hpp"
#include "Configuration.hpp"
#include "ASRequestHandler.hpp"

typedef struct {
    void* PostASMetaPtr;
    void* PostASRequestsPtr;
    void* PostASResultsPtr;
} ASProcessHandlerSHMPointer_t;


class ASProcessHandler : private SHMPythonAS, private CPU
{

public:

    ASProcessHandler();
    ~ASProcessHandler();

    void forkProcessASHandler(ASProcessHandlerSHMPointer_t);
    void setTerminationHandler();
    void setASProcessHandlerNamespaces(Namespaces_t);
    void setASProcessHandlerOffsets(VHostOffsetsPrecalc_t);
    uint getASInterpreterCount();

    static void terminate(int);

private:

    Namespaces_t _Namespaces;
    VHostOffsetsPrecalc_t _VHostOffsetsPrecalc;

};

#endif
