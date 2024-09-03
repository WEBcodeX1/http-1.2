#include "ASProcessHandler.hpp"

using namespace std;

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
    exit(0);
}

void ASProcessHandler::forkProcessASHandler(ASProcessHandlerSHMPointer_t SHMAdresses)
{
    setBaseAddresses( { SHMAdresses.PostASMetaPtr, SHMAdresses.PostASRequestsPtr, SHMAdresses.PostASResultsPtr } );

    for (const auto& Namespace: _Namespaces) {
        for (const auto &Index:_VHostOffsetsPrecalc.at(Namespace.first)) {
            pid_t ForkResult = fork();
            if (ForkResult == -1) {
                ERR("ASProcessHandler Process fork() error.");
                exit(0);
            }
            if (ForkResult > 0) {
                DBG(120, "ASProcessHandler Process ParentPID:" << getpid());
            }

            if (ForkResult == 0) {

                //- get parent pid filedescriptor
                pidfd_t ParentPidFD = Syscall::pidfd_open(getppid(), 0);

                //- overwrite parent termination handler
                //setTerminationHandler();

                DBG(120, "Child ASProcessHandler Process PID:" << getpid() << " ParentPidFD:" << ParentPidFD);
                //DBG(120, "Child ASProcessHandler SharedMemAddress:" << SharedMemBaseAddr);

                namespace bp = boost::python;

                string PythonPath =  "/var/www" + Namespace.second.PathRel + STATICFS_SUBDIR + PYTHONAS_SUBDIR;
                setenv("PYTHONPATH", PythonPath.c_str(), 1);

                DBG(120, "Python Path::" << PythonPath.c_str());

                Py_InitializeEx(0);
                //bp::object PyModule = bp::import("WebApp");
                /*
                try{

                } catch(const bp::error_already_set&) {
                    PyErr_Print();
                }
                */

                //bp::object PyClass = PyModule.attr("BaseClass")();

                //- main loop
                while(1) {
                    DBG(120, "PythonAS Main Loop Index:" << Index);
                    atomic_uint16_t* CanReadAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 0));
                    atomic_uint16_t* WriteReadyAddr = static_cast<atomic_uint16_t*>(getMetaAddress(Index, 1));
                    DBG(120, " CanReadAddr:" << CanReadAddr << " Val:" << *(CanReadAddr) << " WriteReadyAddr:" << WriteReadyAddr << " Val:" << *(WriteReadyAddr));

                    string ReqJSON = "";

                    if (*(CanReadAddr) == 1 && *(WriteReadyAddr) == 0) {
                        DBG(-1, "PythonAS invoking!");
                        //bp::object Result = PyModule.attr("invoke")(ReqJSON);
                        //DBG(-1, "PythonAS Result::" << bp::extract<string>(Result)());
                        new(getMetaAddress(Index, 0)) uint16_t(0);
                        new(getMetaAddress(Index, 1)) uint16_t(1);
                    }

                    this_thread::sleep_for(chrono::seconds(1));
                    //this_thread::sleep_for(chrono::microseconds(IDLE_SLEEP_MICROSECONDS));
                }
                DBG(-1, "Exit Parent ASProcessHandler Process.");
                exit(0);
            }

        }
    }
}
