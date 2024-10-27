#include "ResultProcessor.hpp"

using namespace std;

static bool RunServer = true;

ResultProcessor::ResultProcessor()
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

void ResultProcessor::setVHostOffsets(VHostOffsetsPrecalc_t VHostOffsets) {
    _VHostOffsetsPrecalc = VHostOffsets;
}

void ResultProcessor::forkProcessResultProcessor(ResultProcessorSHMPointer_t SHMAdresses)
{
    //- reset atomic lock
    atomic_uint16_t* StaticFSLock = new(SHMAdresses.StaticFSPtr) atomic_uint16_t(0);

    //- set as memory base addresses
    setBaseAddresses( { SHMAdresses.PostASMetaPtr, SHMAdresses.PostASRequestsPtr, SHMAdresses.PostASResultsPtr } );

    _ForkResult = fork();

    if (_ForkResult == -1) {
        ERR("ResultProcessor Process fork() error.");
        exit(0);
    }

    if (_ForkResult > 0) {        
        DBG(120, "Parent ResultProcessor Process PID:" << getpid());
        DBG(120, "Parent ResultProcessor Atomic Address:" << StaticFSLock);
    }

    if (_ForkResult == 0) {

        //- get parent pid filedescriptor
        _ParentPidFD = Syscall::pidfd_open(getppid(), 0);
        THsetGlobalData(_ParentPidFD, _Namespaces);

        //- overwrite parent termination handler
        setTerminationHandler();

        DBG(120, "Child ResultProcessor Process PID:" << getpid() << " ParentPidFD:" << _ParentPidFD);
        DBG(120, "Child ResultProcessor SharedMemAddress:" << SHMAdresses.StaticFSPtr);
        DBG(120, "Child ResultProcessor Atomic Address:" << StaticFSLock << " Value:" << *(StaticFSLock));

        //- main loop until terminate signal raised
        while(RunServer) {

            DBG(250, "ResultProcessor Address:" << StaticFSLock << " Atomic Lock = " << *(StaticFSLock));

            bool WorkDone = false;

            ResultOrder::reset();

            if (*(StaticFSLock) == 1)
            {
                void* ClientCountAddr = static_cast<char*>(SHMAdresses.StaticFSPtr) + sizeof(atomic_uint16_t);
                uint16_t ClientCount = *(static_cast<uint16_t*>(ClientCountAddr));

                DBG(120, "StaticFS GET Request ClientCount = " << ClientCount);

                if (ClientCount == 0) {
                    ERR("ClientCount == 0 should never happen!");
                    this_thread::sleep_for(chrono::microseconds(IDLE_SLEEP_MICROSECONDS));
                }
                if (ClientCount > 0) {

                    DBG(120, "Child ResultProcessor - processing client requests PID:" << getpid() << " ClientCount:" << ClientCount);

                    //- set shared memory handler base address
                    void* CientDataMemAddr = static_cast<char*>(SHMAdresses.StaticFSPtr) + sizeof(atomic_uint16_t) + sizeof(uint16_t);
                    setBaseAddress(CientDataMemAddr);

                    _processStaticFSRequests(ClientCount);
                }
                *(StaticFSLock) = 0;
                WorkDone = true;
            }

            uint16_t WorkDoneASResults = _processPythonASResults();

            ResultOrder::calculate();
            ResultOrder::processRequests(HTTP1_2);
            ResultOrder::processRequests(HTTP1_1);

            THprocessThreads();

            if (WorkDone == true || WorkDoneASResults > 0) {
                WorkDone = true;
            }

            if (WorkDone == false) {
                this_thread::sleep_for(chrono::microseconds(IDLE_SLEEP_MICROSECONDS));
            }
        }

        delete StaticFSLock;

        DBG(-1, "Exit Parent ResultProcessor Process.");
        exit(0);
    }
}

void ResultProcessor::_processStaticFSRequests(uint16_t RequestCount)
{
    for (uint16_t i=0; i<RequestCount; ++i) {

        void* ClientFDAddr = static_cast<char*>(getCurrentOffsetAddress());
        uint16_t ClientFD = *(static_cast<uint16_t*>(ClientFDAddr));

        void* HTTPVersionAddr = static_cast<char*>(getNextAddress());
        uint16_t HTTPVersion = *(static_cast<uint16_t*>(HTTPVersionAddr));

        void* ReqNrAddr = static_cast<char*>(getNextAddress());
        uint16_t ReqNr = *(static_cast<uint16_t*>(ReqNrAddr));

        void* ClientPayloadLengthAddr = static_cast<char*>(getNextAddress());
        uint16_t ClientPayloadLength = *(static_cast<uint16_t*>(ClientPayloadLengthAddr));

        void* ClientPayloadAddr = static_cast<char*>(getNextAddress());
        DBG(120, "MemAddr ClientFD:" << ClientFDAddr << " ReqNr:" << ReqNrAddr << " PayloadLength:" << ClientPayloadLengthAddr << " HTTPPayload:" << ClientPayloadAddr);

        char ClientPayload[ClientPayloadLength];
        memcpy(&ClientPayload[0], ClientPayloadAddr, ClientPayloadLength);
        string ClientPayloadString(ClientPayload, ClientPayloadLength);

        getNextAddress(ClientPayloadLength);
        DBG(120, "ClientFD:" << ClientFD << " ReqNr:" << ReqNr << " HTTpVersion:" << HTTPVersion << " PayloadLength:" << ClientPayloadLength << " Payload:'" << ClientPayloadString << "'");

        ClientFD_t ClientFDShared = Syscall::pidfd_getfd(_ParentPidFD, ClientFD, 0);

        RequestProps_t Request;

        Request.ClientFD = ClientFD;
        Request.ClientFDShared = ClientFDShared;
        Request.HTTPPayload = ClientPayloadString;
        Request.HTTPVersion = HTTPVersion;
        Request.PayloadLength = ClientPayloadLength;
        Request.ASIndex = -1;

        ResultOrder::append(ReqNr, std::move(Request));
    }
}

uint16_t ResultProcessor::_processPythonASResults()
{
    uint16_t processed = 0;

    for (const auto& Namespace: _Namespaces) {
        for (const auto &Index: _VHostOffsetsPrecalc.at(Namespace.first)) {
            atomic_uint16_t* CanReadAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 0));
            atomic_uint16_t* WriteReadyAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 1));
            if (*(CanReadAddr) == 0 && *(WriteReadyAddr) == 1) {

                DBG(120, "PythonAS Index:" << Index << " CanRead=0 WriteReady=1");

                RequestProps_t Request;

                Request.ClientFD = *(static_cast<ClientFD_t*>(getMetaAddress(Index, 2)));
                Request.ClientFDShared = Syscall::pidfd_getfd(_ParentPidFD, Request.ClientFD, 0);
                Request.HTTPVersion = *(static_cast<HTTPVersionType_t*>(getMetaAddress(Index, 3)));
                Request.PayloadLength = *(static_cast<HTTPPayloadLentgh_t*>(getMetaAddress(Index, 7)));

                char HTTPPayload[Request.PayloadLength];
                memcpy(&HTTPPayload[0], static_cast<char*>(getResultAddress(Index)), Request.PayloadLength);
                string ClientPayloadString(HTTPPayload, Request.PayloadLength);

                Request.HTTPPayload = HTTPPayload;
                Request.ASIndex = Index;

                DBG(120, "PythonAS Index:" << Index << " ClientFD:" << Request.ClientFD << " ClientFDShared:" << Request.ClientFDShared << " HTTPType:" << Request.HTTPVersion << " PayloadLength:" << Request.PayloadLength);

                ResultOrder::append(*(static_cast<HTTPMethodType_t*>(getMetaAddress(Index, 5))), std::move(Request));

                new(getMetaAddress(Index, 0)) uint16_t(0);
                new(getMetaAddress(Index, 1)) uint16_t(0);

                processed += 1;
            }
        }
    }
    return processed;
}
