#include "Configuration.hpp"

using namespace std;

Configuration::Configuration() :
    RunAsUnixUserID(RUNAS_USER_DEFAULT),
    RunAsUnixGroupID(RUNAS_GROUP_DEFAULT)
{
    //- TODO: exception handling
    DBG(120, "Constructor");

    namespace bp = boost::python;

    const string PScript = "/etc/falcon-http/config.py";

    FILE* fp = fopen(PScript.c_str(), "r");
    PyObject *MainModule, *MainDict, *TmpPyObject;
    Py_InitializeEx(0);
    MainModule = PyImport_ImportModule("__main__");
    MainDict = PyModule_GetDict(MainModule);
    PyRun_SimpleFile(fp, PScript.c_str());

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

    for (uint8_t i=0; i<len(TmpMimetypes); ++i) {
        Mimetypes.push_back(bp::extract<string>(TmpMimetypes[i]));
    }

    TmpPyObject = PyMapping_GetItemString(MainDict, "namespaces");
    bp::list TmpNamespaces = bp::extract<bp::list>(TmpPyObject);

    for (uint8_t i=0; i<len(TmpNamespaces); ++i) {

        string NamespaceID;
        NamespaceProps_t NamespaceProps;
        string AttributeID;

        bp::dict TmpAttributesDict = bp::extract<bp::dict>(TmpNamespaces[i]);
        bp::list DictAttributes = TmpAttributesDict.items();
        bp::dict DictNamespace = bp::extract<bp::dict>(DictAttributes[0][1]);
        bp::list NamespaceAttrDict = DictNamespace.items();

        NamespaceProps.FilesystemRef = nullptr;

        for (uint8_t x=0; x<len(NamespaceAttrDict); ++x) {

            AttributeID = bp::extract<string>(NamespaceAttrDict[x][0]);

            const string DebugOut = bp::extract<string>(NamespaceAttrDict[x][1]);

            DBG(-1, "AttributeID:" << AttributeID << " Value:" << DebugOut);

            if (AttributeID == "hostname") { NamespaceID = bp::extract<string>(NamespaceAttrDict[x][1]); }
            if (AttributeID == "path") { NamespaceProps.PathRel = bp::extract<string>(NamespaceAttrDict[x][1]); }
            if (AttributeID == "interpreters") { NamespaceProps.InterpreterCount = stoi(bp::extract<string>(NamespaceAttrDict[x][1]));}
        }

        DBG(-1, "Namespace:" << NamespaceID << " Path:" << NamespaceProps.PathRel << " Interpreters:" << NamespaceProps.InterpreterCount);

        Namespaces.insert(
            NamespacePair_t(NamespaceID, NamespaceProps)
        );
    }
}

Configuration::~Configuration()
{
    DBG(120, "Destructor");
}
