#ifndef StaticFS_hpp
#define StaticFS_hpp

//#include "Debug.cpp"

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


class StaticFS : public SharedMemManager, public CPU
{

public:

    StaticFS();
    ~StaticFS();

    void loadFilesystemData(Namespaces_t, string, Mimetypes_t);
    void forkFilesystemProcess(void*);
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
