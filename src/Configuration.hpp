#ifndef Configuration_hpp
#define Configuration_hpp

#include <fstream>
#include <string>
#include <ctime>

#include <nlohmann/json.hpp>

#include "Debug.cpp"

#include "IPCHandler.hpp"
#include "Filesystem.hpp"


typedef struct {
    nlohmann::json JSONConfig;
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
