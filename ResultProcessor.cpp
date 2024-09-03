#include "ResultProcessor.hpp"

using namespace std;

static bool RunServer = true;

ResultProcessor::ResultProcessor() :
    _ThreadHandler(new ThreadHandler())
{
    DBG(120, "Constructor");
}

ResultProcessor::~ResultProcessor()
{
    DBG(120, "Destructor");
}

void ResultProcessor::setTerminationHandler()
{
    DBG(-1, "Setting ResultProcessor SIGTERM handler.");
    signal(SIGTERM, ResultProcessor::terminate);
}

void ResultProcessor::terminate(int _ignored)
{
    DBG(-1, "SIGTERM ResultProcessor received, shutting down");
    RunServer = false;
}

void ResultProcessor::loadStaticFSData(
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

void ResultProcessor::forkProcessResultProcessor(ResultProcessorSHMPointer_t SHMAdresses)
{
    //- reset atomic lock
    atomic_int8_t* AtomicLockAddr = new(SHMAdresses.StaticFSPtr) atomic_int8_t(0);

    _ForkResult = fork();

    if (_ForkResult == -1) {
        ERR("ResultProcessor Process fork() error.");
        exit(0);
    }

    if (_ForkResult > 0) {        
        DBG(120, "Parent ResultProcessor Process PID:" << getpid());
        DBG(120, "Parent ResultProcessor Atomic Address:" << AtomicLockAddr);
    }

    if (_ForkResult == 0) {

        //- get parent pid filedescriptor
        _ParentPidFD = Syscall::pidfd_open(getppid(), 0);
        _ThreadHandler->_setGlobalData(_ParentPidFD, _Namespaces);

        //- overwrite parent termination handler
        setTerminationHandler();

        DBG(120, "Child ResultProcessor Process PID:" << getpid() << " ParentPidFD:" << _ParentPidFD);
        DBG(120, "Child ResultProcessor SharedMemAddress:" << SHMAdresses.StaticFSPtr);
        DBG(120, "Child ResultProcessor Atomic Address:" << AtomicLockAddr << " Value:" << *(AtomicLockAddr));

        //- static fs main loop
        while(RunServer) {

            //DBG(120, "ResultProcessor Address:" << AtomicLockAddr << " Atomic Lock = " << *(AtomicLockAddr));

            if (*(AtomicLockAddr) == 1)
            {
                void* ClientCountAddr = static_cast<char*>(SHMAdresses.StaticFSPtr) + sizeof(atomic_int8_t);
                uint16_t ClientCount = *(static_cast<uint16_t*>(ClientCountAddr));

                DBG(120, "ResultProcessor ClientCount = " << ClientCount);

                if (ClientCount == 0) {
                    ERR("ClientCount == 0 should never happen!");
                    this_thread::sleep_for(chrono::microseconds(IDLE_SLEEP_MICROSECONDS));
                }
                if (ClientCount > 0) {

                    DBG(120, "Child ResultProcessor - processing client requests PID:" << getpid() << " ClientCount:" << ClientCount);

                    //- set shared memory handler base address
                    void* CientDataMemAddr = static_cast<char*>(SHMAdresses.StaticFSPtr) + sizeof(atomic_int8_t) + sizeof(uint16_t);
                    setBaseAddress(CientDataMemAddr);

                    _processClients(ClientCount);
                }
                *(AtomicLockAddr) = 0;
            }
            else {
                this_thread::sleep_for(chrono::microseconds(IDLE_SLEEP_MICROSECONDS));
            }
        }
        DBG(-1, "Exit Parent ResultProcessor Process.");
        exit(0);
    }
}

void ResultProcessor::_processClients(uint16_t RequestCount)
{
    for (uint16_t i=0; i<RequestCount; ++i) {

        void* ClientFDAddr = static_cast<char*>(getCurrentOffsetAddress());
        uint16_t ClientFD = *(static_cast<uint16_t*>(ClientFDAddr));

        void* ClientMsgLengthAddr = static_cast<char*>(getNextAddress());
        uint16_t ClientMsgLength = *(static_cast<uint16_t*>(ClientMsgLengthAddr));

        void* ClientMsgAddr = static_cast<char*>(getNextAddress());
        DBG(120, "MemAddr ClientFD:" << ClientFDAddr << " MsgLength:" << ClientMsgLengthAddr << " HTTPMessage:" << ClientMsgAddr);

        char ClientMsg[ClientMsgLength];
        memcpy(&ClientMsg[0], ClientMsgAddr, ClientMsgLength);
        string ClientMsgString(ClientMsg, ClientMsgLength);

        getNextAddress(ClientMsgLength);
        DBG(120, "ClientFD:" << ClientFD << " MessageLength:" << ClientMsgLength << " Message:'" << ClientMsgString << "'");

        ClientFD_t ClientFDShared = Syscall::pidfd_getfd(_ParentPidFD, ClientFD, 0);

        _ThreadHandler->_addClient({ClientFD, ClientFDShared, ClientMsgLength, ClientMsgString});
    }

    _ThreadHandler->_startProcessingThreads();
    _ThreadHandler->_checkProcessed();
}
