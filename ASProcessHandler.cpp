#include "ASProcessHandler.hpp"

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
        ASInterpreterCount += NamespaceProps.second.InterpreterCount;
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

                //- set process name
                std::string ProcessNameDyn = "python-as" + std::to_string(Index);
                const char* ProcessName = ProcessNameDyn.c_str();
                prctl(PR_SET_NAME, (unsigned long) ProcessName);

                //- drop privileges
                Permission::dropPrivileges(2000, 2000);

                //- overwrite parent termination handler
                setTerminationHandler();

                DBG(120, "Process UID:" << getuid() << " GID:" << getgid());

                const std::string PythonPath = "/var/www" + Namespace.second.PathRel + STATICFS_SUBDIR + PYTHONAS_SUBDIR;

                setenv("PYTHONPATH", PythonPath.c_str(), 1);

                //- get parent pid filedescriptor
                //pidfd_t ParentPidFD = Syscall::pidfd_open(getppid(), 0);

                const char* Env1 = std::getenv("PATH");
                const char* Env2 = std::getenv("PYTHONPATH");

                DBG(120, "Process PATH:" << Env1 << " PYTHONPATH:" << Env2);

                //std::filesystem::current_path("/var/www/app1/python");

                //DBG(120, "Child ASProcessHandler Process PID:" << getpid() << " ParentPidFD:" << ParentPidFD);
                //DBG(120, "Child ASProcessHandler SharedMemAddress:" << SharedMemBaseAddr);

                DBG(120, "Python Path:" << PythonPath.c_str());

                Py_InitializeEx(0);

                int rc = Py_IsInitialized();
                DBG(120, "Python Interpreter initialized:" << rc);

                bp::object PyModule;
                bp::object PyClass;

                try{
                    PyModule = bp::import("WebApp");
                    PyClass = PyModule.attr("BaseClass")();
                } catch(const bp::error_already_set&) {
                    PyErr_Print();
                    DBG(120, "Python Module Import Error.");
                    //std::exit(1);
                }

                //- main loop
                while(true) {

                    DBG(200, "PythonAS Main Loop Index:" << Index);
                    atomic_uint16_t* CanReadAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 0));
                    atomic_uint16_t* WriteReadyAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 1));
                    DBG(200, " CanReadAddr:" << CanReadAddr << " Val:" << *(CanReadAddr) << " WriteReadyAddr:" << WriteReadyAddr << " Val:" << *(WriteReadyAddr));

                    if (*(CanReadAddr) == 1 && *(WriteReadyAddr) == 0) {
                        DBG(-1, "PythonAS invoking!");

                        string ReqJSON = "{\"payload\": 32}";

                        HTTPPayloadLength_t ReqPayloadLength = *(static_cast<HTTPPayloadLength_t*>(getMetaAddress(Index, 6)));

                        char ReqPayload[ReqPayloadLength];
                        memcpy(&ReqPayload[0], static_cast<char*>(getRequestAddress(Index)), ReqPayloadLength);
                        string ReqPayloadString(ReqPayload, ReqPayloadLength);

                        uint r = ReqPayloadString.compare(ReqJSON);
                        DBG(140, "PythonAS Payload:'" << ReqPayloadString << "' ReqJSON:'" << ReqJSON << "' Compare:" << r);

                        string PyResultString = "";
                        try{
                            bp::object PyResult = PyClass.attr("invoke")(ReqPayloadString);
                             PyResultString = to_string(bp::extract<int>(PyResult));
                            DBG(-1, "PythonAS Result:" << PyResultString);
                        } catch(const bp::error_already_set&) {
                            PyErr_Print();
                            DBG(120, "Python call 'invoke()' method Error.");
                        }

                        //- set result payload
                        const char* ResultCString = PyResultString.c_str();
                        char* Payload = new(getResultAddress(Index)) char[PyResultString.length()];
                        memcpy(Payload, &ResultCString[0], PyResultString.length());

                        //- set result payload length
                        new(getMetaAddress(Index, 7)) HTTPPayloadLength_t(PyResultString.length());

                        //- set CanRead and WriteReady
                        new(getMetaAddress(Index, 0)) uint16_t(0);
                        new(getMetaAddress(Index, 1)) uint16_t(1);
                    }

                    this_thread::sleep_for(chrono::microseconds(IDLE_SLEEP_MICROSECONDS));
                    //this_thread::sleep_for(chrono::seconds(1));
                    DBG(300, "Process PythonAS alive:");
                }

                DBG(-1, "Exit Parent ASProcessHandler Process.");
                exit(0);

            }
        }
    }
}
