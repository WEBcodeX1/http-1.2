#ifndef ASProcessHandler_hpp
#define ASProcessHandler_hpp

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include <atomic>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <filesystem>

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


class ASProcessHandler : public SHMPythonAS, private CPU
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

    string ReqPayloadString;
    string ResultString;

    boost::python::object PyClass;

    private:

    Namespaces_t _Namespaces;
    VHostOffsetsPrecalc_t _VHostOffsetsPrecalc;
};

#endif
