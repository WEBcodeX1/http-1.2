#include <boost/python.hpp>
#include "../Global.hpp"
#include "../ASProcessHandler.hpp"


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
                //std::exit(1);
            }
        }

        //- process request
        static void process(ASProcessHandler* ProcessHandlerPtr, int Index)
        {
            try{
                bp::object PyResult = ProcessHandlerPtr->PyClass.attr("invoke")(ProcessHandlerPtr->ReqPayloadString);
                ProcessHandlerPtr->ResultString = to_string(bp::extract<int>(PyResult));
                DBG(-1, "PythonAS Result:" << ProcessHandlerPtr->ResultString);
            } catch(const bp::error_already_set&) {
                PyErr_Print();
                DBG(120, "Python call 'invoke()' method Error.");
            }

            //- set result payload
            const char* ResultCString = ProcessHandlerPtr->ResultString.c_str();
            char* Payload = new(ProcessHandlerPtr->getResultAddress(Index)) char[ProcessHandlerPtr->ResultString.length()];
            memcpy(Payload, &ResultCString[0], ProcessHandlerPtr->ResultString.length());

            //- set result payload length
            new(ProcessHandlerPtr->getMetaAddress(Index, 7)) HTTPPayloadLength_t(ProcessHandlerPtr->ResultString.length());
        }

    };
}
