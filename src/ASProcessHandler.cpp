#include "ASProcessHandler.hpp"

#if defined(JAVA_BACKEND)
#include "../src/ASBackend/JavaBackend.hpp"
#else
#include "../src/ASBackend/PythonBackend.hpp"
#endif

using namespace std;

static bool RunServer = true;

ASProcessHandler::ASProcessHandler()
{
    DBG(120, "Constructor");
}

ASProcessHandler::~ASProcessHandler()
{
    DBG(120, "Destructor");
}

void ASProcessHandler::setASProcessHandlerNamespaces(Namespaces_t Namespaces)
{
    _Namespaces = Namespaces;
}

void ASProcessHandler::setASProcessHandlerOffsets(VHostOffsetsPrecalc_t Offsets)
{
    _VHostOffsetsPrecalc = Offsets;
}

uint ASProcessHandler::getASInterpreterCount()
{
    uint ASInterpreterCount = 0;
    for (const auto& NamespaceProps:_Namespaces) {
        ASInterpreterCount += uint(NamespaceProps.second.JSONConfig["interpreters"]);
    }
    return ASInterpreterCount;
}

void ASProcessHandler::setTerminationHandler()
{
    DBG(-1, "Setting ASProcessHandler SIGTERM handler.");
    signal(SIGTERM, ASProcessHandler::terminate);
}

void ASProcessHandler::terminate(int _ignored)
{
    DBG(-1, "SIGTERM ASProcessHandler received, shutting down");
    RunServer = false;
}

void ASProcessHandler::forkProcessASHandler(ASProcessHandlerSHMPointer_t SHMAdresses)
{
    namespace bp = boost::python;

    //- disable OS signals SIGINT, SIGPIPE
    Signal::disableSignals();

    //- set shared mem address pointer
    setBaseAddresses( { SHMAdresses.PostASMetaPtr, SHMAdresses.PostASRequestsPtr, SHMAdresses.PostASResultsPtr } );

    for (const auto& Namespace: _Namespaces) {

        for (const auto &Index:_VHostOffsetsPrecalc.at(Namespace.first)) {

            pid_t ForkResult = fork();

            if (ForkResult == -1) {
                ERR("ASProcessHandler Process fork() error.");
                exit(1);
            }
            if (ForkResult > 0) {
                DBG(120, "ASProcessHandler Process ParentPID:" << getpid());
            }

            if (ForkResult == 0) {

                DBG(20, "ASIndex:" << Index << " Process UID:" << getuid() << " GID:" << getgid());

                #if defined(DEBUG_BUILD)
                //- overwrite termination handler (display backtrace)
                std::set_terminate(SigHandler::myterminate);
                #endif

                //- set process name
                std::string ProcessNameDyn = "falcon-as-" + std::to_string(Index);
                const char* ProcessName = ProcessNameDyn.c_str();
                prctl(PR_SET_NAME, (unsigned long) ProcessName);

                //- enable spectre userspace protection
                prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_INDIRECT_BRANCH, PR_SPEC_ENABLE, 0, 0);

                //- drop privileges
                Permission::dropPrivileges(RUNAS_USER_DEFAULT, RUNAS_GROUP_DEFAULT);

                //- overwrite parent termination handler
                setTerminationHandler();

                DBG(120, "Process UID:" << getuid() << " GID:" << getgid());

                const string BackendRootPath = "/var/www" + string(Namespace.second.JSONConfig["path"]) + string(BACKEND_ROOT_PATH);
                setenv("PYTHONPATH", BackendRootPath.c_str(), 1);
                DBG(120, "BackendRootPath Path:" << BackendRootPath.c_str());

                //- set backend working dir
                try {
                    filesystem::current_path(BackendRootPath);
                }
                catch(exception &e) {
                    ERR("Cannot change dir to BackendRootPath Path:" << BackendRootPath.c_str());
                }

                //- get parent pid filedescriptor
                pidfd_t ParentPidFD = Syscall::pidfd_open(getppid(), 0);

                const char* Env1 = std::getenv("PATH");
                const char* Env2 = std::getenv("PYTHONPATH");

                DBG(120, "Process PATH:" << Env1 << " PYTHONPATH:" << Env2);
                DBG(200, "Child ASProcessHandler Process PID:" << getpid() << " ParentPidFD:" << ParentPidFD);

                //- initialize backend
                Backend::Processor::init(this);

                //- main loop
                while(true) {

                    DBG(300, "AppServer Main Loop Index:" << Index);
                    atomic_uint16_t* CanReadAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 0));
                    atomic_uint16_t* WriteReadyAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 1));
                    DBG(300, " CanReadAddr:" << CanReadAddr << " Val:" << *(CanReadAddr) << " WriteReadyAddr:" << WriteReadyAddr << " Val:" << *(WriteReadyAddr));

                    if (*(CanReadAddr) == 1 && *(WriteReadyAddr) == 0) {
                        DBG(-1, "AppServer invoking!");

                        HTTPPayloadLength_t ReqPayloadLength = *(static_cast<HTTPPayloadLength_t*>(getMetaAddress(Index, 6)));
                        char ReqPayload[ReqPayloadLength];
                        memcpy(&ReqPayload[0], static_cast<char*>(getRequestAddress(Index)), ReqPayloadLength);
                        string ReqPayloadStringTmp(ReqPayload, ReqPayloadLength);
                        ReqPayloadString = ReqPayloadStringTmp;

                        Backend::Processor::process(this, Index);

                        //- set CanRead and WriteReady
                        new(getMetaAddress(Index, 0)) uint16_t(0);
                        new(getMetaAddress(Index, 1)) uint16_t(1);
                    }
                    else {
                        this_thread::sleep_for(chrono::microseconds(IDLE_SLEEP_MICROSECONDS));
                        //this_thread::sleep_for(chrono::seconds(1));
                        DBG(300, "AppServer process idle.");
                    }
                }

                DBG(-1, "Exit ASProcessHandler child process.");
                exit(0);

            }
        }
    }
}
