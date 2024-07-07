#include <cassert>
#include <string>
#include <iostream>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/python.hpp>
#include <boost/python/dict.hpp>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

using namespace std;


int main()
{
    namespace bp = boost::python;

    //const wchar_t PythonFilename[1024] = L"/home/cpruefer/Repos/data-gateway/python/config.py";
    const string PScript = "/home/cpruefer/Repos/data-gateway/python/config.py";
    //const wstring PScriptWide = L"/home/cpruefer/Repos/data-gateway/python/config.py";

    FILE* fp = fopen(PScript.c_str(), "r");

    PyObject *MainModule, *MainDict, *ResultObj, *ResultObjDict;

    //Py_SetProgramName(PScriptWide.c_str());
    Py_InitializeEx(0);
    //Py_Initialize();

    MainModule = PyImport_ImportModule("__main__");
    MainDict = PyModule_GetDict(MainModule);

    PyRun_SimpleFile(fp, PScript.c_str());
    //PyRun_SimpleString("config='test'");

    ResultObj = PyMapping_GetItemString(MainDict, "config");
    string ResultJSON = PyUnicode_AsUTF8(ResultObj);

    cout << ResultJSON << endl;

    ResultObjDict = PyMapping_GetItemString(MainDict, "config_dict");

    /*
    object MainModuleBP = import("__main__");
    object MainNamespace = MainModuleBP.attr("__dict__");
    */

    bp::dict mn_dict = bp::extract<bp::dict>(ResultObjDict);

    //object test(handle<>(borrowed(ResultObjDict)));

    string result = "";

    bp::list items = mn_dict.items();
    for (uint8_t i=0; i<len(items); ++i) {
        string dict_key = bp::extract<string>(items[i][0]);
        cout << "Root dict key:" << dict_key << endl;
        result = dict_key;
    }

    assert(result == "config");
}
