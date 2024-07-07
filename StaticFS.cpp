#include "StaticFS.hpp"

using namespace std;

static bool RunServer = true;

StaticFS::StaticFS() :
    _ThreadHandler(new ThreadHandler())
{
    DBG(120, "Constructor");
}

StaticFS::~StaticFS()
{
    DBG(120, "Destructor");
}

void StaticFS::setTerminationHandler()
{
    DBG(-1, "Setting StaticFS SIGTERM handler.");
    signal(SIGTERM, StaticFS::terminate);
}

void StaticFS::terminate(int _ignored)
{
    DBG(-1, "SIGTERM StaticFS received, shutting down");
    RunServer = false;
}

void StaticFS::loadFilesystemData(
        Namespaces_t Namespaces,
        string BasePath,
        Mimetypes_t Mimetypes
){
    for (auto& [Key, Value]: Namespaces) {

        Filesystem* FilesysRef = new Filesystem();

        DBG(120, "Host:" << Key << " Path:" << Value.PathRel << " InterpreterCount:" << Value.InterpreterCount);

        FilesysRef->Hostname = Key;
        FilesysRef->Path = Value.PathRel;
        FilesysRef->BasePath = BasePath;
        FilesysRef->Mimetypes = Mimetypes;

        FilesysRef->initFiles();
        FilesysRef->processFileProperties();

        std::shared_ptr<Filesystem> FilesysRefPtr(FilesysRef);
        _Namespaces[Key].FilesystemRef = std::move(FilesysRefPtr);
    }
}

void StaticFS::forkFilesystemProcess(void* SharedMemBaseAddr)
{
    //- reset atomic lock
    atomic_int64_t* AtomicLockAddr = new(SharedMemBaseAddr) atomic_int64_t(0);

    _ForkResult = fork();

    if (_ForkResult == -1) {
        ERR("StaticFS Process fork() error.");
        exit(0);
    }

    if (_ForkResult > 0) {        
        DBG(120, "Parent StaticFS Process PID:" << getpid());
        DBG(120, "Parent StaticFS Atomic Address:" << AtomicLockAddr);
    }

    if (_ForkResult == 0) {

        //- get parent pid filedescriptor
        _ParentPidFD = Syscall::pidfd_open(getppid(), 0);
        _ThreadHandler->_setGlobalData(_ParentPidFD, _Namespaces);

        //- overwrite parent termination handler
        setTerminationHandler();

        DBG(120, "Child StaticFS Process PID:" << getpid() << " ParentPidFD:" << _ParentPidFD);
        DBG(120, "Child StaticFS SharedMemAddress:" << SharedMemBaseAddr);
        DBG(120, "Child StaticFS Atomic Address:" << AtomicLockAddr << " Value:" << *(AtomicLockAddr));

        //- static fs main loop
        while(RunServer) {

            //DBG(120, "StaticFS Address:" << AtomicLockAddr << " Atomic Lock = " << *(AtomicLockAddr));

            if (*(AtomicLockAddr) == 1)
            {
                void* ClientCountAddr = static_cast<char*>(SharedMemBaseAddr) + sizeof(atomic_int64_t);
                uint16_t ClientCount = *(static_cast<uint16_t*>(ClientCountAddr));

                DBG(120, "StaticFS ClientCount = " << ClientCount);

                if (ClientCount == 0) {
                    ERR("ClientCount == 0 should never happen!");
                    this_thread::sleep_for(chrono::milliseconds(IDLE_SLEEP_MILLISECONDS));
                }
                if (ClientCount > 0) {

                    DBG(120, "Child StaticFS - processing client requests PID:" << getpid() << " ClientCount:" << ClientCount);

                    //- set shared memory handler base address
                    void* CientDataMemAddr = static_cast<char*>(SharedMemBaseAddr) + sizeof(atomic_int64_t) + sizeof(uint16_t);
                    setSharedMemBaseAddress(CientDataMemAddr);

                    _processClients(ClientCount);
                }
                *(AtomicLockAddr) = 0;
            }
            else {
                this_thread::sleep_for(chrono::milliseconds(IDLE_SLEEP_MILLISECONDS));
            }
        }
        DBG(-1, "Exit Parent StaticFS Process.");
        exit(0);
    }
}

void StaticFS::_processClients(uint16_t RequestCount)
{
    for (uint16_t i=0; i<RequestCount; ++i) {

        void* ClientFDAddr = static_cast<char*>(getCurrentOffsetAddress());
        uint16_t ClientFD = *(static_cast<uint16_t*>(ClientFDAddr));

        void* ClientMsgLengthAddr = static_cast<char*>(getNextSharedMemAddress());
        uint16_t ClientMsgLength = *(static_cast<uint16_t*>(ClientMsgLengthAddr));

        void* ClientMsgAddr = static_cast<char*>(getNextSharedMemAddress());
        DBG(120, "MemAddr ClientFD:" << ClientFDAddr << " MsgLength:" << ClientMsgLengthAddr << " HTTPMessage:" << ClientMsgAddr);

        char ClientMsg[ClientMsgLength];
        memcpy(&ClientMsg[0], ClientMsgAddr, ClientMsgLength);
        string ClientMsgString(ClientMsg, ClientMsgLength);

        getNextSharedMemAddress(ClientMsgLength);
        DBG(120, "ClientFD:" << ClientFD << " MessageLength:" << ClientMsgLength << " Message:'" << ClientMsgString << "'");

        ClientFD_t ClientFDShared = Syscall::pidfd_getfd(_ParentPidFD, ClientFD, 0);

        _ThreadHandler->_addClient({ClientFD, ClientFDShared, ClientMsgLength, ClientMsgString});
    }

    _ThreadHandler->_startProcessingThreads();
    _ThreadHandler->_checkProcessed();
}
