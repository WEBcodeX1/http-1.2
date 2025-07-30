#include "../Global.hpp"
#include "../ASProcessHandler.hpp"

#include <boost/python.hpp>


namespace bp = boost::python;

namespace Backend {

    class Processor {

    public:

        //- initialization
        static void init(ASProcessHandler* ProcessHandlerPtr)
        {
            Py_InitializeEx(0);

            int rc = Py_IsInitialized();
            DBG(120, "Python Interpreter initialized:" << rc);

            bp::object PyModule;

            try {
                PyModule = bp::import("WebApp");
                ProcessHandlerPtr->PyClass = PyModule.attr("WebApp")();
            } catch(const bp::error_already_set&) {
                PyErr_Print();
                DBG(120, "Python Module Import Error.");
            }
        }

        //- process request
        static void process(ASProcessHandler* ProcessHandlerPtr, int Index)
        {
            DBG(100, "PythonAS Index:" << Index << " call 'invoke(" << ProcessHandlerPtr->ReqPayloadString << ")'.");
            const char* ResultCharArray = "";

            try{
                bp::object PyResult = ProcessHandlerPtr->PyClass.attr("invoke")(ProcessHandlerPtr->ReqPayloadString);
                ResultCharArray = bp::extract<const char*>(PyResult);
                DBG(-1, "PythonAS Result:" << ResultCharArray);
            } catch(const bp::error_already_set&) {
                PyErr_Print();
                DBG(120, "Python call 'invoke()' method Error.");
            }

            //- copy result into correct memory region
            char* Payload = new(ProcessHandlerPtr->getResultAddress(Index)) char[strlen(ResultCharArray)];
            memcpy(Payload, &ResultCharArray[0], strlen(ResultCharArray));

            //- set result payload length
            new(ProcessHandlerPtr->getMetaAddress(Index, 7)) HTTPPayloadLength_t(strlen(ResultCharArray));
        }

    };
}
