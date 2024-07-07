#ifndef Configuration_hpp
#define Configuration_hpp

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/python.hpp>
#include <boost/python/dict.hpp>

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <string>
#include <ctime>

#include "Debug.cpp"

#include "IPCHandler.hpp"
#include "Filesystem.hpp"


typedef struct {
    string PathRel;
    unsigned int InterpreterCount;
    std::shared_ptr<Filesystem> FilesystemRef;
} NamespaceProps_t;

typedef unordered_map<string, NamespaceProps_t> Namespaces_t;
typedef pair<string, NamespaceProps_t> NamespacePair_t;
typedef vector<string> Mimetypes_t;

typedef Namespaces_t& NamespacesRef_t;


class Configuration
{

public:

    Configuration();
    ~Configuration();

    string RunAsUnixUser;
    string RunAsUnixGroup;

    uint16_t RunAsUnixUserID;
    uint16_t RunAsUnixGroupID;

    string BasePath;

    string ServerAddress;
    uint16_t ServerPort;

    Mimetypes_t Mimetypes;
    Namespaces_t Namespaces;

};

#endif
