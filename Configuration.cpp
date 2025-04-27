#include "Configuration.hpp"

using namespace std;

Configuration::Configuration() :
    RunAsUnixUserID(RUNAS_USER_DEFAULT),
    RunAsUnixGroupID(RUNAS_GROUP_DEFAULT)
{
    //- TODO: exception handling
    DBG(120, "Constructor");

    namespace bp = boost::python;

    const string PScript = "/home/cpruefer/Documents/Repos/data-gateway/python/config.py";

    FILE* fp = fopen(PScript.c_str(), "r");
    PyObject *MainModule, *MainDict, *TmpPyObject;

    Py_InitializeEx(0);

    PyRun_SimpleFile(fp, PScript.c_str());

    MainModule = PyImport_ImportModule("__main__");
    MainDict = PyModule_GetDict(MainModule);

    TmpPyObject = PyMapping_GetItemString(MainDict, "server_run_user");
    RunAsUnixUser = PyUnicode_AsUTF8(TmpPyObject);

    TmpPyObject = PyMapping_GetItemString(MainDict, "server_run_group");
    RunAsUnixGroup = PyUnicode_AsUTF8(TmpPyObject);

    TmpPyObject = PyMapping_GetItemString(MainDict, "path_base");
    BasePath = PyUnicode_AsUTF8(TmpPyObject);

    TmpPyObject = PyMapping_GetItemString(MainDict, "server_ip");
    ServerAddress = PyUnicode_AsUTF8(TmpPyObject);

    TmpPyObject = PyMapping_GetItemString(MainDict, "server_port");
    ServerPort = stoi(PyUnicode_AsUTF8(TmpPyObject));

    TmpPyObject = PyMapping_GetItemString(MainDict, "mimetypes");
    bp::list TmpMimetypes = bp::extract<bp::list>(TmpPyObject);

    for (auto i=0; i<len(TmpMimetypes); ++i) {
        Mimetypes.push_back(bp::extract<string>(TmpMimetypes[i]));
    }

    TmpPyObject = PyMapping_GetItemString(MainDict, "namespaces");
    bp::list TmpNamespaces = bp::extract<bp::list>(TmpPyObject);

    for (auto i=0; i<len(TmpNamespaces); ++i) {

        string NamespaceID;
        NamespaceProps_t NamespaceProps;
        string AttributeID;

        DBG(-1, "Namespace:" << i);

        bp::dict TmpAttributesDict = bp::extract<bp::dict>(TmpNamespaces[i]);
        bp::list DictAttributes = TmpAttributesDict.items();

        NamespaceProps.FilesystemRef = nullptr;

        for (auto x=0; x<len(DictAttributes)-1; ++x) {

            DBG(-1, "Attribute Nr:" << x);

            AttributeID = bp::extract<string>(DictAttributes[x][0]);

            const string AttributeValue = bp::extract<string>(DictAttributes[x][1]);

            DBG(-1, "AttributeID:" << AttributeID << " Value:" << AttributeValue);

            if (AttributeID == "hostname") { NamespaceID = AttributeValue; }
            if (AttributeID == "path") { NamespaceProps.PathRel = AttributeValue; }
            if (AttributeID == "interpreters") { NamespaceProps.InterpreterCount = stoi(AttributeValue); }
        }

        DBG(-1, "Namespace:" << NamespaceID << " Path:" << NamespaceProps.PathRel << " Interpreters:" << NamespaceProps.InterpreterCount);

        Namespaces.insert(
            NamespacePair_t(NamespaceID, NamespaceProps)
        );
    }

    Py_FinalizeEx();

}

Configuration::~Configuration()
{
    DBG(120, "Destructor");
}
